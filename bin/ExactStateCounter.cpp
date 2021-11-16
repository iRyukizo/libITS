#include "ExactStateCounter.hh"
#include <iostream>

const ExactStateCounter::stat_t & one_stat (){
  static ExactStateCounter::stat_t one_stat = 1 ;
  return one_stat;
}

const ExactStateCounter::stat_t & zero_stat (){
  static ExactStateCounter::stat_t zero_stat = 0 ;
  return zero_stat;
}



const ExactStateCounter::stat_t & ExactStateCounter::compute (const GSDD & d, IAggregate * agg) {
  if (d == GSDD::one || d == GSDD::top) {
    return one_stat();
  } else if (d == GSDD::null) {
    return zero_stat();
  }

  scache_t::accessor accessFirst;
  scache.find(accessFirst,d);
  if (accessFirst.empty()) {
	  scache.insert(accessFirst, d);
	  accessFirst->second = smap_t();
  }
  smap_t & map = accessFirst->second;

  smap_t::accessor access;
  map.find(access, agg);
  if( access.empty() ) {
    // miss
    stat_t res = 0;
    size_t childIndex = 0;
    for(GSDD::const_iterator gi=d.begin();gi!=d.end();++gi,++childIndex) {
      stat_t childStat = compute (gi->second, agg);
      const stat_t & edgeStat = compute (gi->first, agg==NULL ? NULL : agg->getChild(childIndex));
      stat_t prod = childStat * edgeStat ;
      res = res  + prod ;
    }

    map.insert(access,agg);
    access->second = res ;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}

#include "gmp.h"

ExactStateCounter::stat_t arrangements (int nbToken, int nbRepresents) {
	// Number of way of putting m black balls in n boxes
	// C (n+m-1, m)
	mpz_t res;
	mpz_init(res);
	mpz_bin_uiui(res, nbRepresents+ nbToken -1, nbToken);
	ExactStateCounter::stat_t resmpz(res);
	mpz_clear(res);
	return resmpz;
}

const ExactStateCounter::stat_t & ExactStateCounter::compute (const GDDD & d, IAggregate * agg) {
  if (d == GDDD::one || d == GDDD::top) {
    return one_stat();
  } else if (d == GDDD::null) {
    return zero_stat();
  }

  cache_t::accessor accessFirst;
  cache.find(accessFirst,d);
  if (accessFirst.empty()) {
	  cache.insert(accessFirst, d);
	  accessFirst->second = smap_t();
  }
  smap_t & map = accessFirst->second;

  smap_t::accessor access;
  map.find(access, agg);

  if( access.empty() ) {
    // miss
	  stat_t res = 0;
	  for(GDDD::const_iterator gi=d.begin();gi!=d.end();++gi) {
		  const stat_t & childStat = compute (gi->second, agg);
		  // check if we have multiplier for this variable
		  if (agg != nullptr) {
			  size_t represents = agg->getAggregateCount(d.variable());
			  if (represents != 1) {
				  // we do have multiplier
				  int val = gi->first;
				  stat_t nbTotal = arrangements (val, represents);
				  res += nbTotal * childStat;
			  } else {
				  res += childStat;
			  }
		  } else {
			  res += childStat;
		  }
    }

    map.insert(access,agg);
    access->second = res;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}


const ExactStateCounter::stat_t & ExactStateCounter::compute (const DataSet* g, IAggregate * agg)
{
    // Used to work for referenced DDD
    if (typeid(*g) == typeid(GSDD) ) {
      return compute ( GSDD ((SDD &) *g), agg );
    } else if (typeid(*g) == typeid(DDD)) {
      return compute ( GDDD ((DDD &) *g), agg );
      //    } else if (typeid(*g) == typeid(IntDataSet)) {
      // nothing, no nodes for this implem
      //return stat_t();
    } else {
      if (firstError) {
        std::cerr << "Warning : unknown referenced dataset type on arc, node count is inacurate"<<std::endl;
        std::cerr << "Read type :" << typeid(*g).name() <<std::endl ;
	firstError = false;
      }
      return zero_stat();
    }
}

void ExactStateCounter::printStats (const ExactStateCounter::stat_t & stat, std::ostream & out) const  {
  out << "Exact state count : " << stat << std::endl;
}
