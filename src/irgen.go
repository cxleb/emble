package main

import (
	"fmt"
	"strconv"

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
	//fmt.Printf("%s\n", n.Kind())
}

func (ctx *context) _func(n *Node) {
	f := new(Func)
	f.exported = n.ChildByFieldName("exported") != nil
	f.name = ctx.identifier(n.ChildByFieldName("name"))
	if r := n.ChildByFieldName("return"); r != nil {
		f.returnType = ctx._type(r)
	}
	//fmt.Printf("%+v\n", f)
	ctx.block(f, n.ChildByFieldName("block"))
	f.Print()
	ctx.m.addFunc(f)
}

func (ctx *context) statement(f *Func, n *Node) {
	n = n.Child(0)
	switch n.Kind() {
	case "return_stmt":
		ctx._return(f, n)
		break
	}
}

func (ctx *context) block(f *Func, n *Node) {
	cur := n.Walk()
	p := cur.GotoFirstChild()
	for p {
		c := cur.Node()
		if c.Kind() == "statement" {
			ctx.statement(f, c)
		}
		p = cur.GotoNextSibling()
	}
}

func (ctx *context) _return(f *Func, n *Node) {
	expr := ctx.expression(f.lastBlock(), n.Child(1))
	f.lastBlock().ret(expr)
}

func (ctx *context) expression(b *Block, n *Node) int {
	n = n.Child(0)
	switch n.Kind() {
	case "binary_expression":
		return ctx.binaryExpr(b, n)
	case "integer":
		return ctx.integer(b, n)
	default:
		panic("Unimplemented expression type")
	}
}

func (ctx *context) binaryExpr(b *Block, n *Node) int {
	left := ctx.expression(b, n.ChildByFieldName("left"))
	right := ctx.expression(b, n.ChildByFieldName("right"))
	return b.add(left, right)
}

func (ctx *context) integer(b *Block, n *Node) int {
	i, err := strconv.Atoi(n.Utf8Text(ctx.code))
	// This is probably not going to happen
	if err != nil {
		panic("int was not parsable!")
	}
	return b.int(i)
}

func (ctx *context) _type(n *Node) Type {
	n = n.Child(0)
	switch n.Kind() {
	case "identifier":
		return ctx.identifier(n)
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

func (ctx *context) identifier(n *Node) string {
	return n.Utf8Text(ctx.code)
}
