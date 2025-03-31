/**
 * @file Emble grammar for tree-sitter
 * @author Caleb Wait <waitcaleb@gmail.com>
 * @license MIT
 */

module.exports = grammar({
  name: "emble",

  rules: {
    // TODO: add the actual grammar rules
    source_file: $ => repeat(
      choice(
        $.definition,
        $.export_definition,
      ) 
    ),

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
            $.function,
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
        choice(
          $.self_function_decl,
          $.function_decl,
        )
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
          choice(
            $.self_function,
            $.function,
          )
        ),
      '}',
    ),

    function: $ => seq(
      $.function_decl,
      $.statement
    ),

    function_decl: $ => seq(
      'func',
      field('name', $.identifier),
      $.parameter_list,
      field('return_type', optional($.identifier)),
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
      $.statement
    ),

    self_function_decl: $ => seq(
      'func',
      field('name', $.identifier),
      $.self_parameter_list,
      field('return_type', optional($.identifier)),
    ),

    self_parameter_list: $ => seq(
      '(',
      'self',
      ',',
      commaSep(
        seq(
          field('name', $.identifier),
          ':',
          field('type', $.identifier),
        )
      ),
      ')'
    ),

    statement: $ => choice(
      $.block,
      // let
      // const
      // if
      // while
      // for
      // return
    ),

    block: $ => seq(
      '{',
      repeat($.statement),
      '}'
    ),

    identifier: $ => /[A-Za-z][A-Za-z0-9]+/
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