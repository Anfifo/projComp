// $Id: mem_alloc_node.h,v 1.3 2018/03/23 13:47:26 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_MEMALLOCNODE_H__
#define __GR8_MEMALLOCNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing memory allocation nodes.
   */
  class mem_alloc_node: public cdk::unary_expression_node {

  public:
    inline mem_alloc_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_expression_node(lineno, argument) {
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_mem_alloc_node(this, level);
    }

  };

} // gr8

#endif
