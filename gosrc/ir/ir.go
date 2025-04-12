package ir

import "fmt"

type Type interface {
	String() string
}
type TypeName struct {
	Inner string
}

func (t *TypeName) String() string {
	return t.Inner
}

type TypePointer struct {
	InnerType Type
}

func (t *TypePointer) String() string {
	return fmt.Sprintf("*%s", t.InnerType)
}

type TypeArray struct {
	InnerType Type
}

func (t *TypeArray) String() string {
	return fmt.Sprintf("[]%s", t.InnerType)
}

type Package struct {
	Modules []Module
}

type Module struct {
	Funcs []*Func
}

func (m *Module) AddFunc(f *Func) {
	m.Funcs = append(m.Funcs, f)
}

type Struct struct {
}

type Impl struct {
}

type Func struct {
	Name       string
	Exported   bool
	ReturnType Type
	Block      *Block
	BlockCount int
}

func (f *Func) Print() {
	fmt.Printf("Func %s:\n", f.Name)
	f.Block.Print(0)
}

func (f *Func) NewBlock(parent *Block) *Block {
	b := &Block{
		Number: f.BlockCount,
		F:      f,
		Parent: parent,
		Insts:  make([]Inst, 0),
	}
	f.BlockCount += 1
	return b
}

type Variable struct {
	Name       string
	Type       Type
	Block      *Block
	InsertedAt Ref
	IsParam    bool
	IsConst    bool
}

type Ref = int

type Block struct {
	Number    int
	Parent    *Block
	F         *Func
	Variables []*Variable
	Insts     []Inst
}

func (b *Block) Print(level int) {
	printf(level, "Block %d:\n", b.Number)
	for i, v := range b.Insts {
		printf(level, "\t%d:", i)
		v.Print(level + 1)
	}
}

func (b *Block) findVariable(name string) *Variable {
	s := b
	for s != nil {
		for _, i := range s.Variables {
			if name == i.Name {
				return i
			}
		}
		s = s.Parent
	}
	return nil
}

func (b *Block) Append(i Inst) Ref {
	loc := len(b.Insts)
	b.Insts = append(b.Insts, i)
	return loc
}

func (b *Block) Ret() Ref {
	return b.Append(&InstRet{})
}

func (b *Block) RetValue(value Ref) Ref {
	return b.Append(&InstRet{
		HasValue: true,
		Value:    value,
	})
}

func (b *Block) Add(lhs Ref, rhs Ref) Ref {
	return b.Append(&InstAdd{
		Rhs: rhs,
		Lhs: lhs,
	})
}

func (b *Block) Eq(lhs Ref, rhs Ref) Ref {
	return b.Append(&InstEq{
		Rhs: rhs,
		Lhs: lhs,
	})
}

func (b *Block) NotEq(lhs Ref, rhs Ref) Ref {
	return b.Append(&InstNotEq{
		Rhs: rhs,
		Lhs: lhs,
	})
}

func (b *Block) Int(value int) Ref {
	return b.Append(&InstInt{
		Value: value,
	})
}

func (b *Block) If(condition Ref, then *Block, _else *Block) Ref {
	return b.Append(&InstIf{
		Condition: condition,
		Then:      then,
		Else:      _else,
	})
}

func (b *Block) Var(name string, _type Type, isConst bool, equals Ref) Ref {
	if b.findVariable(name) != nil {
		// TODO(caleb): compiler error
		panic("Variable already exists")
	}
	v := &Variable{
		Name:       name,
		Type:       _type,
		Block:      b,
		InsertedAt: len(b.Insts),
		IsParam:    false,
		IsConst:    isConst,
	}
	b.Variables = append(b.Variables, v)
	return b.Append(&InstVar{
		V:      v,
		Equals: equals,
	})
}

func (b *Block) Load(name string) Ref {
	v := b.findVariable(name)
	if v == nil {
		// TODO(caleb): compiler error
		panic("Could not find variable")
	}
	return b.Append(&InstLoad{
		V: v,
	})
}

func (b *Block) Store(name string, equals Ref) Ref {
	v := b.findVariable(name)
	if v == nil {
		// TODO(caleb): compiler error
		panic("Could not find variable")
	}
	return b.Append(&InstStore{
		V:      v,
		Equals: equals,
	})
}

func (b *Block) Call(name string, params []Ref) Ref {
	return b.Append(&InstCall{
		Name:   name,
		Params: params,
	})
}

type Inst interface {
	Print(level int)
}

// Represents a block inside another block
type InstBlock struct {
	Block *Block
}

func (i *InstBlock) Print(level int) {
	fmt.Printf("Block:\n")
	i.Block.Print(level + 1)
}

type InstAdd struct {
	Lhs Ref
	Rhs Ref
}

func (i *InstAdd) Print(level int) {
	fmt.Printf("Add %d %d\n", i.Lhs, i.Rhs)
}

type InstEq struct {
	Lhs Ref
	Rhs Ref
}

func (i *InstEq) Print(level int) {
	fmt.Printf("Eq %d %d\n", i.Lhs, i.Rhs)
}

type InstNotEq struct {
	Lhs Ref
	Rhs Ref
}

func (i *InstNotEq) Print(level int) {
	fmt.Printf("NotEq %d %d\n", i.Lhs, i.Rhs)
}

type InstRet struct {
	HasValue bool
	Value    Ref
}

func (i *InstRet) Print(level int) {
	if i.HasValue {
		fmt.Printf("Ret %d\n", i.Value)
	} else {
		fmt.Printf("Ret\n")
	}
}

type InstInt struct {
	Value int
}

func (i *InstInt) Print(level int) {
	fmt.Printf("Int %d\n", i.Value)
}

type InstIf struct {
	Condition Ref
	Then      *Block
	Else      *Block
}

func (i *InstIf) Print(level int) {
	fmt.Printf("If %d\n", i.Condition)

	i.Then.Print(level + 1)
	if i.Else != nil {
		i.Else.Print(level + 1)
	}
}

type InstVar struct {
	V      *Variable
	Equals Ref
}

func (i *InstVar) Print(level int) {
	fmt.Printf("Var (%s: %s) %d\n", i.V.Name, i.V.Type, i.Equals)
}

type InstLoad struct {
	V *Variable
}

func (i *InstLoad) Print(level int) {
	fmt.Printf("Load (%s: %s)\n", i.V.Name, i.V.Type)
}

type InstStore struct {
	V      *Variable
	Equals Ref
}

func (i *InstStore) Print(level int) {
	fmt.Printf("Store (%s: %s) %d\n", i.V.Name, i.V.Type, i.Equals)
}

type InstCall struct {
	Name   string
	Params []Ref
}

func (i *InstCall) Print(level int) {
	fmt.Printf("Call %s(n%d)\n", i.Name, len(i.Params))
}

func printf(level int, format string, a ...any) {
	for i := 0; i < level; i++ {
		fmt.Print("\t")
	}
	fmt.Printf(format, a...)
}
