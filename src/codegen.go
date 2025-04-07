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

func codegen() {
	llvm.InitializeAllTargets()
	llvm.InitializeAllTargetMCs()
	llvm.InitializeAllTargetInfos()
	llvm.InitializeAllAsmParsers()
	llvm.InitializeAllAsmPrinters()

	ctx := llvm.NewContext()
	mod := ctx.NewModule("main")

	llvm.AddFunction(mod, "main", llvm.FunctionType(ctx.Int32Type(), nil, false))
	main_func := mod.NamedFunction("main")
	//main_func.SetLinkage(llvm.InternalLinkage)
	block := ctx.AddBasicBlock(main_func, "entry")
	builder := ctx.NewBuilder()
	defer builder.Dispose()
	builder.SetInsertPointAtEnd(block)
	builder.CreateRet(llvm.ConstInt(ctx.Int32Type(), 69420, false))

	err := llvm.VerifyModule(mod, llvm.ReturnStatusAction)
	if err != nil {
		fmt.Printf("failed to verify module: %s\n", err)
		return
	}

	fmt.Printf("%s\n", mod.String())

	// err = outputModuleToAsm(ctx, mod, "demo.ll")
	// if err != nil {
	// 	fmt.Printf("errpr: %s\n", err)
	//
	// }
	fmt.Printf("done\n")
}
