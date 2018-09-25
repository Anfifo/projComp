// $Id: return_node.h,v 1.3 2018/05/20 01:09:16 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_RETURNNODE_H__
#define __GR8_RETURNNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>


namespace gr8 {

  /**
   * Class for describing return nodes.
   */
  class return_node: public cdk::basic_node {
    cdk::expression_node *_argument;
    basic_type *_type;

  public:
    inline return_node(int lineno, cdk::expression_node *argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline cdk::expression_node *argument() {
      return _argument;
    }


    inline basic_type *type() {
      return _type;
    }

    inline void type(basic_type * arg) {
      basic_type *type = new basic_type(arg->size(), arg->name());
      for(basic_type *it = arg->subtype(), *subtype = type->subtype(); it != nullptr; it = it->subtype(), subtype = subtype->subtype()){
        subtype = new basic_type(it->size(), it->name());
      }
      _type = type;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // gr8

#endif
