#ifndef __SCORE_HH__
#define __SCORE_HH__

#include "formats.hh"
#include <vector>
#include <memory>

struct scorer {
	virtual float operator()(const seq_it &a, const seq_it &b, size_t len) const = 0;
};


// Weighted BLOSUM score
struct blosum_score : scorer {
	const blosum_t &blosum;
	const blosum_weights_t &weight;
	blosum_score(const blosum_t &blosum, const blosum_weights_t &weight) : blosum(blosum), weight(weight) {}
	
	virtual float operator()(const seq_it &a, const seq_it &b, size_t len) const {
		if(len >= weight.size()) throw std::domain_error("Sequence too long for weights");
		size_t s = 0;
		for(size_t i = 0 ; i < len ; i++)
			if(a[i].value != residue::Xaa && b[i].value != residue::Xaa)
				s += blosum[static_cast<size_t>(a[i].value)][static_cast<size_t>(b[i].value)];
		if(s >= weight[len].size()) throw std::domain_error("Blosum to large for weights");
		return weight[len][s];
	}
};


struct q3_score : scorer {
	virtual float operator()(const seq_it &a, const seq_it &b, size_t len) const {
		size_t same = 0;
		for(size_t i = 0 ; i < len ; i++) if(a[i].sec == b[i].sec) same++;
		return 1.0f * same / len;
	}
};



// score combinator
struct combined_score {
	const std::vector<std::shared_ptr<scorer>> scorers;

	float operator()(const seq_it &a, const seq_it &b, size_t len) const {
		float value = 1;
		for(const auto &s : scorers)
			value *= (*s)(a, b, len);
		return value;
	}
};


#endif
