// $Id: block_node.h,v 1.2 2018/03/23 02:09:48 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_BLOCKNODE_H__
#define __GR8_BLOCKNODE_H__

#include <cdk/ast/sequence_node.h>

namespace gr8 {

  /**
   * Class for describing block nodes.
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_variables;
    cdk::sequence_node *_instructions;

  public:
    inline block_node(int lineno, cdk::sequence_node *variables, cdk::sequence_node *instructions) :
        cdk::basic_node(lineno), _variables(variables), _instructions(instructions) {
    }

  public:
    inline cdk::sequence_node *variables() {
      return _variables;
    }
    inline cdk::sequence_node *instructions() {
      return _instructions;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // gr8

#endif
