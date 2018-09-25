#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/memory_management.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated

//---------------------------------------------------------------------------
bool func_def = false;    // inside function definition
int offset = 0;           // used to calculate local variable offsets
bool isArg = false;       // processing function arguments
bool isGlobal = false;    // is global variable
int leaveFunc_lbl = 0;    // label end  of function
bool inMain = false;      // inside main function
bool isTrash = false;     // trash arguments after function call returns
std::string *calledFunc;  // called function
bool sub = false;

std::vector<int> sweep_end_lbls;    // stores the labels of the sweep increments
std::vector<int> sweep_inc_lbls;   // stores the labels of the sweep ends
std::vector<std::string> function_decl;

int haxCounter = 0;  // counter of sequence nodes visited

/* ============================ LITERALS =============================== */


void gr8::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(isGlobal)
    _pf.SDOUBLE(node->value());
  else
    _pf.DOUBLE(node->value()); // push an integer
}

void gr8::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    if(isGlobal)
      _pf.SDOUBLE(node->value());
    else
      _pf.DOUBLE(node->value()); // push an integer
  }else{
    if(isGlobal)
      _pf.SINT(node->value());
    else
      _pf.INT(node->value()); // push an integer
  }
}

void gr8::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {

    if(isGlobal)
      _pf.SSTRING(node->value());
    else{
      int lbl1;

      /* generate the string */
      _pf.RODATA(); // strings are DATA readonly
      _pf.ALIGN(); // make sure we are aligned
      _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
      _pf.SSTRING(node->value()); // output string characters
      /* leave the address on the stack */
      _pf.TEXT(); // return to the TEXT segment
      _pf.ADDR(mklbl(lbl1)); // the string to be printed
  }
}

/* ===================== LOGIC OPERATIONS ================================ */

void gr8::postfix_writer::processIntDoubleLogicBinaryExpression(cdk::binary_expression_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node->right());
  CHECK_TYPES(_compiler, _symtab, node->left());

  node->left()->accept(this, lvl);

  if(node->right()->type()->name() == basic_type::TYPE_DOUBLE &&
      node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
  }

  node->right()->accept(this, lvl);

  if(node->left()->type()->name() == basic_type::TYPE_DOUBLE &&
      node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
  }

  if(node->left()->type()->name() == basic_type::TYPE_DOUBLE ||
    node->right()->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.DCMP();
    _pf.INT(0);
  }
}


void gr8::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  node->argument()->accept(this, lvl);
  _pf.INT(0);
  _pf.EQ();
}

void gr8::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->left()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->right()->accept(this, lvl);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
}

void gr8::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->left()->accept(this, lvl);
    _pf.JNZ(mklbl(lbl1 = ++_lbl));
  node->right()->accept(this, lvl);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
}

void gr8::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node->right());
  CHECK_TYPES(_compiler, _symtab, node->left());

  node->left()->accept(this, lvl);

  if(node->right()->type()->name() == basic_type::TYPE_DOUBLE &&
      node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
  }

  node->right()->accept(this, lvl);

  if(node->left()->type()->name() == basic_type::TYPE_DOUBLE &&
      node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
  }

  if(node->left()->type()->name() == basic_type::TYPE_DOUBLE ||
    node->right()->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.DCMP();
    _pf.INT(0);
  }
  _pf.EQ();
}


/* ========================== INSTRUCTIONS ================================== */
void gr8::postfix_writer::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.TRASH(8);
  } else if (node->argument()->type()->name() == basic_type::TYPE_POINTER) {
    _pf.TRASH(4);
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void gr8::postfix_writer::do_print_node(gr8::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.CALL("printd");
    _pf.TRASH(8); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  if(node->newline())
    _pf.CALL("println"); // print a newline
}

void gr8::postfix_writer::do_if_node(gr8::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}



void gr8::postfix_writer::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

void gr8::postfix_writer::do_return_node(gr8::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(inMain){
    // end the main function
    _pf.INT(0);
    _pf.STFVAL32();
  }else{
    if(node->argument() != nullptr){
      node->argument()->accept(this, lvl);
      // modify to check node->type
      if(node->type()->name() == basic_type::TYPE_DOUBLE){
          if(node->argument()->type()->name() == basic_type::TYPE_INT)
            _pf.I2D();

          _pf.STFVAL64();
      }
      else
        _pf.STFVAL32();
      }
  }
  _pf.JMP(mklbl(leaveFunc_lbl));
}

void gr8::postfix_writer::do_again_node(gr8::again_node *const node, int lvl) {
  int index = sweep_inc_lbls.size() - node->argument();
  if( index < 0){
    index = 0;
  }
  int lbl = sweep_inc_lbls[index];
  _pf.JMP(mklbl(lbl));
}

void gr8::postfix_writer::do_stop_node(gr8::stop_node *const node, int lvl) {
  int index = sweep_end_lbls.size() - node->argument();
  if( index < 0){
    index = 0;
  }
  int lbl = sweep_end_lbls[index];
  _pf.JMP(mklbl(lbl));
}

void gr8::postfix_writer::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int cond_lbl = ++_lbl;
  int cond_neg_lbl = ++_lbl;;
  int block_lbl = ++_lbl;
  int inc_lbl = ++_lbl;
  sweep_inc_lbls.push_back(inc_lbl);
  int end_lbl = ++_lbl;
  sweep_end_lbls.push_back(end_lbl);
  //inicializacao
  node->expression1()->accept(this,lvl);
  node->lvalue()->accept(this, lvl);
  _pf.STINT();

  _pf.ALIGN();
  _pf.LABEL(mklbl(cond_lbl));
  node->lvalue()->accept(this, lvl);
  _pf.LDINT();
  //verifica valor incremento
  node->expression3()->accept(this, lvl);
  _pf.INT(0);
  _pf.GT();
  _pf.JZ(mklbl(cond_neg_lbl));

  // incremento positivo

  node->expression2()->accept(this, lvl);
  _pf.GT();
  _pf.JNZ(mklbl(end_lbl));
  _pf.JMP(mklbl(block_lbl));

  _pf.ALIGN();
  _pf.LABEL(mklbl(cond_neg_lbl));
  node->expression2()->accept(this, lvl);
  _pf.LT();
  _pf.JNZ(mklbl(end_lbl));

  _pf.ALIGN();
  _pf.LABEL(mklbl(block_lbl));
  node->block()->accept(this, lvl);

  _pf.ALIGN();
  _pf.LABEL(mklbl(inc_lbl));
  node->lvalue()->accept(this, lvl);
  _pf.LDINT();
  node->expression3()->accept(this,lvl);
  _pf.ADD();
  node->lvalue()->accept(this, lvl);
  _pf.STINT();
  _pf.JMP(mklbl(cond_lbl));

  _pf.ALIGN();
  _pf.LABEL(mklbl(end_lbl));

  sweep_inc_lbls.pop_back();
  sweep_end_lbls.pop_back();
}


/* =========================== FUNCTIONS ======================================= */
void gr8::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  int sequenceNr = haxCounter++;

  if(isTrash){
    for(int i = node->size()-1; i >= 0 ; i--){
        processTrash((cdk::typed_node*)node->node(i),lvl);
    }
  }else if(isArg){
      for(int i = node->size()-1; i >= 0 ; i--){
        node->node(i)->accept(this, lvl);
    }

  }else{
    std::shared_ptr<gr8::symbol> symbol;
    if(calledFunc != nullptr){
      symbol =_symtab.find(*calledFunc);
    }

    for (size_t i = 0; i < node->size(); i++) {
      node->node(i)->accept(this, lvl);
      if(symbol != nullptr && !symbol->args().empty()){
        if(symbol->getArg(i)->name() == basic_type::TYPE_DOUBLE )
          implicitConversion((cdk::typed_node*)node->node(i),lvl);
      }
    }
  }
  if(sequenceNr == 0){ // its the end of the program
    std::shared_ptr<gr8::symbol> symbol;
    for(std::string fname : function_decl){
        symbol =_symtab.find(fname);
        if(!symbol->defined()){
          _pf.EXTERN(fname);
        }
    }
  }
}


void gr8::postfix_writer::do_invocation_node(gr8::invocation_node *const node, int lvl) {

  ASSERT_SAFE_EXPRESSIONS;
  if(node->arguments() != nullptr){
    calledFunc = node->name();

    node->arguments()->accept(this, lvl);
    isArg = false;
    calledFunc = nullptr;
  }

  _pf.CALL(*node->name());
  if(node->arguments() != nullptr){
    isTrash = true;
    node->arguments()->accept(this, lvl);
    isTrash = false;
  }

  if(node->type()->name() != basic_type::TYPE_UNSPEC){
    if(node->type()->name() == basic_type::TYPE_DOUBLE){
      _pf.LDFVAL64();
    }else{
      _pf.LDFVAL32();
    }
  }
}

void gr8::postfix_writer::do_block_node(gr8::block_node *const node, int lvl) {
  if(node->variables() != NULL)
    node->variables()->accept(this, lvl);

  if(node->instructions() != NULL)
    node->instructions()->accept(this,lvl);
}

void gr8::postfix_writer::do_function_node(gr8::function_node *const node, int lvl) {
    func_def = true;
    ASSERT_SAFE_EXPRESSIONS;
    ASSERT_LOCAL_MEMORY_SPACE;
    // generate the main function (RTS mandates that its name be "_main")
    std::shared_ptr<gr8::symbol> symbol =_symtab.find(*node->name());

      _pf.TEXT();
      _pf.ALIGN();
      if(*node->name() == "covfefe"){
        inMain = true;
        _pf.GLOBAL("_main", _pf.FUNC());
        _pf.LABEL("_main");
      }else{
        _pf.GLOBAL(*node->name(), _pf.FUNC());
        _pf.LABEL(*node->name());
      }
      _pf.ENTER(symbol->offset());  // Simple didn't implement local variables
      _symtab.push();
      offset = 8;
      isArg = true;
      if(node->variables() != nullptr)
        node->variables()->accept(this, lvl);

      offset = -4;
      isArg = false;
      leaveFunc_lbl = ++_lbl;
      FUNCTION_BEGIN
      node->block()->accept(this, lvl);
      FUNCTION_END
      _symtab.pop();

      _pf.ALIGN();
      _pf.LABEL(mklbl(leaveFunc_lbl));
      _pf.LEAVE();
      _pf.RET();

      if(*node->name() == "covfefe"){
        // these are just a few library function imports
        _pf.EXTERN("readi");
        _pf.EXTERN("readd");
        _pf.EXTERN("printi");
        _pf.EXTERN("prints");
        _pf.EXTERN("printd");
        _pf.EXTERN("println");
        _pf.EXTERN("argc");
        _pf.EXTERN("argv");
      }
    func_def = false;
    inMain = false;
}

void gr8::postfix_writer::do_function_prototype_node(gr8::function_prototype_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  function_decl.push_back(*node->name());
}


/* ============================= POINTER INSTRUCTIONS ========================= */
void gr8::postfix_writer::do_ind_pos_node(gr8::ind_pos_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
}
void gr8::postfix_writer::do_indexation_node(gr8::indexation_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->base()->accept(this, lvl);
  // base is right value does ldint
  node->step()->accept(this, lvl);
  //_pf.LDINT();  //set is also rvalue
  if(node->base()->type()->subtype()->name() == basic_type::TYPE_DOUBLE){
    _pf.INT(8);
  }else{
    _pf.INT(4);
  }
  _pf.MUL();
  _pf.ADD();
}

void gr8::postfix_writer::do_mem_alloc_node(gr8::mem_alloc_node *const node, int lvl) {

  node->argument()->accept(this, lvl);
  if(node->type()->subtype()->name() == basic_type::TYPE_DOUBLE){
    _pf.INT(8);
  }else{
    _pf.INT(4);
  }
  _pf.MUL();
  _pf.ALLOC();
  _pf.SP();
}

/* ============================ EXPRESSIONS =================================== */
void gr8::postfix_writer::do_read_node(gr8::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->type()->name() == basic_type::TYPE_INT){
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }else{
    _pf.CALL("readd");
    _pf.LDFVAL64();
  }
}

/* ========================= UNARY EXPRESSIONS ================================= */
void gr8::postfix_writer::do_identity_node(gr8::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}

void gr8::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DNEG();
  else
    _pf.NEG(); // 2-complement
}





void gr8::postfix_writer::do_null_node(gr8::null_node *const node, int lvl) {
  // EMPTY
}

/* ======================== VARIABLEs ======================================= */

void gr8::postfix_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  std::shared_ptr<gr8::symbol> symbol =_symtab.find(node->name());
  if(symbol->offset() != 0){
    _pf.LOCAL(symbol->offset());
  }else{
      _pf.ADDR(node->name());
  }
}

void gr8::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->lvalue()->accept(this, lvl);
  if(sub) return;
  if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.LDDOUBLE();
  else
    _pf.LDINT(); //strings, int and pointer are 4 bytes

}

void gr8::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl);

  if(node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE && node->rvalue()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();
  node->lvalue()->accept(this, lvl);

  if(node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.STDOUBLE();
  }
  else
    _pf.STINT();
}

void gr8::postfix_writer::do_variable_node(gr8::variable_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if((node->qualifier() != nullptr && *node->qualifier() == "public") || !func_def){
    isGlobal = true;
    if(node->type()->name() == basic_type::TYPE_STRING){

      int lbl1;

      _pf.RODATA();
      _pf.ALIGN();
      _pf.LABEL(mklbl(lbl1 = ++_lbl));
      node->expression()->accept(this, lvl);


      _pf.DATA();
      _pf.ALIGN();
      _pf.GLOBAL(*node->name(), _pf.OBJ());
      _pf.LABEL(*node->name());
      _pf.SADDR(mklbl(lbl1));
    }else{
      if(node->expression() == nullptr)
        _pf.BSS();   //variavel nao inicializada
      else
        _pf.DATA(); //variavel inicializada

      _pf.ALIGN();
      _pf.GLOBAL(*node->name(), _pf.OBJ());
      _pf.LABEL(*node->name());

      if(node->expression() == nullptr){ //var nao inicializada
        if(node->type()->name() == basic_type::TYPE_DOUBLE)
          _pf.SALLOC(8);
        else
          _pf.SALLOC(4);

      }else{ //variavel inicializada
        node->expression()->accept(this, lvl);
      }
    }
    isGlobal = false;
  }else{
    std::shared_ptr<gr8::symbol> symbol =_symtab.find(*node->name());
    if(isArg){
      if(node->type()->name() == basic_type::TYPE_DOUBLE){
        offset = symbol->offset(offset) + 8;
      }else{
        offset = symbol->offset(offset) + 4;
      }
    }else{
      if(node->type()->name() == basic_type::TYPE_DOUBLE){
        offset = symbol->offset(offset) - 8;
      }else{
        offset = symbol->offset(offset) - 4;
      }
    }
    if(node->expression() != nullptr &&
      !(node->expression()->type()->name() == basic_type::TYPE_POINTER && node->expression()->type()->subtype() == nullptr)){ /// initialized a null
      node->expression()->accept(this, lvl);
      _pf.LOCAL(symbol->offset());
      if(node->type()->name() == basic_type::TYPE_DOUBLE)
        _pf.STDOUBLE();
      else
        _pf.STINT();

    }
  }
}


//---------------------------------------------------------------------------
void gr8::postfix_writer::processTrash(cdk::typed_node * const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;

  if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.TRASH(8);
  else
    _pf.TRASH(4);
}

void gr8::postfix_writer::implicitConversion(cdk::typed_node * const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;

  if(node->type()->name() == basic_type::TYPE_INT){
    _pf.I2D();
  }
}

/* ========================== BINARY EXPRESSION ============================= */



void gr8::postfix_writer::processIntDoubleBinaryArgs(cdk::binary_expression_node * const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    if(node->left()->type()->name() == basic_type::TYPE_INT){
      _pf.I2D();
    }
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    if(node->right()->type()->name() == basic_type::TYPE_INT){
      _pf.I2D();
    }
  }
}

void gr8::postfix_writer::processIntDoublePointerBinaryArgs(cdk::binary_expression_node * const node, int lvl){

  ASSERT_SAFE_EXPRESSIONS;


  node->left()->accept(this, lvl);
  if(node->right()->type()->name()  == basic_type::TYPE_POINTER){
    if(node->right()->type()->subtype()->name() ==basic_type::TYPE_DOUBLE)
      _pf.INT(8);
    else
      _pf.INT(4);
    _pf.MUL();
  }
  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    if(node->left()->type()->name() == basic_type::TYPE_INT){
      _pf.I2D();
    }
  }
  node->right()->accept(this, lvl);
  if(node->left()->type()->name()  == basic_type::TYPE_POINTER){
    if(node->left()->type()->subtype()->name() ==basic_type::TYPE_DOUBLE)
      _pf.INT(8);
    else
      _pf.INT(4);
    _pf.MUL();
  }
  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    if(node->right()->type()->name() == basic_type::TYPE_INT){
      _pf.I2D();
    }
  }
}

void gr8::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  processIntDoublePointerBinaryArgs(node, lvl);

  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.DADD();
  }else{
    _pf.ADD();
  }
}

void gr8::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS
  if(node->right()->type()->name()  == basic_type::TYPE_POINTER &&
    node->left()->type()->name()  == basic_type::TYPE_POINTER){
      node->left()->accept(this, lvl);
      node->right()->accept(this, lvl);
      sub = true;
      _pf.SUB();
      if(node->left()->type()->subtype()->name() == basic_type::TYPE_DOUBLE)
        _pf.INT(8);
      else
        _pf.INT(4);
      _pf.DIV();
      sub = false;
      return;
  }

  processIntDoublePointerBinaryArgs(node, lvl);

  if(node->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.DSUB();
  }else{
    _pf.SUB();
  }
}
void gr8::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  processIntDoubleBinaryArgs(node, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL();
}
void gr8::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  processIntDoubleBinaryArgs(node, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}
void gr8::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void gr8::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  processIntDoubleLogicBinaryExpression(node, lvl);
  _pf.GT();
}

void gr8::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  processIntDoubleLogicBinaryExpression(node, lvl);
  _pf.LT();
}

/* =================== DOES NOT EXIST IN GR8 ============================= */
void gr8::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void gr8::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}

void gr8::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}

void gr8::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // DOES NO
}
void gr8::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
