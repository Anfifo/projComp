// $Id: function_node.h,v 1.6 2018/04/19 00:52:12 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_FUNCTIONODE_H__
#define __GR8_FUNCTIONODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/basic_type.h>

namespace gr8 {

  /**
   * Class for describing function declaration with body nodes.
   */
  class function_node: public cdk::basic_node {
    std::string *_qualifier;
    basic_type *_type;
    std::string *_name;
    cdk::sequence_node *_variables;
    gr8::block_node *_block;

  public:
    inline function_node(int lineno, std::string *qualifier, basic_type *type,
       std::string *name, cdk::sequence_node *variables, gr8::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type),
         _name(name), _variables(variables), _block(block) {
    }

  public:
    inline std::string *qualifier() {
      return _qualifier;
    }
    inline basic_type *type() {
      return _type;
    }
    inline std::string *name() {
      return _name;
    }
    inline cdk::sequence_node *variables() {
      return _variables;
    }
    inline gr8::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_node(this, level);
    }

  };

} // gr8

#endif
