#ifndef __AACODES_HH__
#define __AACODES_HH__

#include <stdexcept>


// 3-Letter AA codes in 3-Letter order
enum class residue : size_t {
	// normal amino acids
	Ala, Arg, Asn, Asp, Cys,
	Gln, Glu, Gly, His, Ile,
	Leu, Lys, Met, Phe, Pro,
	Ser, Thr, Trp, Tyr, Val,
	Xaa, // X -- Unknown, last one.
};

static residue char_to_aa(char c) {
	switch(c) {
		case 'a': case 'A': return residue::Ala;
		case 'r': case 'R': return residue::Arg;
		case 'n': case 'N': return residue::Asn;
		case 'd': case 'D': return residue::Asp;
		case 'c': case 'C': return residue::Cys;
		case 'q': case 'Q': return residue::Gln;
		case 'e': case 'E': return residue::Glu;
		case 'g': case 'G': return residue::Gly;
		case 'h': case 'H': return residue::His;
		case 'i': case 'I': return residue::Ile;
		case 'l': case 'L': return residue::Leu;
		case 'k': case 'K': return residue::Lys;
		case 'm': case 'M': return residue::Met;
		case 'f': case 'F': return residue::Phe;
		case 'p': case 'P': return residue::Pro;
		case 's': case 'S': return residue::Ser;
		case 't': case 'T': return residue::Thr;
		case 'w': case 'W': return residue::Trp;
		case 'y': case 'Y': return residue::Tyr;
		case 'v': case 'V': return residue::Val;
		default:            return residue::Xaa;
	}
}

static char aa_to_char(residue r) {
	switch(r) {
		case residue::Ala: return 'A';
		case residue::Arg: return 'R';
		case residue::Asn: return 'N';
		case residue::Asp: return 'D';
		case residue::Cys: return 'C';
		case residue::Gln: return 'Q';
		case residue::Glu: return 'E';
		case residue::Gly: return 'G';
		case residue::His: return 'H';
		case residue::Ile: return 'I';
		case residue::Leu: return 'L';
		case residue::Lys: return 'K';
		case residue::Met: return 'M';
		case residue::Phe: return 'F';
		case residue::Pro: return 'P';
		case residue::Ser: return 'S';
		case residue::Thr: return 'T';
		case residue::Trp: return 'W';
		case residue::Tyr: return 'Y';
		case residue::Val: return 'V';
		case residue::Xaa: return 'X';
		default: throw std::logic_error("Illegal residue.");
	}
}




#endif
