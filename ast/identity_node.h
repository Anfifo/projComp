// $Id: identity_node.h,v 1.2 2018/03/23 02:09:48 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_IDENTITYNODE_H__
#define __GR8_IDENTITYNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing identity nodes.
   */
  class identity_node: public cdk::unary_expression_node {

  public:
    inline identity_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_expression_node(lineno, argument) {
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_identity_node(this, level);
    }

  };

} // gr8

#endif
