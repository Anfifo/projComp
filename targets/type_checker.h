// $Id: type_checker.h,v 1.6 2018/05/20 01:09:16 ist425998 Exp $ -*- c++ -*-
#ifndef __GR8_SEMANTICS_TYPE_CHECKER_H__
#define __GR8_SEMANTICS_TYPE_CHECKER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/ast/basic_node.h>
#include "targets/symbol.h"
#include "targets/basic_ast_visitor.h"


extern std::string *_curr_function;


namespace gr8 {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;

    basic_ast_visitor *_parent;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    void processUnaryExpression(cdk::unary_expression_node * const node, int lvl);
    void processIntDoubleUnaryExpression(cdk::unary_expression_node * const node, int lvl);
    void processBinaryExpression(cdk::binary_expression_node * const node, int lvl);
    void processIntBinaryExpression(cdk::binary_expression_node * const node, int lvl);
    void processIntDoubleBinaryExpression(cdk::binary_expression_node * const node, int lvl);
    void processIntDoubleLogicBinaryExpression(cdk::binary_expression_node * const node, int lvl);
    bool matchable_pointer_types(basic_type *type1, basic_type *type2 );
    template<typename T>
    void process_literal(cdk::literal_node<T> * const node, int lvl) {

    }
    bool matchable_types(unsigned long int type1, unsigned long int type2 );

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, node) { \
  try { \
    gr8::type_checker checker(compiler, symtab, this); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, node)

#define FUNCTION_BEGIN {\
  if (_curr_function != nullptr)\
    throw new std::string("Can't have nested function definitions");\
  _curr_function = node->name();\
}
#define FUNCTION_END {\
  if (_curr_function == nullptr)\
    throw new std::string("function already undefined");\
  _curr_function = nullptr;\
}

#endif
