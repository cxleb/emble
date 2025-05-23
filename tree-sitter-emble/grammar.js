/**
 * @file Emble grammar for tree-sitter
 * @author Caleb Wait <waitcaleb@gmail.com>
 * @license MIT
 */

module.exports = grammar({
  name: "emble",

  extras: $ => [
    $.comment,
    /\s/,
  ],

  rules: {
    source_file: $ => repeat(
      choice(
        $.import_definition,
        $.definition,
      )
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Top level definitions

    import_definition: $ => choice(
      $.import_all,
      $.import_from,
      $.export_all,
      $.export_from,
    ),

    import_all: $ => seq(
      'import',
      $.identifier
    ),

    import_from: $ => seq(
      'import',
      commaSep1($.identifier),
      'from',
      $.identifier
    ),

    export_all: $ => seq(
      'export',
      $.identifier
    ),

    export_from: $ => seq(
      'expoty',
      commaSep1($.identifier),
      'from',
      $.identifier
    ),

    definition: $ => choice(
      $.struct,
      $.interface,
      $.impl,
      $.extern_func,
      $.func
    ),

    export: $ => 'export',

    struct: $ => seq(
      field('exported', optional('export')),
      'struct',
      $.identifier,
      '{',
        repeat(
          choice(
            $.struct_field,
            $.self_func,
          )
        ),
      '}',
    ),

    struct_field: $ => seq(
      optional(field('exported', $.export)),
      ':',
      field('type', $.type),
      ','
    ),

    interface: $ => seq (
      optional(field('exported', $.export)),
      'interface',
      field('name', $.identifier),
      '{',
      repeat(
        $.self_func_decl,
      ),
      '}'
    ),

    impl: $ => seq(
      'impl',
      field('interface', $.identifier),
      'on',
      field('struct', $.identifier),
      '{',
        repeat(
          $.self_func,
        ),
      '}',
    ),

    extern_func: $ => seq(
      optional(field('exported', $.export)),
      'extern(c)',
      'func',
      field('name', $.identifier),
      field('parameters', $.parameter_list),
      field('return', $.type),
    ),

    func: $ => seq(
      optional(field('exported', $.export)),
      'func',
      field('name', $.identifier),
      field('parameters', $.parameter_list),
      field('return', $.type),
      field('block', $.block)
    ),

    parameter_list: $ => seq(
      '(',
      commaSep(
        seq(
          field('name', $.identifier),
          ':',
          field('type', $.type),
        )
      ),
      ')'
    ),

    self_func: $ => seq(
      $.self_func_decl,
      $.block
    ),

    self_func_decl: $ => seq(
      'func',
      field('name', $.identifier),
      field('parameters', $.parameter_list),
      field('return', $.type),
    ),

    self_parameter_list: $ => seq(
      '(',
      optional(
        seq(
          'self',
          ',',
        )
      ),
      commaSep(
        seq(
          field('name', $.identifier),
          ':',
          field('type', $.type),
        )
      ),
      ')'
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Statements

    statement: $ => choice(
      $.block,
      $.variable,
      $.if,
      $.while,
      $.for,
      $.return,
      $.assignment,
      $.function_call
    ),

    block: $ => seq(
      '{',
      repeat($.statement),
      '}'
    ),

    variable: $ => seq(
      field("specifier", $.variable_specifier),
      field('name', $.identifier),
      optional(
        seq(
          ':',
          field('type', $.identifier),
        )
      ),
      '=',
      field("equals", $.expression),
      $.terminator,
    ),

    variable_specifier: $ => choice(
      'let',
      'const'
    ),

    if: $ => seq(
      'if',
      field('condition', $.expression),
      field('then', $.block),
      optional(seq(
        'else',
        field('else', choice($.block, $.if)),
      )),
    ),

    while: $ => seq(
      'while',
      $.expression,
      $.block,
    ),

    for: $ => seq(
      'for',
      $.identifier,
      'in',
      $.identifier,
      $.block,
    ),

    return: $ => seq(
      'return',
      optional(field("value", $.expression)),
      $.terminator,
    ),

    assignment: $ => seq(
      $.identifier,
      $.assignment_op,
      $.expression,
      $.terminator,
    ),

    assignment_op: $ => choice(
      '=',
      '+=',
      '-=',
      '/=',
      '*=',
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Expression

    expression: $ => choice(
      $.binary_expression,
      $.boolean_constant,
      $.integer,
      $.float,
      $.function_call,
      $.identifier,
    ),

    boolean_constant: $ => choice(
      'true',
      'false'
    ),

    binary_expression: $ => {
      const table = [
        [1, choice('*', '/', '%', '<<', '>>', '&', '&^')],
        [2, choice('+', '-', '|', '^')],
        [3, choice('==', '!=', '>', '>=', '<', '<=')],
        [4, 'and'],
        [5, 'or'],
      ];

      return choice(...table.map(([precedence, operator]) =>
        prec.left(precedence, seq(
          field('left', $.expression),
          field('operator', operator),
          field('right', $.expression),
        ))
      ));
    },

    function_call: $ => seq(
      field("name", $.identifier),
      '(',
      field("params", commaSep($.expression)),
      ')'
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Miscellaneous

    type: $ => choice(
      $.array_type,
      $.pointer_type,
      $.identifier,
    ),

    pointer_type: $ => seq(
      '*',
      $.type
    ),

    //reference_type: $ => seq(
    //  '&',
    //  $.type
    //),

    array_type: $ => seq(
      '[',
      optional($.identifier),
      ']',
      $.type
    ),

    identifier: $ => /[A-Za-z][A-Za-z0-9_\-]*/,
    integer: $ => /[0-9]+/,
    float: $ => /[0-9]*\.[0-9]+/,
    comment: _ => token(choice(
      seq('//', /.*/),
      seq(
        '/*',
        /[^*]*\*+([^/*][^*]*\*+)*/,
        '/',
      ),
    )),

    terminator: $ => choice(';', '\n'), 
  }
});

/**
 * Creates a rule to optionally match one or more of the rules separated by a comma
 *
 * @param {Rule} rule
 *
 * @returns {ChoiceRule}
 */
function commaSep(rule) {
  return optional(commaSep1(rule));
}

/**
 * Creates a rule to match one or more of the rules separated by a comma
 *
 * @param {Rule} rule
 *
 * @returns {SeqRule}
 */
function commaSep1(rule) {
  return seq(rule, repeat(seq(',', rule)));
} 

module.exports.commaSep = commaSep;
module.exports.commaSep1 = commaSep1;