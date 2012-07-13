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

	string input1_name, input2_name, output_name, blosum_name, blosum_weights_name;

	options_description desc("Usage: " + string(argv[0]) + " [options] input1 input2 (output|-)");
	desc.add_options()
		("help,h", "show this help message")
		("blosum,b", value<string>(&blosum_name)->required(),
		 "blosum matrix in 3-column format.")
		("weights,w", value<string>(&blosum_weights_name)->required(),
		 "weights for blosum scores by run length.");
#if CAIRO_FOUND
	string debug_name;
	desc.add_options()
		("debug,d", value<string>(&debug_name)->implicit_value("debug.pdf"),
		 "dump the DP matrix into a PDF.");
#endif

	options_description hidden;
	hidden.add_options()
		("input1", value<string>(&input1_name)->required(),
		 "input file")
		("input2", value<string>(&input2_name)->required(),
		 "input file")
		("output", value<string>(&output_name)->default_value("-"),
		 "output file, or -");

	positional_options_description pos;
	pos.add("input1", 1)
		.add("input2", 1)
		.add("output", 1);

	options_description options;
	options.add(desc).add(hidden);

	variables_map vm;
	store(command_line_parser(argc, argv)
		.options(options).positional(pos).run(), vm);

	if(vm.count("help")) {
		cerr << desc << endl;
		return EXIT_SUCCESS;
	}

	try {
		notify(vm);
	} catch(required_option e) {
		cerr << e.what() << endl << desc << endl;
		return EXIT_FAILURE;
	}

	// read blosum
	blosum_t blosum;
	{
		ifstream i{blosum_name};
		i >> blosum;
	}

	// read blosum weights
	blosum_weights_t blosum_weights;
	{
		ifstream i{blosum_weights_name};
		i >> blosum_weights;
	}
	size_t max_length = blosum_weights.size() - 2;

	// read input
	typedef sequence<3> sequence_t;
	sequence_t input1, input2;
	{
		ifstream i1{input1_name};
		i1 >> input1;
		ifstream i2{input2_name};
		i2 >> input2;
	}

	// output
	ostream *output_file;
	if(output_name == "-") output_file = &cout;
	else output_file = new ofstream{output_name};

	// scorers
	blosum_score<sequence_t> s0{blosum, blosum_weights};
	profile_score<sequence_t> s1;

	// compute alignment
	vector<entry> output;
	auto matrix = align(back_inserter(output), input1, input2, max_length,
		s0 * s1
	);
	for(auto e : output)
		*output_file << (e.start_i() + 1) << ' ' << (e.start_j() + 1) << ' ' << e.length()
		        << ' ' << e.run_value << '\n';

#if CAIRO_FOUND
	if(debug_name.size() > 0)
		debug(debug_name, input1, input2, matrix, output);
#endif
}
