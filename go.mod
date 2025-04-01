module github.com/cxleb/emble

go 1.23

toolchain go1.23.7

require (
	github.com/cxleb/emble/tree-sitter-emble v0.0.0-00010101000000-000000000000
	github.com/tree-sitter/go-tree-sitter v0.25.0
)

require (
	github.com/mattn/go-pointer v0.0.1 // indirect
	tinygo.org/x/go-llvm v0.0.0-20250119132755-9dca92dfb4f9 // indirect
)

replace github.com/cxleb/emble/tree-sitter-emble => ./tree-sitter-emble
