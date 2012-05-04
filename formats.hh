#ifndef __BLOSUM_HH__
#define __BLOSUM_HH__

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>

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



typedef std::map<std::string, std::string> sequences;

/** Reads a fasta file into a map. */
sequences read_fasta(std::istream &input) {
	using namespace std;
	sequences seqs;
	string name, data;
	while(input) {
		string line;
		getline(input, line);
		if(line.size() == 0 || line[0] == ';') continue;
		if(line[0] == '>') {
			if(data.size() > 0) seqs[name] = data;
			istringstream ss{line.substr(1)};
			ss >> name;
			data = "";
		} else {
			data.append(line);
		}
	}
	if(data.size() > 0) seqs[name] = data;
	return seqs;
}


#endif
