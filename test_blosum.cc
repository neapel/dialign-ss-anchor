#include "formats.hh"
#include <iomanip>
#include <fstream>
using namespace std;

int main(int argc, char **argv) {
	if(argc != 2) return EXIT_FAILURE;

	// read
	blosum_t b;
	ifstream ifs{argv[1]};
	ifs >> b;

	// dump
	cout << "  ";
	for(size_t i = 0 ; i < 20 ; i++) cout << ' ' << aa_to_char((residue)i);
	cout << '\n';
	for(size_t i = 0 ; i < 20 ; i++) {
		cout << aa_to_char((residue)i) << ' ';
		for(size_t j = 0 ; j < 20 ; j++)
			cout << setw(2) << b[i][j];
		cout << '\n';
	}

	return EXIT_SUCCESS;
}
