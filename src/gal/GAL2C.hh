#ifndef ___GAL2C__H__
#define ___GAL2C__H__

#include "GAL.hh"

namespace its
{

class GAL2C
{
public:
  explicit GAL2C(const GAL& g);

  void
  dump(std::ostream& os) const;

  IntExpression
  remap_expr(const IntExpression&) const;
  BoolExpression
  remap_expr(const BoolExpression&) const;
private:
  const GAL& gal_;
  int next_;
  std::map<std::string, int> vars2index;
  std::map<std::string, int> arrays2index;

  std::string
  new_label(const std::string&) const;

  env_t
  remap_env(const env_t&) const;

};

}

#endif

