// $Id: null_node.h,v 1.2 2018/03/23 14:57:04 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_NULLNODE_H__
#define __GR8_NULLNODE_H__

namespace gr8 {

  /**
   * Class for describing read nodes.
   */
  class null_node: public cdk::literal_node<std::nullptr_t>{

  public:
    inline null_node(int lineno) :
        cdk::literal_node<std::nullptr_t>(lineno,NULL) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_null_node(this, level);
    }

  };

} // gr8

#endif
