#ifndef __BLOSUM_HH__
#define __BLOSUM_HH__

#include "aacodes.hh"

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <array>
#include <limits>
#include <stdexcept>

typedef std::array<std::array<size_t, (size_t)residue::Xaa>, (size_t)residue::Xaa> blosum_t;

// Reads the BLOSUM matrix in [row column value]* format.
// Doesn't care about symmetry.
std::istream &operator >>(std::istream &input, blosum_t &mat) {
	using namespace std;
	const size_t guard = numeric_limits<size_t>::max();
	for(auto &row : mat)
		for(auto &col : row)
			col = guard;
	while(input) {
		char row, col;
		size_t value;
		input >> row >> col >> value;
		residue row_r = char_to_aa(row), col_r = char_to_aa(col);
		if(row_r != residue::Xaa && col_r != residue::Xaa)
			mat[(size_t)row_r][(size_t)col_r] = value;
	}

	for(size_t i = 0 ; i < mat.size() ; i++)
		for(size_t j = 0 ; j < mat[i].size() ; j++)
			if(mat[i][j] == guard) {
				ostringstream e; e << "Missing BLOSUM value: " << aa_to_char((residue)i) << '/' << aa_to_char((residue)j);
				throw runtime_error(e.str());
			}
	return input;
}


typedef std::vector<std::vector<float>> blosum_weights_t;

// Reads the BLOSUM weights in [runlength sum value]* format.
// Into a 2D vector with dim0: sequence length, dim1: sum
std::istream &operator >>(std::istream &input, blosum_weights_t &w) {
	w.push_back(std::vector<float>());
	while(input) {
		size_t len;
		int sum;
		float value;
		input >> len >> sum >> value;
		if(len + 1 != w.size()) w.push_back(std::vector<float>());
		w.back().push_back(value);
		if(w[len][sum] != value) throw std::runtime_error("Invalid weights");
	}
	return input;
}


// Sequence with auxiliary information.
template<size_t N>
struct position {
	residue value;
	std::array<float, N> aux;
	position(residue v, std::array<float, N> a = std::array<float, N>{}) : value(v), aux(a) {}
};

template<size_t N>
struct sequence : std::vector<position<N>> {
	typedef typename std::vector<position<N>>::const_iterator const_iterator;

	sequence() {}

	template<class Iterator>
	sequence(Iterator begin, Iterator end) : std::vector<position<N>>(begin, end) {}
};


// Reads a sequence in PSIPRED VFORMAT format:
// comments with '#', blank lines.
// columns: index, residue, structure, C prob, H prob, E prob
template<size_t N>
std::istream &operator >>(std::istream &input, sequence<N> &seq) {
	using namespace std;
	while(input) {
		string line;
		getline(input, line);
		if(line.size() == 0 || line[0] == '#') continue;
		istringstream ss{line};
		size_t index;
		char res, struc;
		ss >> index >> res >> struc;
		array<float, N> data;
		for(auto &x : data) ss >> x;
		seq.push_back(position<N>(char_to_aa(res), data));
	}
	return input;
}



#endif
