package main

import (
	"fmt"
	"os"

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

type CodeGen struct {
	ctx llvm.Context
	m   llvm.Module
}

func CodeGenInit() *CodeGen {
	llvm.InitializeAllTargets()
	llvm.InitializeAllTargetMCs()
	llvm.InitializeAllTargetInfos()
	llvm.InitializeAllAsmParsers()
	llvm.InitializeAllAsmPrinters()

	c := &CodeGen{
		ctx: llvm.NewContext(),
	}

	return c
}

func (c *CodeGen) Generate(m *Module) {
	c.m = c.ctx.NewModule("root")

	for _, f := range m.funcs {
		c._func(f)
	}

	err := llvm.VerifyModule(c.m, llvm.ReturnStatusAction)
	if err != nil {
		fmt.Printf("failed to verify module: %s\n", err)
		return
	}

	fmt.Printf("%s\n", c.m.String())

	//err = outputModuleToAsm(c.ctx, c.m, "demo.asm")
	//if err != nil {
	//	fmt.Printf("error: %s\n", err)
	//}

	fmt.Printf("done\n")
}

func (c *CodeGen) _func(f *Func) {
	llvm.AddFunction(c.m, "main", llvm.FunctionType(c.ctx.Int32Type(), nil, false))
	lf := c.m.NamedFunction("main")
	//lf.SetLinkage(llvm.InternalLinkage)

	for _, b := range f.blocks {
		c.block(&b, lf)
	}
}

func (c *CodeGen) block(b *Block, lf llvm.Value) {
	// Create block
	lb := c.ctx.AddBasicBlock(lf, fmt.Sprintf("block%d", b.number))
	builder := c.ctx.NewBuilder()
	defer builder.Dispose()
	builder.SetInsertPointAtEnd(lb)

	v := make([]llvm.Value, len(b.insts))

	// Convert block instructions to llvm IR
	for i, inst := range b.insts {
		switch inst.inst {
		case InstInt:
			v[i] = llvm.ConstInt(c.ctx.Int32Type(), uint64(inst.ops[0]), false)
			break
		case InstAdd:
			lhs := lf.Param(0) //v[inst.ops[0]]
			rhs := v[inst.ops[1]]
			v[i] = builder.CreateAdd(lhs, rhs, "")
			break
		case InstRet:
			result := v[inst.ops[0]]
			v[i] = builder.CreateRet(result)
			break
		case InstCondBr:
			//builder.CreateCondBr()
			break
		default:
			panic(fmt.Sprintf("Unimplemented IR instruction: %d", inst.inst))
		}
	}
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

func (c *CodeGen) toLLVMType(ty *Type) llvm.Type {
	return c.ctx.Int32Type()
}
