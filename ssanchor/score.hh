#ifndef __SCORE_HH__
#define __SCORE_HH__

#include "formats.hh"
#include <vector>
#include <memory>


// Weighted BLOSUM score
struct blosum_score {
	const blosum_t &blosum;
	const blosum_weights_t &weight;
	blosum_score(const blosum_t &blosum, const blosum_weights_t &weight) : blosum(blosum), weight(weight) {}
	
	float operator()(const seq_it &a, const seq_it &b, size_t len) const {
		//assert(len < weight.size());
		size_t s = 0;
		for(size_t i = 0 ; i < len ; i++)
			if(a[i].value != residue::Xaa && b[i].value != residue::Xaa)
				s += blosum[static_cast<size_t>(a[i].value)][static_cast<size_t>(b[i].value)];
		//assert(s < weight[len].size());
		return weight[len][s];
	}
};


struct q3_score {
	float operator()(const seq_it &a, const seq_it &b, size_t len) const {
		size_t same = 0;
		for(size_t i = 0 ; i < len ; i++) if(a[i].sec == b[i].sec) same++;
		return 1.0f * same / len;
	}
};


struct dummy_score {
	float operator()(const seq_it &, const seq_it &, size_t) const {
		return 1;
	}
};



#endif
