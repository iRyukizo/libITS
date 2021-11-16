#ifndef BIN_TREEAGGREGATE_HH_
#define BIN_TREEAGGREGATE_HH_

#include <vector>
#include <string>
#include <memory>

class IAggregate {

public :
	virtual ~IAggregate (){}

	virtual void setChild (size_t childIndex, IAggregate * child)=0;
	virtual const IAggregate * cgetChild (size_t childIndex) const=0;
	virtual IAggregate * getChild (size_t childIndex)=0;
	virtual size_t nbChildren() const = 0;

	virtual void setAggregateCount (size_t varIndex, size_t aggCount)=0;
	virtual size_t getAggregateCount (size_t varIndex) const =0;
	virtual size_t hash() const = 0;
	virtual bool operator==(const IAggregate & o) const = 0;
};

class FlatAggregate : public IAggregate {
	// for each variable index, how many places are represented
	std::vector<int> represents;
public :
	FlatAggregate (size_t nbVariables) : represents(nbVariables,1) {}

	void setAggregateCount (size_t varIndex, size_t aggCount) {
		represents[varIndex] = aggCount;
	}
	size_t getAggregateCount (size_t varIndex) const {
		return represents.at(varIndex);
	}

	void setChild (size_t childIndex, IAggregate * child) {
		throw "UnsupportedOperationException setChild";
	}
	IAggregate * getChild (size_t childIndex) {
		throw "UnsupportedOperationException getChild";
	}
	const IAggregate * cgetChild (size_t childIndex) const {
		throw "UnsupportedOperationException cgetChild";
	}
	size_t nbChildren() const {
		return 0;
	}

	size_t hash() const {
		return d3::util::hash<std::vector<int> >()(represents);
	}
	bool operator==(const IAggregate & o) const {
		if (o.nbChildren() != 0) {
			return false;
		} else {
			// TODO Hugo : choisir le bon cast
			const FlatAggregate * fa = (const FlatAggregate *) & o;
			return represents == fa->represents;
		}
	}
};


class CompositeAggregate : public IAggregate {
	// for each variable index, give child aggregate structure
	std::vector< std::unique_ptr<IAggregate> > children;
public :
	CompositeAggregate (size_t nbChildren):children(nbChildren) {}

	void setChild (size_t childIndex, IAggregate * child) {
		children[childIndex] = std::unique_ptr<IAggregate>(child);
	}
	IAggregate * getChild (size_t childIndex) {
		return children [childIndex].get();
	}
	const IAggregate * cgetChild (size_t childIndex) const {
		return children.at(childIndex).get();
	}
	size_t nbChildren() const {
		return children.size();
	}
	void setAggregateCount (size_t varIndex, size_t aggCount) {
		throw "UnsupportedOperationException setAggregateCount";
	}
	size_t getAggregateCount (size_t varIndex) const {
		throw "UnsupportedOperationException getAggregateCount";
	}

	size_t hash() const {
		size_t res = 23981;
		for (const auto & pa : children) {
			res ^= ddd::wang32_hash(pa->hash()) * res;
		}
		return res;
	}

	bool operator==(const IAggregate & o) const {
		if (o.nbChildren() != nbChildren()) {
			return false;
		} else {
			for (size_t i=0; i < children.size() ; i++) {
				if ( ! (* children.at(i) == * o.cgetChild(i))) {
					return false;
				}
			}
			return true;
		}
	}
};



#endif /* BIN_TREEAGGREGATE_HH_ */
