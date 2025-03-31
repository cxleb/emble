from unittest import TestCase

import tree_sitter
import tree_sitter_emble


class TestLanguage(TestCase):
    def test_can_load_grammar(self):
        try:
            tree_sitter.Language(tree_sitter_emble.language())
        except Exception:
            self.fail("Error loading Emble grammar")
