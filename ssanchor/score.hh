#ifndef __SCORE_HH__
#define __SCORE_HH__

#include "formats.hh"
#include <array>


// Weighted BLOSUM score
template<class T> struct blosum_score;

template<template<size_t> class sequence_t, size_t N>
struct blosum_score<sequence_t<N>> {
	const blosum_t &blosum;
	const blosum_weights_t &weight;
	blosum_score(const blosum_t &blosum, const blosum_weights_t &weight) : blosum(blosum), weight(weight) {}
	
	template<class Iterator>
	float operator()(const Iterator &a_begin, const Iterator &a_end,
	                 const Iterator &b_begin, const Iterator &b_end) const {
		size_t len = 0, s = 0;
		for(auto a = a_begin, b = b_begin ; a != a_end && b != b_end ; a++, b++, len++)
			if(a->value != residue::Xaa && b->value != residue::Xaa)
				s += blosum[(size_t)a->value][(size_t)b->value];
		if(len >= weight.size()) throw std::domain_error("Sequence too long for weights");
		if(s >= weight[len].size()) {
			//std::cerr << "sum=" << s << " max=" << weight[len].size() << std::endl;
			//return weight[len].back();
			throw std::domain_error("Blosum to large for weights");
		} 
		return weight[len][s];
	}
};


// structure score
template<class T> struct profile_score;

template<template<size_t> class sequence_t, size_t N>
struct profile_score<sequence_t<N>> {
	template<class Iterator>
	float operator()(const Iterator &a_begin, const Iterator &a_end,
	                 const Iterator &b_begin, const Iterator &b_end) const {
		size_t len = 0;
		std::array<float, N> weights_a, weights_b;
		for(size_t k = 0 ; k < N ; k++)
			weights_a[k] = weights_b[k] = 0;
		for(auto a = a_begin, b = b_begin ; a != a_end && b != b_end ; a++, b++, len++)
			for(size_t k = 0 ; k < N ; k++) {
				weights_a[k] += a->aux[k];
				weights_b[k] += b->aux[k];
			}
		float sum = 0;
		for(size_t k = 0 ; k < N ; k++)
			sum += (weights_a[k] / len) * (weights_b[k] / len);
		return sum / N;
	}
};


// score combinator
template<class T0, class T1>
struct combined_score {
	const T0 &s0;
	const T1 &s1;
	combined_score(const T0 &s0, const T1 &s1) : s0(s0), s1(s1) {}

	template<class Iterator>
	float operator()(const Iterator &a_begin, const Iterator &a_end,
	                 const Iterator &b_begin, const Iterator &b_end) const {
		return s0(a_begin, a_end, b_begin, b_end) * s1(a_begin, a_end, b_begin, b_end);
	}
};

template<class T0, class T1>
combined_score<T0, T1> operator *(const T0 &a, const T1 &b) {
	return combined_score<T0, T1>{a, b};
}


#endif
