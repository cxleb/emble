package main

import (
	"emble/src/ir"
	"fmt"
	"strconv"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
)

type Node = tree_sitter.Node

type context struct {
	code []byte
	tree *tree_sitter.Tree
	m    *ir.Module
}

func Generate(code []byte, tree *tree_sitter.Tree) *ir.Module {
	ctx := &context{
		code: code,
		tree: tree,
		m:    new(ir.Module),
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
	f := new(ir.Func)
	f.Exported = n.ChildByFieldName("exported") != nil
	f.Name = ctx.string(n.ChildByFieldName("name"))
	if r := n.ChildByFieldName("return"); r != nil {
		f.ReturnType = ctx._type(r)
	}
	// TODO(caleb): insert parameters
	f.Block = f.NewBlock(nil)
	ctx.block(f.Block, n.ChildByFieldName("block"))
	f.Print()
	ctx.m.AddFunc(f)
}

func (ctx *context) statement(b *ir.Block, n *Node) {
	n = n.Child(0)
	switch n.Kind() {
	case "block":
		ctx.block(b, n)
		break
	case "return":
		ctx._return(b, n)
		break
	case "if":
		ctx._if(b, n)
		break
	case "variable":
		ctx.variable(b, n)
		break
	case "function_call":
		ctx.function_call(b, n)
	}
}

func (ctx *context) block(b *ir.Block, n *Node) {
	cur := n.Walk()
	p := cur.GotoFirstChild()
	for p {
		c := cur.Node()
		if c.Kind() == "statement" {
			ctx.statement(b, c)
		}
		p = cur.GotoNextSibling()
	}
}

func (ctx *context) _return(b *ir.Block, n *Node) {
	expr := ctx.expression(b, n.Child(1))
	b.RetValue(expr)
}

func (ctx *context) _if(b *ir.Block, n *Node) {
	c := ctx.expression(b, n.ChildByFieldName("condition"))
	then := b.F.NewBlock(b)
	ctx.block(then, n.ChildByFieldName("then"))
	var _else *ir.Block
	if elseBlock := n.ChildByFieldName("else"); elseBlock != nil {
		_else = b.F.NewBlock(b)
		ctx.block(_else, elseBlock)
	}

	b.If(c, then, _else)
}

func (ctx *context) variable(b *ir.Block, n *Node) {
	isConst := false
	if ctx.string(n.ChildByFieldName("specifier")) == "const" {
		isConst = true
	}
	name := ctx.string(n.ChildByFieldName("name"))
	var _type ir.Type = &ir.TypeName{Inner: "unknown"}
	if t := n.ChildByFieldName("type"); t != nil {
		_type = ctx._type(t)
	}
	equals := ctx.expression(b, n.ChildByFieldName("equals"))
	b.Var(name, _type, isConst, equals)
}

func (ctx *context) expression(b *ir.Block, n *Node) ir.Ref {
	n = n.Child(0)
	switch n.Kind() {
	case "binary_expression":
		return ctx.binaryExpr(b, n)
	case "integer":
		return ctx.integer(b, n)
	case "identifier":
		return ctx.identifier(b, n)
	case "function_call":
		return ctx.function_call(b, n)
	default:
		panic(fmt.Sprintf("Unimplemented expression type: %s", n.Kind()))
	}
}

func (ctx *context) binaryExpr(b *ir.Block, n *Node) ir.Ref {
	left := ctx.expression(b, n.ChildByFieldName("left"))
	op := ctx.string(n.ChildByFieldName("operator"))
	right := ctx.expression(b, n.ChildByFieldName("right"))
	switch op {
	case "+":
		return b.Add(left, right)
	case "==":
		return b.Eq(left, right)
	case "!=":
		return b.NotEq(left, right)
	default:
		panic(fmt.Sprintf("Unimplemented operator: %s", op))
	}
}

func (ctx *context) integer(b *ir.Block, n *Node) ir.Ref {
	i, err := strconv.Atoi(n.Utf8Text(ctx.code))
	// This is probably not going to happen
	if err != nil {
		panic("int was not parsable!")
	}
	return b.Int(i)
}

func (ctx *context) identifier(b *ir.Block, n *Node) ir.Ref {
	return b.Load(ctx.string(n))
}

func (ctx *context) function_call(b *ir.Block, n *Node) ir.Ref {
	name := ctx.string(n.ChildByFieldName("name"))
	return b.Call(name, []ir.Ref{})
}

func (ctx *context) _type(n *Node) ir.Type {
	n = n.Child(0)
	switch n.Kind() {
	case "identifier":
		return &ir.TypeName{
			Inner: ctx.string(n),
		}
	case "array_type":
		return &ir.TypeArray{
			InnerType: ctx._type(n.Child(n.ChildCount() - 1)),
		}
	case "pointer_type":
		return &ir.TypePointer{
			InnerType: ctx._type(n.Child(n.ChildCount() - 1)),
		}
	default:
		panic(fmt.Sprintf("Invalid type: %s", n.Kind()))
	}
}

func (ctx *context) string(n *Node) string {
	return n.Utf8Text(ctx.code)
}
