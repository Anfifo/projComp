#ifndef __GR8_SEMANTICS_SYMBOL_H__
#define __GR8_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace gr8 {

  class symbol {
    basic_type *_type;
    std::string _name;
    long _value; // hack!
    int _offset;
    bool _defined;
    typedef std::vector<basic_type*> argument_types;
    argument_types _args;

  public:
    symbol(basic_type *type, const std::string &name, long value) :
        _type(type), _name(name), _value(value), _defined(false) {
    }

    virtual ~symbol() {
      delete _type;
    }

    basic_type *type() const {
      return _type;
    }
    const std::string &name() const {
      return _name;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    int offset() const {
      return _offset;
    }
    int offset(int o){
      return _offset = o;
    }
    bool defined() const {
      return _defined;
    }
    bool defined(bool d){
      return _defined = d;
    }
    argument_types &args(){
      return _args;
    }
    void addArg(basic_type * arg){
      basic_type *type = new basic_type(arg->size(), arg->name());
      for(basic_type *it = arg->subtype(), *subtype = type->subtype(); it != nullptr; it = it->subtype(), subtype = subtype->subtype()){
        subtype = new basic_type(it->size(), it->name());
      }
      _args.push_back(type);
    }

    basic_type* getArg(size_t i){
      return _args[i];
    }
  };

} // gr8

#endif
