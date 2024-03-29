// $Id: again_node.h,v 1.5 2018/04/17 22:33:06 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_AGAIN_H__
#define __GR8_AGAIN_H__

#include <cdk/ast/integer_node.h>

namespace gr8 {

  /**
   * Class for describing again nodes.
   */
  class again_node: public cdk::basic_node {
    int _argument;

  public:
    inline again_node(int lineno, int argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline int argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_again_node(this, level);
    }

  };

} // gr8

#endif
