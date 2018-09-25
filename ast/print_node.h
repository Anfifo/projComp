// $Id: print_node.h,v 1.2 2018/03/21 23:11:02 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_PRINTNODE_H__
#define __GR8_PRINTNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing print nodes.
   */
  class print_node: public cdk::basic_node {
    cdk::expression_node *_argument;
    bool _newline;

  public:
    inline print_node(int lineno, cdk::expression_node *argument, bool newline = true) :
        cdk::basic_node(lineno), _argument(argument), _newline(newline) {
    }

  public:
    inline cdk::expression_node *argument() {
      return _argument;
    }

  public:
    inline bool newline() {
      return _newline;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_print_node(this, level);
    }

  };

} // gr8

#endif
