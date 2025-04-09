package main

import (
	"fmt"
	"os"

	tree_sitter_emble "github.com/cxleb/emble/tree-sitter-emble/bindings/go"
	tree_sitter "github.com/tree-sitter/go-tree-sitter"
)

func job(name string) {
	code, err := os.ReadFile(name)
	if err != nil {
		fmt.Printf("Error occured reading file: %s", err)
		return
	}

	parser := tree_sitter.NewParser()
	defer parser.Close()
	parser.SetLanguage(tree_sitter.NewLanguage(tree_sitter_emble.Language()))

	tree := parser.Parse(code, nil)
	defer tree.Close()

	fmt.Println(tree.RootNode().ToSexp())

	_ = Generate(code, tree)
}

func main() {
	job("./demo.emble")
}
