#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <string>
#include "Naming.hh"

#define GCC_VERSION (__GNUC__ * 10000 \
                + __GNUC_MINOR__ * 100 \
                   + __GNUC_PATCHLEVEL__)

namespace its {

class Variable {
  vLabel name;
  vLabel aname;
  int index;
public:
  Variable(Label nname):name(nname) {
    size_t pos = name.find_last_of('[');
    aname = name.substr(0,pos);
    vLabel nstr = name.substr(pos+1, name.find_last_of(']'));
    index=-1;
    sscanf(nstr.c_str(), "%d", &index);
  }
  Label getName () const { return name; }
  bool operator== (const Variable & v) const {
    return v.name == name;
  }
  bool operator<(const Variable &v) const {
    return name < v.name;
  }
  size_t hash () const { 
    return d3::util::hash<vLabel>()(name);
  }
  Label getArrayName () const { 
    return aname; 
  }
  int getIndex () const {
    return index;
  }

};

}


#endif
