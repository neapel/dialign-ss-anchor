#ifndef __BLOSUM_HH__
#define __BLOSUM_HH__

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

typedef std::unordered_map<char, std::unordered_map<char, float>> blosum;

/** Reads the BLOSUM matrix in [row column value]* format.
 * Doesn't care about symmetry.
 * @Return a row→col→value map.
 */
blosum read_blosum(std::istream &input) {
	blosum mat;
	while(input) {
		char row, col;
		float value;
		input >> row >> col >> value;
		mat[row][col] = value;
	}
	return mat;
}

typedef std::unordered_map<size_t, std::unordered_map<int, float>> blosum_weights;

// Reads the BLOSUM weights in [runlength sum value]* format.
blosum_weights read_blosum_weights(std::istream &input) {
	blosum_weights w;
	while(input) {
		size_t len;
		int sum;
		float value;
		input >> len >> sum >> value;
		w[len][sum] = value;
	}
	return w;
}


/** Sequence with auxiliary information. */
struct position {
	char value;
	std::vector<float> aux;
	position(char v, std::vector<float> a = std::vector<float>()) : value(v), aux(a) {}
	operator char() { return value; }
};

typedef std::vector<position> sequence;

typedef std::map<std::string, sequence> named_sequences;

/** Reads a fasta file into a map. */
named_sequences read_fasta(std::istream &input) {
	using namespace std;
	named_sequences seqs;
	string name;
	sequence data;
	while(input) {
		string line;
		getline(input, line);
		if(line.size() == 0 || line[0] == ';') continue;
		if(line[0] == '>') {
			if(data.size() > 0) seqs[name] = data;
			istringstream ss{line.substr(1)};
			ss >> name;
			data = sequence();
		} else {
			for(auto c : line)
				data.push_back(position(c));
		}
	}
	if(data.size() > 0) seqs[name] = data;
	return seqs;
}


/** Reads a sequence in PSIPRED VFORMAT format:
 * comments with '#', blank lines.
 * columns: index, residue, structure, C prob, H prob, E prob
 */
sequence read_vformat(std::istream &input) {
	using namespace std;
	sequence seq;
	while(input) {
		string line;
		getline(input, line);
		if(line.size() == 0 || line[0] == '#') continue;
		istringstream ss{line};
		size_t index;
		char res, struc;
		ss >> index >> res >> struc;
		vector<float> data;
		while(ss) {
			float value = -1;
			ss >> value;
			if(ss) data.push_back(value);
		}
		seq.push_back(position(res, data));
	}
	return seq;
}



#endif
