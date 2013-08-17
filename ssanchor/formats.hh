#ifndef __FORMATS_HH__
#define __FORMATS_HH__

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

typedef std::array<std::array<size_t, static_cast<size_t>(residue::Xaa)>, static_cast<size_t>(residue::Xaa)> blosum_t;

// Reads the BLOSUM matrix in triangular DIALIGN format:
//   C S T
//
//  13 3 3   C
//     8 5   S
//       9   T
void read_blosum(std::istream &input, blosum_t &mat) {
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
}




typedef std::vector<std::vector<float>> blosum_weights_t;

// Reads the BLOSUM weights in [runlength sum value]* format.
// Into a 2D vector with dim0: sequence length, dim1: sum
void read_weights(std::istream &input, blosum_weights_t &w) {
	w.push_back(std::vector<float>());
	size_t len;
	int sum;
	float value;
	while(input >> len >> sum >> value) {
		if(len + 1 != w.size()) w.push_back(std::vector<float>());
		w.back().push_back(value);
		if(w[len][sum] != value) throw std::runtime_error("Invalid weights");
	}
}



// Sequence with auxiliary information.
struct position {
	residue value;
	secondary sec;
	position(residue v, secondary s) : value(v), sec(s) {}
};

typedef std::vector<position> sequence;
typedef sequence::const_iterator seq_it;


// Reads a sequence in PSIPRED VFORMAT format:
// comments with '#', blank lines.
// columns: index, residue, structure, {C prob, H prob, E prob}
void read_psipred(std::istream &input, sequence &seq) {
	using namespace std;
	string line;
	while(getline(input, line)) {
		if(line.size() == 0 || line[0] == '#') continue;
		istringstream ss{line};
		size_t index;
		char res, struc;
		if(ss >> index >> res >> struc) {
			seq.push_back(position(char_to_aa(res), char_to_sec(struc)));
		}
	}
}

// IPSSP FASTA format:
// >name
// residue*
// structure*
void read_ipssp(std::istream &input, sequence &seq) {
	using namespace std;
	string name, residue, structure;
	if(!getline(input, name) || name.size() == 0 || name[0] != '>')
		throw runtime_error("Expected >{name}.");
	if(!getline(input, residue) || residue.size() == 0)
		throw runtime_error("Expected residues.");
	if(!getline(input, structure) || structure.size() == 0)
		throw runtime_error("Expected structure.");
	if(residue.size() != structure.size())
		throw runtime_error("Expected structure info for each residue.");
	for(size_t i = 0 ; i < residue.size() ; i++)
		seq.push_back(position(char_to_aa(residue[i]), char_to_sec(structure[i])));
}



#endif
