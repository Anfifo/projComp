#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

bool printOP = false;
std::string *_curr_function = nullptr;


/* ============================ LITERALS =============================== */


void gr8::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

void gr8::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}


//---------------------------------------------------------------------------






/* ===================== LOGIC OPERATIONS ================================ */
void gr8::type_checker::processIntBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in right argument of binary expression");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in argument of unary expression");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  processIntBinaryExpression(node, lvl);
}
void gr8::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  processIntBinaryExpression(node, lvl);
}
void gr8::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  processIntDoubleLogicBinaryExpression(node, lvl);
}
void gr8::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
}

void gr8::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
}

void gr8::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  processIntDoubleLogicBinaryExpression(node, lvl);
}
void gr8::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
}
void gr8::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't compare strings - left");
  if (node->right()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't compare strings - right");

  if( !gr8::type_checker::matchable_types(node->left()->type()->name(), node->right()->type()->name()))
    throw std::string("right argument does not match or convert to left's argument type.");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

//---------------------------------------------------------------------------






/* ========================== INSTRUCTIONS ================================== */


void gr8::type_checker::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void gr8::type_checker::do_print_node(gr8::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if(node->argument()->type()->name() == basic_type::TYPE_UNSPEC)
    node->argument()->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_if_node(gr8::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  node->block()->accept(this, lvl + 4);
}

void gr8::type_checker::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  node->thenblock()->accept(this, lvl + 4);
  if(node->elseblock() != nullptr){
    node->elseblock()->accept(this, lvl + 4);
  }
}

void gr8::type_checker::do_return_node(gr8::return_node *const node, int lvl) {

  if (_curr_function == nullptr)
    throw std::string("return node found outside function");

  node->argument()->accept(this, lvl + 2);

  // get current function return value
  std::shared_ptr<gr8::symbol> symbol =_symtab.find(*_curr_function);
  if (symbol == nullptr)
    throw std::string("Return found outside function!");

  if(! gr8::type_checker::matchable_types(symbol->type()->name(), node->argument()->type()->name()))
    throw std::string("Return type does not match function type");

  if(symbol->type()->name() == basic_type::TYPE_INT && node->argument()->type()->name() != basic_type::TYPE_INT )
    throw std::string("Return type cannot be converted to function type int");

  if(symbol->type()->name() == basic_type::TYPE_POINTER)
    if(! gr8::type_checker::matchable_pointer_types(symbol->type(), node->argument()->type()))
      throw std::string("Return subtype does not match function subtype");

  node->type(symbol->type());
}

void gr8::type_checker::do_again_node(gr8::again_node *const node, int lvl) {
}

void gr8::type_checker::do_stop_node(gr8::stop_node *const node, int lvl) {
}

void gr8::type_checker::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  node->lvalue()->accept(this, lvl+4);

  if(node->lvalue()->type()->name() != basic_type::TYPE_INT)
    throw std::string("Iterator type must be small");

  node->expression1()->accept(this, lvl+4);
  if(node->expression1()->type()->name() != basic_type::TYPE_INT)
    throw std::string("To has to be of type small.");

  node->expression2()->accept(this, lvl+4);
  if(node->expression2()->type()->name() != basic_type::TYPE_INT)
    throw std::string("From has to be of type small.");

  if(node->expression3() != NULL){
    node->expression3()->accept(this, lvl+4);
    if(node->expression3()->type()->name() != basic_type::TYPE_INT)
      throw std::string("By has to be of type small.");
  }
}

//---------------------------------------------------------------------------








/* =========================== FUNCTIONS ======================================= */

void gr8::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
}

void gr8::type_checker::do_invocation_node(gr8::invocation_node *const node, int lvl) {
  const std::string &id = *node->name();
  std::shared_ptr<gr8::symbol> symbol =_symtab.find(id);
  if (symbol == nullptr)
    throw std::string("Function not undeclared.");

  if(node->arguments()!= nullptr)
  // compare invocation args types to defined function arg types
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    ((cdk::typed_node*)node->arguments()->node(i))->accept(this, lvl);
    if(symbol != nullptr && !symbol->args().empty()){
      basic_type* invoc_arg = ((cdk::typed_node*)node->arguments()->node(i))->type();

      // handle cast of ReadDouble or ReadInt
      if(invoc_arg->name() == basic_type::TYPE_UNSPEC && (symbol->getArg(i)->name() == basic_type::TYPE_INT ||
                                                          symbol->getArg(i)->name() == basic_type::TYPE_DOUBLE )){
        ((cdk::typed_node*)node->arguments()->node(i))->type(symbol->getArg(i));
      }

      invoc_arg = ((cdk::typed_node*)node->arguments()->node(i))->type();
      if(! (matchable_types( symbol->getArg(i)->name(),invoc_arg->name() ))){
        throw std::string("Invocation args' types dont match reference function types");
      }
    }
  }
  node->type(symbol->type());
}

void gr8::type_checker::do_block_node(gr8::block_node *const node, int lvl) {

}

void gr8::type_checker::do_function_node(gr8::function_node *const node, int lvl) {
  const std::string &id = *node->name();
  std::shared_ptr<gr8::symbol> symbol =_symtab.find(id);

  if (symbol == nullptr){
    symbol = std::make_shared<gr8::symbol>(node->type(), id, 0);
    if(node->variables() != nullptr){
      for(size_t i = 0; i < node->variables()->size(); i++){
        symbol->addArg(((gr8::variable_node*)node->variables()->node(i))->type());
      }
    }
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);

  }else if(symbol->type()->name() != node->type()->name()){
    throw std::string("Definition type does not match declaration type.");
  }

  if(node->variables() != nullptr)
    node->variables()->accept(this,lvl+2);
  symbol->defined(true);
}

void gr8::type_checker::do_function_prototype_node(gr8::function_prototype_node *const node, int lvl) {
  const std::string &id = *node->name();

  if (_symtab.find(id) == nullptr) {
    std::shared_ptr<gr8::symbol> symbol = std::make_shared<gr8::symbol>(node->type(), id, 0);
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);
  }

  if(node->variables() != nullptr)
    node->variables()->accept(this,lvl+2);
}


//---------------------------------------------------------------------------







/* ============================= POINTER INSTRUCTIONS ========================= */


void gr8::type_checker::do_mem_alloc_node(gr8::mem_alloc_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(new basic_type(4, basic_type::TYPE_POINTER));
    node->type()->_subtype = new basic_type(4, basic_type::TYPE_UNSPEC);
}

void gr8::type_checker::do_ind_pos_node(gr8::ind_pos_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
  node->type()->_subtype=(node->lvalue()->type());
}

void gr8::type_checker::do_indexation_node(gr8::indexation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->step()->accept(this, lvl + 4);

  if(node->step()->type()->name() != basic_type::TYPE_INT)
    throw std::string("Pointer offset must be of type small.");

  node->base()->accept(this, lvl + 4);
  // Must be Pointer
  if(node->base()->type()->name() != basic_type::TYPE_POINTER)
    throw std::string("Cannot get index of non pointer type");
  // Null node case
  if(node->base()->type()->subtype() == nullptr)
    throw std::string("Cannot get index of null pointer");

  node->type(node->base()->type()->subtype());

}


//---------------------------------------------------------------------------






/* ============================ EXPRESSIONS =================================== */

void gr8::type_checker::do_read_node(gr8::read_node * const node, int lvl) {
  ASSERT_UNSPEC;
  if(printOP)
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else
    node->type(new basic_type(4, basic_type::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------









/* ========================= UNARY EXPRESSIONS ================================= */

void gr8::type_checker::do_identity_node(gr8::identity_node *const node, int lvl) {
  processIntDoubleUnaryExpression(node,lvl);
}

void gr8::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  processIntDoubleUnaryExpression(node, lvl);
}

void gr8::type_checker::do_null_node(gr8::null_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
}

//---------------------------------------------------------------------------








/* ======================== VARIABLES ======================================= */

void gr8::type_checker::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<gr8::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void gr8::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void gr8::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl);
  } catch (const std::string &id) {
    throw "undeclared identifier:" + id+ "\n";
  }

  node->rvalue()->accept(this, lvl + 2);

  if(node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE){

    // handle cast of ReadDouble
    if(node->rvalue()->type() != nullptr && node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC){
      node->rvalue()->type( node->lvalue()->type());
    }

    if( !(node->rvalue()->type()->name() == basic_type::TYPE_INT || node->rvalue()->type()->name() == basic_type::TYPE_DOUBLE))
      throw std::string("Can only assign small or huge to huge");

  }else if(node->lvalue()->type()->name() == basic_type::TYPE_INT){

    // handle cast of ReadInt
    if(node->rvalue()->type() != nullptr && node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC){
      node->rvalue()->type( node->lvalue()->type());
    }

    if( node->rvalue()->type()->name() != basic_type::TYPE_INT)
      throw std::string("Can only assign small type to small");

  }else if(node->lvalue()->type()->name() == basic_type::TYPE_POINTER){

    // assign with mem alloc - needs to set type
    if(node->rvalue()->type()->subtype() != nullptr && node->rvalue()->type()->subtype()->name() == basic_type::TYPE_UNSPEC){
      node->rvalue()->type( node->lvalue()->type());

    // subtype matching for pointers
    }else if( !gr8::type_checker::matchable_pointer_types (node->lvalue()->type(), node->rvalue()->type())){
        throw std::string("Left fake subtype must match right fake subtype");
    }

  }else if(node->lvalue()->type()->name() == basic_type::TYPE_STRING){
    if( node->rvalue()->type()->name() != basic_type::TYPE_STRING)
      throw std::string("Left type must match right type: news");

  } else{
    throw std::string("Identifier is typeless");
  }
  node->type(new basic_type(4, basic_type::TYPE_UNSPEC));
}


void gr8::type_checker::do_variable_node(gr8::variable_node *const node, int lvl) {

  const std::string &id = *node->name();

  if (_symtab.find_local(id) != nullptr) {
    throw std::string("Variable redeclared - already exists");
  }

  std::shared_ptr<gr8::symbol> symbol = std::make_shared<gr8::symbol>(node->type(), id, 0);
  _symtab.insert(id, symbol);
  _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted ???? nani TODO:UNDERSTAND THIS


  if(node->expression() != NULL){
    node->expression()->accept(this, lvl + 2);

  if(node->type() == nullptr)
    throw std::string("Variable has null type!!");


  // assign with mem alloc - needs to set type
  if(node->type()->name() == basic_type::TYPE_POINTER){
    if(node->expression()->type()->subtype() != nullptr && node->expression()->type()->subtype()->name() == basic_type::TYPE_UNSPEC){
      node->expression()->type( node->type());
    }
  }

  if ( !gr8::type_checker::matchable_types( node->type()->name(), node->expression()->type()->name()))
    throw std::string("Unable to convert or match expression's type to variable's type.");


  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->expression()->type()->name() == basic_type::TYPE_INT)
    node->expression()->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  }
}

//---------------------------------------------------------------------------









//---------------------------------------------------------------------------




/* ========================== UNARY EXPRESSION ============================= */


void gr8::type_checker::do_nil_node(cdk::nil_node * const node, int lvl) {
}
void gr8::type_checker::do_data_node(cdk::data_node * const node, int lvl) {
}

void gr8::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in argument of unary expression");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::processIntDoubleUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    node->type(new basic_type(4, basic_type::TYPE_INT));

  }else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  }else{
    throw std::string("argument must be of type int or double");
  }
}




/* ========================== BINARY EXPRESSION ============================= */




void gr8::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type in right argument of binary expression");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::processIntDoubleLogicBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT){
    // if not int or double, wrong type in left argument
    if(node->left()->type()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("left argument must be of type int or double");
  }

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT){
    // if not int or double, wrong type in right argument
    if (node->right()->type()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("right argument must be of type int or double");
  }
  //logic operations are always int
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::processIntDoubleBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  bool type_double = false;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT){
    // if not int or double, wrong type in left argument
    if(node->left()->type()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("left argument must be of type int or double");

    type_double = true;
  }

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT){
    // if not int or double, wrong type in right argument
    if (node->right()->type()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("right argument must be of type int or double");

    type_double = true;
  }

  // if any of the arguments is double, then expression is double
  if(type_double){
      node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  }
  else{
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }
}





void gr8::type_checker::do_add_node(cdk::add_node * const node, int lvl) {

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't add strings - left");
  else if (node->right()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't add strings - right");

  // check type matchability
  if( !gr8::type_checker::matchable_types(node->left()->type()->name(), node->right()->type()->name()))
    throw std::string("right argument does not match or convert to left's argument type.");

  // POINTER + INT
  else if (node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_POINTER)
    node->type(node->left()->type());

  // INT + POINTER
  else if(node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_POINTER)
    node->type(node->right()->type());

  // INT + DOUBLE
  else if(node->right()->type()->name() == basic_type::TYPE_DOUBLE || node->left()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  // INT + INT
  else if(node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));

  // POINTER + POINTER
  else if(node->right()->type()->name() == basic_type::TYPE_POINTER && node->left()->type()->name() == basic_type::TYPE_POINTER)
    throw std::string("Cannot add 2 pointers - both right and left are pointers");

  // ??
  else
    throw std::string("Type mismatch in plus");
}


void gr8::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't do strings difference - left");
  else if (node->right()->type()->name() == basic_type::TYPE_STRING) throw std::string("Can't do strings difference - right");

  // check type matchability
  if( !gr8::type_checker::matchable_types(node->left()->type()->name(), node->right()->type()->name()))
    throw std::string("right argument does not match or convert to left's argument type.");

  // POINTER + INT
  else if (node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_POINTER)
    node->type(node->left()->type());

  // INT + POINTER
  else if(node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_POINTER)
    node->type(node->right()->type());

  // INT + DOUBLE
  else if(node->right()->type()->name() == basic_type::TYPE_DOUBLE || node->left()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  // INT + INT
  else if(node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));

  // POINTER - POINTER
  else if(node->right()->type()->name() == basic_type::TYPE_POINTER || node->left()->type()->name() == basic_type::TYPE_POINTER){
    if(!gr8::type_checker::matchable_pointer_types(node->right()->type(), node->left()->type())){
      throw std::string("Cannot subtract 2 pointers of different subtypes");
    }else{
      node->type(new basic_type(4, basic_type::TYPE_INT));
    }
  }
  // INT - INT
  else
    node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void gr8::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void gr8::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
  processIntBinaryExpression(node, lvl);
}





//---------------------------------------------------------------------------




/* ======================== AUXILIARS AND EXTRAS ======================================= */


bool gr8::type_checker::matchable_pointer_types(basic_type *type1, basic_type *type2 ){

  // prevents silly non pointer comparisons
  if(type1->name() != basic_type::TYPE_POINTER || type2->name() != basic_type::TYPE_POINTER ){
    return false;
  }

  // Null literal matches any pointer
  if(type1->subtype() == nullptr || type2->subtype() == nullptr ){
    return true;
  }

  // checks for values
  while((type1->subtype()->name() == basic_type::TYPE_POINTER) && (type2->subtype()->name() == basic_type::TYPE_POINTER)){
    type1 = type1->subtype();
    type2 = type2->subtype();
    // Null literal matches any pointer
    if(type1->subtype() == nullptr || type2->subtype() == nullptr ){
      return true;
    }
  }
  return type1->subtype()->name() == type2->subtype()->name();
}

bool gr8::type_checker::matchable_types(unsigned long int type1, unsigned long int type2 ){
  if(type1 == type2)
    return true;

  if( ( type1 == basic_type::TYPE_INT || type1 == basic_type::TYPE_POINTER) &&
    ( ( type2 == basic_type::TYPE_INT || type2 == basic_type::TYPE_POINTER) ))
    return true;

  if( ( type1 == basic_type::TYPE_INT || type1 == basic_type::TYPE_DOUBLE) &&
    ( ( type2 == basic_type::TYPE_INT || type2 == basic_type::TYPE_DOUBLE) ))
    return true;

  return false;
}



//---------------------------------------------------------------------------
