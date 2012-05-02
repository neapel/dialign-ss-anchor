#ifndef __BLOSUM_HH__
#define __BLOSUM_HH__

#include <iostream>
#include <string>
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


#endif
