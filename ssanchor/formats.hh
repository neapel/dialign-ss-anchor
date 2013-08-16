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

// Reads the BLOSUM matrix in triangular DIALIGN format:
//   C S T
//
//  13 3 3   C
//     8 5   S
//       9   T
std::istream &operator >>(std::istream &input, blosum_t &mat) {
	using namespace std;
	const auto N = (size_t)residue::Xaa;

	// map file column order to internal order
	array<size_t, N> indices;
	for(size_t i = 0 ; i < N ; i++) {
		char c;
		if(!(input >> c)) throw runtime_error("Expected AA code.");
		indices[i] = (size_t)char_to_aa(c);
	}

	// read matrix.
	for(size_t row = 0 ; row < N ; row++) {
		for(size_t col = row ; col < N ; col++ ) {
			size_t weight;
			if(!(input >> weight)) throw runtime_error("Expected weight.");
			const auto row_i = indices[row], col_i = indices[col];
			mat[row_i][col_i] = mat[col_i][row_i] = weight;
		}
		char c;
		if(!(input >> c)) throw runtime_error("Expected AA code.");
		if(indices[row] != (size_t)char_to_aa(c))
			throw runtime_error("AA order different for row/col.");
	}

	return input;
}


typedef std::vector<std::vector<float>> blosum_weights_t;

// Reads the BLOSUM weights in [runlength sum value]* format.
// Into a 2D vector with dim0: sequence length, dim1: sum
std::istream &operator >>(std::istream &input, blosum_weights_t &w) {
	w.push_back(std::vector<float>());
	size_t len;
	int sum;
	float value;
	while(input >> len >> sum >> value) {
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
	string line;
	while(getline(input, line)) {
		if(line.size() == 0 || line[0] == '#') continue;
		istringstream ss{line};
		size_t index;
		char res, struc;
		if(ss >> index >> res >> struc) {
			array<float, N> data;
			for(auto &x : data) ss >> x;
			seq.push_back(position<N>(char_to_aa(res), data));
		}
	}
	return input;
}



#endif
