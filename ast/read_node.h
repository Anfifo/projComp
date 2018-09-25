// $Id: read_node.h,v 1.3 2018/04/18 22:57:19 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_READNODE_H__
#define __GR8_READNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace gr8 {

  /**
   * Class for describing read nodes.
   */
  class read_node: public cdk::expression_node {

  public:
    inline read_node(int lineno) :
        cdk::expression_node(lineno){
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_read_node(this, level);
    }

  };

} // gr8

#endif
