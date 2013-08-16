#include "formats.hh"
#include "align.hh"
#include "score.hh"
#include <boost/program_options.hpp>
#include <string>
#include <fstream>
#include <ostream>
using namespace std;


int main(int argc, char **argv) {
	using namespace boost::program_options;

	string blosum_name, blosum_weights_name;
	vector<string> input_names;

	options_description desc("Usage: " + string(argv[0]) + " [options] input1 input2 ... > output");
	desc.add_options()
		("help,h", "show this help message")
		("blosum,b", value<string>(&blosum_name)->default_value("blosum.dat"),
		 "blosum matrix in 3-column format.")
		("weights,w", value<string>(&blosum_weights_name)->default_value("weights.dat"),
		 "weights for blosum scores by run length.");
#if CAIRO_FOUND
	string debug_name;
	desc.add_options()
		("debug,d", value<string>(&debug_name)->implicit_value("debug.pdf"),
		 "dump the DP matrix into a PDF.");
#endif

	options_description hidden;
	hidden.add_options()
		("input", value<vector<string>>(&input_names)->multitoken(),
		 "input files");

	positional_options_description pos;
	pos.add("input", -1);

	options_description options;
	options.add(desc).add(hidden);

	variables_map vm;
	store(command_line_parser(argc, argv)
		.options(options).positional(pos).run(), vm);

	if(vm.count("help")) {
		cerr << desc << endl;
		return EXIT_SUCCESS;
	}

	notify(vm);
	if(input_names.size() < 2) {
		cerr << "Supply at least two input files." << endl << desc << endl;
		return EXIT_FAILURE;
	}

	// read blosum
	blosum_t blosum;
	{
		ifstream is{blosum_name};
		if(!is) {
			cerr << "Could not read blosum: " << blosum_name << endl;
			return EXIT_FAILURE;
		}
		is >> blosum;
	}

	// read blosum weights
	blosum_weights_t blosum_weights;
	{
		ifstream is{blosum_weights_name};
		if(!is) {
			cerr << "Could not read weights: " << blosum_weights_name << endl;
			return EXIT_FAILURE;
		}
		is >> blosum_weights;
	}
	size_t max_length = blosum_weights.size() - 2;

	// read inputs
	typedef sequence<3> sequence_t;
	vector<sequence_t> inputs;
	for(auto name : input_names) {
		sequence_t input;
		ifstream is{name};
		if(!is) {
			cerr << "Could not read input: " << name << endl;
			return EXIT_FAILURE;
		}
		is >> input;
		inputs.push_back(input);
	}

	// initialise scorers
	blosum_score<sequence_t> s0{blosum, blosum_weights};
	profile_score<sequence_t> s1;

	// compute alignments between each sequence
	for(size_t i1 = 0 ; i1 != inputs.size() ; i1++) {
		for(size_t i2 = i1 + 1 ; i2 != inputs.size() ; i2++) {
			// align
			vector<entry> output;
			auto matrix = align(back_inserter(output), inputs[i1], inputs[i2], max_length,
				s0 * s1
			);
			// dump as dialign anchors
			for(auto e : output)
				cout << (i1 + 1) << ' ' << (i2 + 1)
				     << ' ' << (e.start_i() + 1) << ' ' << (e.start_j() + 1)
				     << ' ' << e.length() << ' ' << e.run_value << '\n';
#if CAIRO_FOUND
			if(debug_name.size() > 0)
				debug(debug_name, input1, input2, matrix, output);
#endif
		}
	}

	return EXIT_SUCCESS;
}
