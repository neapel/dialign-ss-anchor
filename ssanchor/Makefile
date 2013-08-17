CXX = clang++
#CXX = g++
CXXFLAGS += --std=c++11
CXXFLAGS += -O3 -mtune=native
#CXXFLAGS += -g
LDLIBS += -lboost_program_options

#CXX = icpc
#CXXFLAGS = --std=c++11 -O3 -xHost -g

#CXXFLAGS += -DCAIRO_FOUND
#LDLIBS += -lcairo

all : align

align : align.cc
align.cc : align.hh debug.hh formats.hh score.hh

test_blosum : test_blosum.cc
test_blosum.cc : formats.hh

test_sov : test_sov.cc
test_sov.cc : sov.hh

.PHONY : clean
clean :
	-rm align test_blosum

new : clean all
