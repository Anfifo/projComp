// $Id: memory_management.h,v 1.1 2018/05/21 01:16:14 ist426036 Exp $ -*- c++ -*-
#ifndef __GR8_LOCAL_MEMORY_MANAGER_H__
#define __GR8_LOCAL_MEMORY_MANAGER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/ast/basic_node.h>
#include "targets/symbol.h"
#include "targets/basic_ast_visitor.h"

namespace gr8 {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class memory_management: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;

    basic_ast_visitor *_parent;

  public:
    memory_management(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab, basic_ast_visitor *parent) :
          basic_ast_visitor(compiler), _symtab(symtab), _parent(parent) {
    }

  public:
    ~memory_management() {
      os().flush();
    }

  protected:


  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

//---------------------------------------------------------------------------
//     HELPER MACRO FOR LOCAL MEMORY MANAGEMENT
//---------------------------------------------------------------------------

#define MANAGE_MEMORY(compiler, symtab, node) { \
  try { \
    gr8::memory_management manager(compiler, symtab, this); \
    (node)->accept(&manager, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_LOCAL_MEMORY_SPACE MANAGE_MEMORY(_compiler, _symtab, node)

#endif
