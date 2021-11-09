#ifndef __EXACT_COMPUTER__HH__
#define __EXACT_COMPUTER__HH__

#include "ddd/SDD.h"
#include "ddd/DDD.h"
#include "its/Ordering.hh"

#include <gmpxx.h>

class ExactStateCounter {
public :
  typedef mpz_class stat_t;
private:
  typedef ext_hash_map<GSDD, stat_t> scache_t;
  scache_t scache;
  typedef ext_hash_map<GDDD, stat_t> cache_t;
  cache_t cache;

  bool firstError;

  const std::unordered_map<std::string,int> * mapRed;
  const its::VarOrder * vo;
public : 
  ExactStateCounter  () : scache(),cache(),firstError(false),mapRed(nullptr),vo(nullptr) {}
  const stat_t & compute (const GSDD & reach) ;
  const stat_t & compute (const GDDD & reach) ;
  const stat_t & compute (const DataSet * ev) ;

  void clear () { cache.clear(); scache.clear(); }
  void printStats (const stat_t & stat, std::ostream & out) const ;
  void setReductionInfo (const std::unordered_map<std::string,int> * mapRed, const its::VarOrder * vo) {
	  this->mapRed=mapRed;
	  this->vo = vo;
  }
};


#endif
