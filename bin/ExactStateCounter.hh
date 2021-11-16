#ifndef __EXACT_COMPUTER__HH__
#define __EXACT_COMPUTER__HH__

#include "ddd/SDD.h"
#include "ddd/DDD.h"
#include "its/Ordering.hh"

#include <gmpxx.h>
#include "TreeAggregate.hh"

class ExactStateCounter {
public :
  typedef mpz_class stat_t;
private:
  typedef ext_hash_map<IAggregate*, stat_t> smap_t;
  typedef ext_hash_map<GSDD, smap_t > scache_t;
  scache_t scache;
  typedef ext_hash_map<GDDD, smap_t > cache_t;
  cache_t cache;

  bool firstError;

  const IAggregate * tree;
public : 
  ExactStateCounter  () : scache(),cache(),firstError(false),tree(nullptr) {}
  const stat_t & compute (const GSDD & reach, IAggregate * agg=nullptr) ;
  const stat_t & compute (const GDDD & reach, IAggregate * agg=nullptr) ;
  const stat_t & compute (const DataSet * ev, IAggregate * agg=nullptr) ;

  void clear () { cache.clear(); scache.clear(); }
  void printStats (const stat_t & stat, std::ostream & out) const ;
  void setReductionInfo (IAggregate *tree) {
	  this->tree = tree;
  }
};


#endif
