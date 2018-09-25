// $Id: variable_node.h,v 1.6 2018/04/18 21:58:26 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_VARIABLENODE_H__
#define __GR8_VARIABLENODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/basic_type.h>


namespace gr8 {

  /**
   * Class for describing variable nodes.
   */
  class variable_node: public cdk::basic_node {
    std::string *_qualifier;
    basic_type *_type;
    std::string *_name;
    cdk::expression_node *_expression;

  public:
    inline variable_node(int lineno, std::string *qualifier, basic_type *type,
       std::string *name, cdk::expression_node *expression) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type),
         _name(name), _expression(expression) {
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
    inline cdk::expression_node *expression() {
      return _expression;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_node(this, level);
    }

  };

} // gr8

#endif
