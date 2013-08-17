#ifndef __AACODES_HH__
#define __AACODES_HH__

#include <stdexcept>


// 3-Letter AA codes in 3-Letter order
enum residue {
	// normal amino acids
	Ala, Arg, Asn, Asp, Cys,
	Gln, Glu, Gly, His, Ile,
	Leu, Lys, Met, Phe, Pro,
	Ser, Thr, Trp, Tyr, Val,
	Xaa, // X -- Unknown, last one.
};

static residue char_to_aa(char c) {
	switch(c) {
		case 'a': case 'A': return Ala;
		case 'r': case 'R': return Arg;
		case 'n': case 'N': return Asn;
		case 'd': case 'D': return Asp;
		case 'c': case 'C': return Cys;
		case 'q': case 'Q': return Gln;
		case 'e': case 'E': return Glu;
		case 'g': case 'G': return Gly;
		case 'h': case 'H': return His;
		case 'i': case 'I': return Ile;
		case 'l': case 'L': return Leu;
		case 'k': case 'K': return Lys;
		case 'm': case 'M': return Met;
		case 'f': case 'F': return Phe;
		case 'p': case 'P': return Pro;
		case 's': case 'S': return Ser;
		case 't': case 'T': return Thr;
		case 'w': case 'W': return Trp;
		case 'y': case 'Y': return Tyr;
		case 'v': case 'V': return Val;
		default:            return Xaa;
	}
}

static char aa_to_char(residue r) {
	switch(r) {
		case Ala: return 'A';
		case Arg: return 'R';
		case Asn: return 'N';
		case Asp: return 'D';
		case Cys: return 'C';
		case Gln: return 'Q';
		case Glu: return 'E';
		case Gly: return 'G';
		case His: return 'H';
		case Ile: return 'I';
		case Leu: return 'L';
		case Lys: return 'K';
		case Met: return 'M';
		case Phe: return 'F';
		case Pro: return 'P';
		case Ser: return 'S';
		case Thr: return 'T';
		case Trp: return 'W';
		case Tyr: return 'Y';
		case Val: return 'V';
		case Xaa: return 'X';
		default: throw std::logic_error("Illegal residue.");
	}
}


// Secondary structure codes.
// psipred uses: H,G=H (alpha helix) / E,B=E (beta) / *=C (coil)
// ipssp uses EHL mapping: H,G,I=H (alpha) / E,B=E (beta) / *=L (coil)
enum secondary {
	Helix, Strand, Other,
};

static secondary char_to_sec(char c) {
	switch(c) {
		case 'h': case 'H': return Helix;
		case 'e': case 'E': return Strand;
		case 'c': case 'C': case 'l': case 'L': return Other;
		default: throw std::runtime_error("Illegal input.");
	}
}

static char sec_to_char(secondary s) {
	switch(s) {
		case Helix: return 'H';
		case Strand: return 'E';
		case Other: return 'C';
		default: throw std::logic_error("Illegal secondary.");
	}
}




#endif
