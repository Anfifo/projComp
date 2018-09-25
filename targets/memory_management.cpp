#include <string>
#include "targets/memory_management.h"
#include "ast/all.h"  // automatically generated


bool insideFunc = false;
int space = 0;


void gr8::memory_management::do_sequence_node(cdk::sequence_node * const node, int lvl) {
    for (size_t i = 0; i < node->size(); i++){
      node->node(i)->accept(this, lvl);
    }
}



void gr8::memory_management::do_nil_node(cdk::nil_node * const node, int lvl) {
}

void gr8::memory_management::do_data_node(cdk::data_node * const node, int lvl) {
}




void gr8::memory_management::do_not_node(cdk::not_node * const node, int lvl) {
}

void gr8::memory_management::do_and_node(cdk::and_node * const node, int lvl) {
}

void gr8::memory_management::do_or_node(cdk::or_node * const node, int lvl) {
}

void gr8::memory_management::do_mem_alloc_node(gr8::mem_alloc_node *const node, int lvl) {
}

void gr8::memory_management::do_block_node(gr8::block_node *const node, int lvl) {
  if(node->variables() != NULL)
    node->variables()->accept(this, lvl);

  if(node->instructions() != NULL)
    node->instructions()->accept(this,lvl);
}

void gr8::memory_management::do_null_node(gr8::null_node *const node, int lvl) {
}



void gr8::memory_management::do_return_node(gr8::return_node *const node, int lvl) {
}

void gr8::memory_management::do_invocation_node(gr8::invocation_node *const node, int lvl) {
}

void gr8::memory_management::do_ind_pos_node(gr8::ind_pos_node *const node, int lvl) {
}

void gr8::memory_management::do_indexation_node(gr8::indexation_node *const node, int lvl) {
}

void gr8::memory_management::do_identity_node(gr8::identity_node *const node, int lvl) {
}



void gr8::memory_management::do_again_node(gr8::again_node *const node, int lvl) {
}

void gr8::memory_management::do_stop_node(gr8::stop_node *const node, int lvl) {
}

void gr8::memory_management::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
}

void gr8::memory_management::do_function_node(gr8::function_node *const node, int lvl) {
  std::shared_ptr<gr8::symbol> symbol =_symtab.find(*node->name());
  insideFunc = true;
  space = 0;
  if(node->variables() != nullptr)
    node->variables()->accept(this, lvl);
  node->block()->accept(this, lvl);
  symbol->offset(space);
  insideFunc = false;
}

void gr8::memory_management::do_function_prototype_node(gr8::function_prototype_node *const node, int lvl) {
}


void gr8::memory_management::do_variable_node(gr8::variable_node *const node, int lvl) {
  if(!(node->qualifier() != nullptr && *node->qualifier() == "public") && insideFunc){
    if(node->type()->name() == basic_type::TYPE_DOUBLE){
      space += 8;
    }else{
      space += 4;
    }
  }
}




void gr8::memory_management::do_integer_node(cdk::integer_node * const node, int lvl) {
}

void gr8::memory_management::do_double_node(cdk::double_node * const node, int lvl) {
}

void gr8::memory_management::do_string_node(cdk::string_node * const node, int lvl) {
}



void gr8::memory_management::do_neg_node(cdk::neg_node * const node, int lvl) {
}


void gr8::memory_management::do_add_node(cdk::add_node * const node, int lvl) {
}

void gr8::memory_management::do_sub_node(cdk::sub_node * const node, int lvl) {
}
void gr8::memory_management::do_mul_node(cdk::mul_node * const node, int lvl) {
}
void gr8::memory_management::do_div_node(cdk::div_node * const node, int lvl) {
}
void gr8::memory_management::do_mod_node(cdk::mod_node * const node, int lvl) {
}
void gr8::memory_management::do_lt_node(cdk::lt_node * const node, int lvl) {
}
void gr8::memory_management::do_le_node(cdk::le_node * const node, int lvl) {
}

void gr8::memory_management::do_ge_node(cdk::ge_node * const node, int lvl) {
}

void gr8::memory_management::do_gt_node(cdk::gt_node * const node, int lvl) {
}
void gr8::memory_management::do_ne_node(cdk::ne_node * const node, int lvl) {
}
void gr8::memory_management::do_eq_node(cdk::eq_node * const node, int lvl) {
}



void gr8::memory_management::do_identifier_node(cdk::identifier_node * const node, int lvl) {
}

void gr8::memory_management::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
}

void gr8::memory_management::do_assignment_node(cdk::assignment_node * const node, int lvl) {
}





void gr8::memory_management::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
}


void gr8::memory_management::do_print_node(gr8::print_node * const node, int lvl) {
}



void gr8::memory_management::do_read_node(gr8::read_node * const node, int lvl) {
}


void gr8::memory_management::do_if_node(gr8::if_node * const node, int lvl) {
}

void gr8::memory_management::do_if_else_node(gr8::if_else_node * const node, int lvl) {
}
