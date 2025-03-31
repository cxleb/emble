import XCTest
import SwiftTreeSitter
import TreeSitterEmble

final class TreeSitterEmbleTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_emble())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Emble grammar")
    }
}
