// $Id: indexation_node.h,v 1.5 2018/04/20 08:49:20 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_INDEXATIONNODE_H__
#define __GR8_INDEXATIONNODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing indexation nodes.
   */
  class indexation_node: public cdk::lvalue_node {
    cdk::expression_node *_base;
    cdk::expression_node *_step;

  public:
    inline indexation_node(int lineno, cdk::expression_node *base, cdk::expression_node *step) :
        cdk::lvalue_node(lineno), _base(base), _step(step) {
    }

  public:
    inline cdk::expression_node *base() {
      return _base;
    }
    inline cdk::expression_node *step() {
      return _step;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_indexation_node(this, level);
    }

  };

} // gr8

#endif
