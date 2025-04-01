/**
 * @file Emble grammar for tree-sitter
 * @author Caleb Wait <waitcaleb@gmail.com>
 * @license MIT
 */

module.exports = grammar({
  name: "emble",

  extras: $ => $ => [
    $.comment,
    /\s/,
  ],

  rules: {
    source_file: $ => repeat(
      choice(
        $.definition,
        $.export_definition,
      ) 
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Top level definitions

    export_definition: $ => seq(
      'export',
      $.definition,
    ),

    definition: $ => choice(
      $.import_definition,
      $.struct,
      $.interface,
      $.impl,
      $.function
    ),

    import_definition: $ => choice(
      $.import_all,
      $.import_from
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

    struct: $ => seq(
      'struct',
      $.identifier,
      '{',
        repeat(
          choice(
            $.struct_field,
            $.self_function,
          )
        ),
      '}',
    ),

    struct_field: $ => seq(
      field('name', $.identifier),
      ':',
      field('type', $.identifier),
      ','
    ),

    interface: $ => seq (
      'interface',
      field('name', $.identifier),
      '{',
      repeat(
        $.self_function_decl,
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
          $.self_function,
        ),
      '}',
    ),

    function: $ => seq(
      $.function_decl,
      $.block
    ),

    function_decl: $ => seq(
      'func',
      field('name', $.identifier),
      $.parameter_list,
      optional(
        field('return_type', $.identifier),
      )
    ),

    parameter_list: $ => seq(
      '(',
      commaSep(
        seq(
          field('name', $.identifier),
          ':',
          field('type', $.identifier),
        )
      ),
      ')'
    ),

    self_function: $ => seq(
      $.self_function_decl,
      $.block
    ),

    self_function_decl: $ => seq(
      'func',
      field('name', $.identifier),
      $.self_parameter_list,
      optional(
        field('return_type', $.identifier),
      )
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
          field('type', $.identifier),
        )
      ),
      ')'
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Statements

    statement: $ => choice(
      $.block,
      $.variable,
      $.if_stmt,
      $.while_stmt,
      $.for_stmt,
      $.return_stmt,
      $.assignment_stmt,
    ),

    block: $ => seq(
      '{',
      repeat($.statement),
      '}'
    ),

    variable: $ => seq(
      $.variable_specifier,
      field('name', $.identifier),
      optional(
        seq(
          ':',
          field('type', $.identifier),
        )
      ),
      '=',
      $.expression
    ),

    variable_specifier: $ => choice(
      'let',
      'const'
    ),

    if_stmt: $ => seq(
      'if',
      $.expression,
      $.statement,
    ),

    while_stmt: $ => seq(
      'while',
      $.expression,
      $.statement,
    ),

    for_stmt: $ => seq(
      'for',
      $.identifier,
      'in',
      $.identifier,
      $.statement,
    ),

    return_stmt: $ => seq(
      'return',
      $.expression,
    ),

    assignment_stmt: $ => seq(
      $.identifier,
      $.assignment_op,
      $.expression,
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
      $.binary_op,
      $.integer,
      $.float,
      $.identifier,
      $.boolean_constant,
    ),

    boolean_constant: $ => choice(
      'true',
      'false'
    ),

    binary_op: $ => prec.left(1, seq(
      $.expression,
      $.op,
      $.expression
    )),

    op: $ => choice(
      '+',
      '-',
      '*',
      '/',
    ),

    ///////////////////////////////////////////////////////////////////////////
    //// Miscellaneous

    identifier: $ => /[A-Za-z][A-Za-z0-9]+/,
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