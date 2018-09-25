// $Id: sweeping_node.h,v 1.3 2018/03/23 02:09:49 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_SWEEPINGNODE_H__
#define __GR8_SWEEPINGNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing sweeping nodes (for-like loop).
   */
  class sweeping_node: public cdk::basic_node {
    cdk::lvalue_node *_lvalue;
    cdk::expression_node *_expression1, *_expression2, *_expression3;
    gr8::block_node *_block;

  public:
    inline sweeping_node(int lineno, cdk::lvalue_node *lvalue, cdk::expression_node *expression1, cdk::expression_node *expression2, cdk::expression_node *expression3, gr8::block_node *block) :
        cdk::basic_node(lineno), _lvalue(lvalue), _expression1(expression1), _expression2(expression2), _expression3(expression3), _block(block) {
    }

  public:
    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }
    inline cdk::expression_node *expression1() {
      return _expression1;
    }
    inline cdk::expression_node *expression2() {
      return _expression2;
    }
    inline cdk::expression_node *expression3() {
      return _expression3;
    }
    inline gr8::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweeping_node(this, level);
    }

  };

} // gr8

#endif
