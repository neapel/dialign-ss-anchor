CXXFLAGS += --std=c++0x

all : BLOSUM60 test_blosum

test_blosum : blosum.hh

BLOSUM% :
	wget ftp://ftp.ncbi.nih.gov/blast/matrices/$@
