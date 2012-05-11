#include "formats.hh"
#include "align.hh"
#include <boost/program_options.hpp>
#include <string>
#include <fstream>
#include <ostream>
using namespace std;


int main(int argc, char **argv) {
	using namespace boost::program_options;

	string input1_name, input2_name, output_name, blosum_name, blosum_weights_name, debug_name;

	options_description desc("Usage: " + string(argv[0]) + " [options] input1 input2 (output|-)");
	desc.add_options()
		("help,h", "show this help message")
		("blosum,b", value<string>(&blosum_name)->required(),
		 "blosum matrix in 3-column format.")
		("weights,w", value<string>(&blosum_weights_name)->required(),
		 "weights for blosum scores by run length.")
		("debug,d", value<string>(&debug_name)->implicit_value("debug.pdf"),
		 "dump the DP matrix into a PDF.");

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
	ifstream blosum_file{blosum_name};
	blosum score = read_blosum(blosum_file);

	// read blosum weights
	ifstream blosum_weights_file{blosum_weights_name};
	blosum_weights score_weight = read_blosum_weights(blosum_weights_file);
	size_t max_length = 0;
	for(auto p : score_weight) if(p.first > max_length) max_length = p.first;

	// read input
	ifstream input1_file{input1_name}, input2_file{input2_name};
	auto input1 = read_vformat(input1_file), input2 = read_vformat(input2_file);

	// output
	ostream *output_file;
	if(output_name == "-") output_file = &cout;
	else output_file = new ofstream{output_name};

	// compute alignment
	vector<entry> output;
	auto matrix = align(back_inserter(output), input1, input2, max_length,
		[&score, &score_weight](sequence a, sequence b) {
			const size_t len = a.size(); // == b.size()
			const size_t aux_len = a[0].aux.size();
			// weighted BLOSUM score
			int blosum = 0;
			for(size_t i = 0 ; i < len ; i++)
				blosum += score[a[i]][b[i]];
			const float weight = score_weight[len][blosum];
			// structure score
			float struc_weight = 0;
			for(size_t k = 0 ; k < aux_len ; k++) {
				float sum_a = 0, sum_b = 0;
				for(size_t i = 0 ; i < len ; i++) {
					sum_a += a[i].aux[k];
					sum_b += b[i].aux[k];
				}
				struc_weight += (sum_a / aux_len) * (sum_b / aux_len);
			}
			struc_weight /= aux_len;
			return weight * struc_weight;
		}
	);
	for(auto e : output)
		*output_file << (e.start_i() + 1) << ' ' << (e.start_j() + 1) << ' ' << e.length()
		        << ' ' << e.run_value << '\n';

	if(debug_name.size() > 0)
		debug(debug_name, input1, input2, matrix, output);
}
