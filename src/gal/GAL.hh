#ifndef ___GAL__H__
#define ___GAL__H__

#include <string>
#include <utility>
#include <iosfwd>
#include <map>

// hash_map from libddd, for initial state
#include "util/configuration.hh"

#include "BoolExpression.hh"
#include "Variable.hh"
#include "IntExpression.hh"


namespace its {

/** 
 * An assignment is a basic atomic action that updates a single variable of the GAL 
 * based on an expression on the current variable states.
 * e.g. X = Y + ( 2 * X )
*/ 
class Assignment {
  // Note : the lhs should be of type Variable or ArrayAccess, ultimately resolved to a variable.
  // It is an error if the lhs resolves to a constant value.
  IntExpression var_;
  // A contrario, the rhs should evaluate to a constant
  IntExpression expr_;
    
  public :
  /// Constructor, by reference since IntExpr are by construction unique
  Assignment (const IntExpression & var, const IntExpression & expr) : var_(var), expr_(expr) {}
  /// Returns the left hand side of the Assignment
  const IntExpression & getVariable () const { return var_; }
  /// Returns the right hand side of the Assignment
  const IntExpression & getExpression () const { return expr_; }
  /// pretty print
  void print (std::ostream & os) const;
  
  /// simplifies the current expression by asserting the value of a variable.
  Assignment operator&(const Assertion &) const;
  /// To get all the variables occuring in the expression
  std::set<Variable> getSupport() const;
  /// equality comparison
  bool operator==(const Assignment &) const;
};

/**
 * A guarded action can occur in any state where its guard evaluates to true.
 * Its effect is atomic, and is defined as a list of actions, 
 * which might be assignments or list operations. The actions are executed in the order they are added.
 * Swapping two variable values thus requires a temporary space. 
*/
class GuardedAction : public NamedElement {
public :
  typedef std::vector<Assignment> actions_t;
  typedef actions_t::const_iterator actions_it;
private :
  BoolExpression guard_;
  actions_t actions_;
public:
  GuardedAction (Label name):NamedElement(name),guard_(BoolExpressionFactory::createConstant(true)) {};
  void setGuard (const BoolExpression & guard) { guard_ = guard.eval(); }
  const BoolExpression & getGuard () const { return guard_; }
  actions_it begin() const { return actions_.begin() ; }
  actions_it end() const { return actions_.end() ; }
  void addAction (const Assignment & ass) { actions_.push_back(ass);}
  void print (std::ostream & os) const;
  
  /// To get all the variables occuring in the guard expression or the actions
  std::set<Variable> getSupport() const;
  
  bool operator==(const GuardedAction &) const;
};

class GALState {
public :
  typedef std::map<vLabel,int> state_t;
  typedef state_t::const_iterator const_iterator;
private :
  state_t state_;

public :
  void setVarValue (Label var, int val) {
    state_.insert(std::make_pair(var,val));
  }

  int getVarValue (Label var) const {
    const_iterator access = state_.find(var);
    if (access != state_.end()) return access->second;     
    return -1;
  }

  const_iterator begin() const { return state_.begin() ; }
  const_iterator end() const { return state_.end() ; }

};

class ArrayDeclaration {
public:
  typedef std::vector<Variable> vars_t;
  typedef vars_t::const_iterator vars_it;
private:
  vars_t vars_;
  vLabel name_;
  size_t size_;
public:
  ArrayDeclaration (Label name, size_t size);

  Label getName() const { return name_; }
  size_t size() const {return size_; }

  vars_it vars_begin() const { return vars_.begin() ; }
  vars_it vars_end() const { return vars_.end() ; }

};

/** A class to represent a Guarded Action Language model. *
 */
class GAL : public NamedElement {
public :
  typedef std::vector<Variable> vars_t;
  typedef vars_t::const_iterator vars_it;

  // to set initial values
  typedef std::vector<int> vals_t;
  typedef std::vector<ArrayDeclaration> arrays_t;
  typedef arrays_t::const_iterator arrays_it;    

  typedef std::vector<GuardedAction> trans_t;
  typedef trans_t::const_iterator trans_it;

  typedef GALState state_t;
  typedef state_t::const_iterator state_it;

  
private :
  vars_t variables_;
  arrays_t arrays_;
  trans_t transitions_;
  state_t init_;
  BoolExpression transient_;
public :
    GAL (Label name) : NamedElement(name), transient_(BoolExpressionFactory::createConstant(false)) {};
  
  void addVariable(const Variable & var, int value) { 
    variables_.push_back(var); 
    init_.setVarValue(var.getName(),value);
  }
  ///  Declare an array with tab name "name" and initial values vals. 
  ///  size of array is deduced from vals.
  void addArray (Label name, const vals_t & vals) ;

  void addTransition (const GuardedAction & act) { transitions_.push_back(act); }  

  vars_it vars_begin() const { return variables_.begin() ; }
  vars_it vars_end() const { return variables_.end() ; }
  arrays_it arrays_begin() const { return arrays_.begin() ; }
  arrays_it arrays_end() const { return arrays_.end() ; }
  trans_it trans_begin() const { return transitions_.begin() ; }
  trans_it trans_end() const { return transitions_.end() ; }
  state_it state_begin() const { return init_.begin() ; }
  state_it state_end() const { return init_.end() ; }
  int getVarValue (Label name) const {
    return init_.getVarValue(name);
  }

  const BoolExpression & isTransientState () const {
    return transient_;
  }

  void setTransientPredicate (const BoolExpression & expr) {
    transient_ = expr.eval();
  }

};


  std::ostream & operator<< ( std::ostream &, const GAL &); 

}


#endif // include protection
