// $Id: ind_pos_node.h,v 1.4 2018/03/23 13:47:26 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_INDPOSNODE_H__
#define __GR8_INDPOSNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace gr8 {

  /**
   * Class for describing position indication nodes.
   */
  class ind_pos_node: public cdk::expression_node {

  cdk::lvalue_node *_lvalue;

  public:
    inline ind_pos_node(int lineno, cdk::lvalue_node *lvalue) :
        cdk::expression_node(lineno), _lvalue(lvalue) {
    }

  public:
    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_ind_pos_node(this, level);
    }

  };

} // gr8

#endif
