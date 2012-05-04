#include "formats.hh"
#include "align.hh"
#include <boost/program_options.hpp>
#include <fstream>
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
		("blosum", value<string>(&blosum_name),
		 "blosum matrix in 3-column format.");

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
	notify(vm);

	if(vm.count("help")) {
		cerr << desc << endl;
		return EXIT_SUCCESS;
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
	
	// compute all alignments
	for(auto a = input.begin() ; a != input.end() ; a++) {
		auto b = a;
		b++;
		for( ; b != input.end() ; b++) {
			cout << a->first << " & " << b->first << endl;
			align(a->second, b->second, max_length, score, [](float x){return x;});
		}
	}
}
