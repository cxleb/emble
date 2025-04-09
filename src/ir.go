package main

type Type interface{}
type TypeName string
type TypePointer struct {
	innerType Type
}
type TypeArray struct {
	innerType Type
}

type Package struct {
	modules []Module
}

type Module struct {
	funcs []*Func
}

func (m *Module) addFunc(f *Func) {
	m.funcs = append(m.funcs, f)
}

type Struct struct {
}

type Impl struct {
}

type Func struct {
	name       string
	exported   bool
	returnType Type
	blocks     []Block
}

func (f *Func) lastBlock() *Block {
	if f.blocks == nil {
		f.blocks = make([]Block, 1)
		f.blocks[0].number = 0
		f.blocks[0].insts = make([]Inst, 0)
	}
	return &f.blocks[len(f.blocks)-1]
}

func (f *Func) newBlock() *Block {
	if f.blocks == nil {
		f.blocks = make([]Block, 1)
		f.blocks[0].number = 0
		f.blocks[0].insts = make([]Inst, 0)
	}
	return &f.blocks[len(f.blocks)-1]
}

type Block struct {
	number int
	insts  []Inst
}

func (b *Block) addInst(i Inst) int {
	loc := len(b.insts)
	b.insts = append(b.insts, i)
	return loc
}

func (b *Block) ret(result int) int {
	return b.addInst(Inst{
		inst: InstRet,
		ops:  []int{result},
	})
}

func (b *Block) int(val int) int {
	return b.addInst(Inst{
		inst: InstInt,
		ops:  []int{val},
	})
}

const (
	InstAdd int = iota
	InstRet
	InstInt
)

type Inst struct {
	inst int
	ops  []int
}
