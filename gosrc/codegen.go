package main

import (
	"fmt"
	"os"
	"reflect"

	"emble/src/ir"

	"tinygo.org/x/go-llvm"
)

func outputModuleToAsm(ctx llvm.Context, mod llvm.Module, path string) error {
	triple := llvm.DefaultTargetTriple()

	target, err := llvm.GetTargetFromTriple(triple)
	if err != nil {
		return err
	}

	fmt.Printf("Triple:       %s\n", triple)
	fmt.Printf("Name:         %s\n", target.Name())
	fmt.Printf("Description:  %s\n", target.Description())

	targetMachine := target.CreateTargetMachine(
		triple,
		"generic",
		"",
		llvm.CodeGenLevelDefault,
		llvm.RelocPIC,
		llvm.CodeModelLarge,
	)

	buffer, err := targetMachine.EmitToMemoryBuffer(mod, llvm.AssemblyFile)
	if err != nil {
		return err
	}

	f, err := os.Create(path)
	if err != nil {
		return err
	}

	f.Write(buffer.Bytes())

	return nil
}

type VariableLocation struct {
	block int
	name  string
}

func getVarLoc(v *ir.Variable) VariableLocation {
	return VariableLocation{
		block: v.Block.Number,
		name:  v.Name,
	}
}

type Func struct {
	fn        llvm.Value
	ty        llvm.Type
	variables map[VariableLocation]llvm.Value
}

type CodeGen struct {
	ctx   llvm.Context
	m     llvm.Module
	b     llvm.Builder
	funcs map[string]*Func
}

func CodeGenInit() *CodeGen {
	llvm.InitializeAllTargets()
	llvm.InitializeAllTargetMCs()
	llvm.InitializeAllTargetInfos()
	llvm.InitializeAllAsmParsers()
	llvm.InitializeAllAsmPrinters()

	ctx := llvm.NewContext()
	b := ctx.NewBuilder()
	c := &CodeGen{
		ctx:   ctx,
		b:     b,
		funcs: make(map[string]*Func),
	}

	return c
}

func (c *CodeGen) Generate(m *ir.Module) {
	c.m = c.ctx.NewModule("root")

	// Firstly, create all the functions
	for _, f := range m.Funcs {
		lt := llvm.FunctionType(c.ctx.Int32Type(), nil, false)
		lf := llvm.AddFunction(c.m, f.Name, lt)
		c.funcs[f.Name] = &Func{
			fn:        lf,
			ty:        lt,
			variables: make(map[VariableLocation]llvm.Value),
		}
	}

	// Perform code gen for all the functions
	for _, f := range m.Funcs {
		c._func(f)
	}

	fmt.Printf("%s\n", c.m.String())

	err := llvm.VerifyModule(c.m, llvm.ReturnStatusAction)
	if err != nil {
		fmt.Printf("failed to verify module: %s\n", err)
		return
	}

	err = outputModuleToAsm(c.ctx, c.m, "demo.asm")
	if err != nil {
		fmt.Printf("error: %s\n", err)
	}

	fmt.Printf("done\n")
}

func (c *CodeGen) _func(f *ir.Func) {
	returned, finalBlock := c.block(f.Block, c.funcs[f.Name])
	if !returned {
		c.b.SetInsertPointAtEnd(finalBlock)
		c.b.CreateRetVoid()
	}
}

func (c *CodeGen) block(b *ir.Block, f *Func) (bool, llvm.BasicBlock) {
	// Create first block
	lb := c.ctx.AddBasicBlock(f.fn, fmt.Sprintf("block%d", b.Number))
	c.b.SetInsertPointAtEnd(lb)

	v := make([]llvm.Value, len(b.Insts))

	// Convert block instructions to llvm IR
	for i, s := range b.Insts {
		switch inst := s.(type) {
		case *ir.InstInt:
			v[i] = llvm.ConstInt(c.ctx.Int32Type(), uint64(inst.Value), false)
			break
		case *ir.InstAdd:
			lhs := v[inst.Lhs]
			rhs := v[inst.Rhs]
			v[i] = c.b.CreateAdd(lhs, rhs, "")
			break
		case *ir.InstRet:
			if inst.HasValue {
				result := v[inst.Value]
				v[i] = c.b.CreateRet(result)
			} else {
				v[i] = c.b.CreateRetVoid()
			}
			return true, lb
		case *ir.InstIf:
			if inst.Else != nil {
				thenReturned, thenBlock := c.block(inst.Then, f)
				elseReturned, elseBlock := c.block(inst.Else, f)

				c.b.SetInsertPointAtEnd(lb)
				c.b.CreateCondBr(v[inst.Condition], thenBlock, elseBlock)

				// if both branches returned eventually, then just return, we are done
				if thenReturned && elseReturned {
					return true, lb
				}

				lb = c.ctx.AddBasicBlock(f.fn, fmt.Sprintf("block%d_%d", b.Number, i))
				if !thenReturned {
					c.b.SetInsertPointAtEnd(thenBlock)
					c.b.CreateBr(lb)
				}
				if !elseReturned {
					c.b.SetInsertPointAtEnd(elseBlock)
					c.b.CreateBr(lb)
				}
				c.b.SetInsertPointAtEnd(lb)
			} else {
				thenReturned, thenBlock := c.block(inst.Then, f)

				newBlock := c.ctx.AddBasicBlock(f.fn, fmt.Sprintf("block%d_%d", b.Number, i))
				c.b.SetInsertPointAtEnd(lb)
				c.b.CreateCondBr(v[inst.Condition], thenBlock, newBlock)

				if !thenReturned {
					c.b.SetInsertPointAtEnd(thenBlock)
					c.b.CreateBr(newBlock)
				}

				lb = newBlock
			}
			break
		case *ir.InstCall:
			fn := c.funcs[inst.Name]
			v[i] = c.b.CreateCall(fn.ty, fn.fn, []llvm.Value{}, "")
			break
		case *ir.InstVar:
			ty := c.toLLVMType(&inst.V.Type)
			a := c.b.CreateAlloca(ty, "")
			f.variables[getVarLoc(inst.V)] = a
			c.b.CreateStore(v[inst.Equals], a)
			inst.V.InsertedAt = i
			break
		case *ir.InstStore:
			c.b.CreateStore(v[inst.Equals], f.variables[getVarLoc(inst.V)])
			break
		case *ir.InstLoad:
			ty := c.toLLVMType(&inst.V.Type)
			v[i] = c.b.CreateLoad(ty, f.variables[getVarLoc(inst.V)], "")
		case *ir.InstNotEq:
			v[i] = c.b.CreateICmp(llvm.IntNE, v[inst.Lhs], v[inst.Rhs], "")
			break
		case *ir.InstEq:
			v[i] = c.b.CreateICmp(llvm.IntEQ, v[inst.Lhs], v[inst.Rhs], "")
			break
		default:
			panic(fmt.Sprintf("Unimplemented IR instruction %s", reflect.TypeOf(inst).String()))
		}
	}

	return false, lb
}

func (c *CodeGen) demo(lf llvm.Value) llvm.BasicBlock {
	// Create block
	lb := c.ctx.AddBasicBlock(lf, fmt.Sprintf("demoblock"))
	builder := c.ctx.NewBuilder()
	defer builder.Dispose()
	builder.SetInsertPointAtEnd(lb)
	builder.CreateRet(llvm.ConstInt(c.ctx.Int32Type(), 69420, false))
	return lb
}

func (c *CodeGen) toLLVMType(ty *ir.Type) llvm.Type {
	return c.ctx.Int32Type()
}
