#include "formats.hh"
#include "align.hh"
#include "score.hh"
#include "sov.hh"
#include "debug.hh"

#include <boost/program_options.hpp>
#include <string>
#include <fstream>
#include <ostream>
#include <memory>

using namespace std;


int main(int argc, char **argv) {
	using namespace boost::program_options;

	string blosum_name, blosum_weights_name;
	vector<string> input_names;
	bool use_ipssp_format, use_sov_score, use_q3_score;

	options_description desc("Usage: " + string(argv[0]) + " [options] input1 input2 ... > output");
	desc.add_options()
		("help,h", "show this help message")
		("blosum,b", value<string>(&blosum_name)->default_value("blosum.dat"),
		 "BLOSUM matrix in DIALIGN format.")
		("weights,w", value<string>(&blosum_weights_name)->default_value("weights.dat"),
		 "weights for BLOSUM scores by run length.")
		("ipssp", bool_switch(&use_ipssp_format),
		 "input in IPSSP-FASTA format instead of PSIPRED-VFORMAT.")
		;
#if CAIRO_FOUND
	string debug_name;
	desc.add_options()
		("debug,d", value<string>(&debug_name)->implicit_value("debug.pdf"),
		 "dump the DP matrix into a PDF.");
#endif
	options_description score("Combine (multiply) the BLOSUM score with");
	score.add_options()
		("sov", bool_switch(&use_sov_score),
		 "Sequence Overlap (SOV) scorer.")
		("q3", bool_switch(&use_q3_score),
		 "Q3 scorer.")
		;
	desc.add(score);


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
		read_blosum(is, blosum);
	}

	// read blosum weights
	blosum_weights_t blosum_weights;
	{
		ifstream is{blosum_weights_name};
		if(!is) {
			cerr << "Could not read weights: " << blosum_weights_name << endl;
			return EXIT_FAILURE;
		}
		read_weights(is, blosum_weights);
	}
	size_t max_length = blosum_weights.size() - 2;

	// read inputs
	vector<sequence> inputs;
	for(auto name : input_names) {
		sequence input;
		ifstream is{name};
		if(!is) {
			cerr << "Could not read input: " << name << endl;
			return EXIT_FAILURE;
		}
		if(use_ipssp_format) read_ipssp(is, input);
		else read_psipred(is, input);
		inputs.push_back(input);
	}

	// compute alignments between each sequence
	for(size_t i1 = 0 ; i1 != inputs.size() ; i1++) {
		for(size_t i2 = i1 + 1 ; i2 != inputs.size() ; i2++) {
			// initialise scorers
			vector<shared_ptr<scorer>> sc;
			sc.push_back(make_shared<blosum_score>(blosum, blosum_weights));
			if(use_sov_score) sc.push_back(make_shared<sov_score>(true));
			if(use_q3_score) sc.push_back(make_shared<q3_score>());
			combined_score scorer{sc};

			// align
			vector<entry> output;
			auto matrix = align(back_inserter(output), inputs[i1], inputs[i2], max_length, scorer);
			// dump as dialign anchors
			for(auto e : output)
				cout << (i1 + 1) << ' ' << (i2 + 1)
				     << ' ' << (e.start_i() + 1) << ' ' << (e.start_j() + 1)
				     << ' ' << e.length() << ' ' << e.run_value << '\n';
#if CAIRO_FOUND
			if(debug_name.size() > 0)
				debug(debug_name, inputs[i1], inputs[i2], matrix, output);
#endif
		}
	}

	return EXIT_SUCCESS;
}
