// $Id: invocation_node.h,v 1.2 2018/04/17 22:33:06 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_INVOCATIONNODE_H__
#define __GR8_INVOCATIONNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing function invocation nodes.
   */
  class invocation_node: public cdk::expression_node {
    std::string *_name;
    cdk::sequence_node *_arguments;


    public:
      inline invocation_node(int lineno, std::string *name,
         cdk::sequence_node *arguments) : cdk::expression_node(lineno),
         _name(name), _arguments(arguments) {
      }

  public:
    inline std::string *name() {
      return _name;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_invocation_node(this, level);
    }

  };

} // gr8

#endif
