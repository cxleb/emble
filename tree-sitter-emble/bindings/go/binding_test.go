package tree_sitter_emble_test

import (
	"testing"

	tree_sitter_emble "github.com/cxleb/emble/tree-sitter-emble/bindings/go"
	tree_sitter "github.com/tree-sitter/go-tree-sitter"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_emble.Language())
	if language == nil {
		t.Errorf("Error loading Emble grammar")
	}
}
