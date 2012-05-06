#include <string>

// debug output
std::string debug_name;

#include "formats.hh"
#include "align.hh"
#include <boost/program_options.hpp>
#include <fstream>
#include <ostream>
using namespace std;




int main(int argc, char **argv) {
	using namespace boost::program_options;

	int max_length;
	string input_name, output_name, blosum_name;

	options_description desc("Usage: " + string(argv[0]) + " [options] (input|-) (output|-)");
	desc.add_options()
		("help,h", "show this help message")
		("max-length", value<int>(&max_length)->default_value(10),
		 "maximum length of chunks")
		("blosum,b", value<string>(&blosum_name)->required(),
		 "blosum matrix in 3-column format.")
		("debug,d", value<string>(&debug_name)->implicit_value("debug.pdf"),
		 "dump the DP matrix into a PDF.");

	options_description hidden;
	hidden.add_options()
		("input", value<string>(&input_name)->default_value("-"),
		 "input file, or -")
		("output", value<string>(&output_name)->default_value("-"),
		 "output file, or -");

	positional_options_description pos;
	pos.add("input", 1)
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

	// read input
	sequences input;
	if(input_name == "-") input = read_fasta(cin);
	else {
		ifstream i{input_name};
		input = read_fasta(i);
	}

	// read blosum
	ifstream blosum_file{blosum_name};
	blosum score = read_blosum(blosum_file);

	// output
	ostream *output;
	if(output_name == "-") output = &cout;
	else output = new ofstream{output_name};

	// compute all alignments
	for(auto a = input.begin() ; a != input.end() ; a++) {
		auto b = a;
		b++;
		for( ; b != input.end() ; b++) {
			auto runs = align(a->second, b->second, max_length, score, [](float x){return x;});
			for(auto e : runs)
				*output << a->first << ' ' << b->first << '\t'
				        << e.start_i() << ' ' << e.start_j() << ' ' << e.length()
				        << ' ' << e.run_value << '\n'
				        << "; " << a->second.substr(e.start_i(), e.length()) << '\n'
				        << "; " << b->second.substr(e.start_j(), e.length()) << '\n';
		}
	}
}
