package main

import (
	"fmt"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
)

type Node = tree_sitter.Node

type context struct {
	code []byte
	tree *tree_sitter.Tree
	m    *Module
}

func Generate(code []byte, tree *tree_sitter.Tree) *Module {
	ctx := &context{
		code: code,
		tree: tree,
		m:    new(Module),
	}

	r := tree.RootNode()
	c := r.Walk()
	p := c.GotoFirstChild()
	for p {
		n := c.Node()
		if n.Kind() == "definition" {
			ctx.definition(n)
		}
		p = c.GotoNextSibling()
	}
	return ctx.m
}

func (ctx *context) definition(n *Node) {
	n = n.Child(0)
	switch n.Kind() {
	case "func":
		ctx._func(n)
		break
	case "extern_func":
		break
	case "struct":
		break
	case "interface":
		break
	case "impl":
		break
	}
	fmt.Printf("%s\n", n.Kind())
}

func (ctx *context) _func(n *Node) {
	f := new(Func)
	f.exported = n.ChildByFieldName("exported") != nil
	f.name = ctx.identifierToString(n.ChildByFieldName("name"))
	if r := n.ChildByFieldName("return"); r != nil {
		f.returnType = ctx._type(r)
	}
	//fmt.Printf("%+v\n", f)
	ctx.block(f, n.ChildByFieldName("block"))
	ctx.m.addFunc(f)
}

func (ctx *context) statement(f *Func, n *Node) {
	n = n.Child(0)
	switch n.Kind() {
	case "return":
		ctx._return(f, n)
	}
}

func (ctx *context) block(f *Func, n *Node) {
	cur := n.Walk()
	p := cur.GotoFirstChild()
	for p {
		c := cur.Node()
		fmt.Printf("Kind: %s\n", c.Kind())
		if c.Kind() == "statement" {
			ctx.statement(f, n)
		}
		p = cur.GotoNextSibling()
	}
}

func (ctx *context) _return(f *Func, n *Node) {
	ctx.expression(f n.Child(1)
}

func (ctx *context) expression(b *Block, n *Node) {}

func (ctx *context) _type(n *Node) Type {
	n = n.Child(0)
	switch n.Kind() {
	case "identifier":
		return ctx.identifierToString(n)
	case "array_type":
		return &TypeArray{
			innerType: ctx._type(n.Child(n.ChildCount() - 1)),
		}
	case "pointer_type":
		return &TypePointer{
			innerType: ctx._type(n.Child(n.ChildCount() - 1)),
		}
	default:
		panic(fmt.Sprintf("Invalid type: %s", n.Kind()))
	}
}

func (ctx *context) identifierToString(n *Node) string {
	return n.Utf8Text(ctx.code)
}
