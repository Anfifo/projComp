// $Id: if_else_node.h,v 1.2 2018/03/23 13:47:26 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_IFELSENODE_H__
#define __GR8_IFELSENODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing if-then-else nodes.
   */
  class if_else_node: public cdk::basic_node {
    cdk::expression_node *_condition;
    gr8::block_node *_thenblock, *_elseblock;

  public:
    inline if_else_node(int lineno, cdk::expression_node *condition, gr8::block_node *thenblock, gr8::block_node *elseblock) :
        cdk::basic_node(lineno), _condition(condition), _thenblock(thenblock), _elseblock(elseblock) {
    }

  public:
    inline cdk::expression_node *condition() {
      return _condition;
    }
    inline gr8::block_node *thenblock() {
      return _thenblock;
    }
    inline gr8::block_node *elseblock() {
      return _elseblock;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_if_else_node(this, level);
    }

  };

} // gr8

#endif
