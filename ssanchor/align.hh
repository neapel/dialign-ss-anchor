#ifndef __ALIGN_HH__
#define __ALIGN_HH__

#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>

struct entry {
	float value, run_value;
	size_t i, j, n;
	entry() : value(0), run_value(0), i(0), j(0), n(0) {}
	entry(size_t i, size_t j, size_t n) : value(0), run_value(0), i(i), j(j), n(n) {}

	size_t start_i() { return i - n; }
	size_t start_j() { return j - n; }
	size_t length() { return n + 1; }
};

bool operator<(const entry &a, const entry &b) {
	return a.value < b.value;
}
bool operator==(const entry &a, const entry &b) {
	return a.i == b.i && a.j == b.j && a.n == b.n && a.value == b.value && a.run_value == b.run_value;
}
bool operator!=(const entry &a, const entry &b) {
	return !(a == b);
}

std::ostream &operator<<(std::ostream &o, const entry &e) {
	return o << "i=" << e.i << " j=" << e.j << " n=" << e.n << " run-value=" << e.run_value << " total=" << e.value;
}


typedef std::vector< std::vector<entry> > matrix_t;


/* f(i,j) = max {
 *   f(i-1, j)
 *   f(i, j-1)
 *   max { n = 0 .. max_run:
 *     f(i-n-1, j-n-1) + weight(sum_{k=0}^n score(a_{i-k}, b_{j-k}) )
 *   }
 * }
 */
template<class OutputIterator, class Score>
matrix_t align(OutputIterator out, const sequence &a, const sequence &b, size_t max_run, Score score) {
	using namespace std;
	matrix_t f{a.size(), vector<entry>(b.size(), entry())};
	vector<entry> cases; cases.reserve(max_run + 5);

	// Find maximum score
	for(size_t i = 0 ; i < a.size() ; i++)
		for(size_t j = 0 ; j < b.size() ; j++) {
			cases.clear();
			// shortest to longest run.
			for(size_t n = 0 ; n <= max_run && n <= i && n <= j ; n++) {
				entry e{i, j, n};
				// score this run
				e.value = e.run_value = score(a.begin() + (i - n), b.begin() + (j - n), n + 1);
				// best run before this one
				if(n < i && n < j)
					e.value += f[i - n - 1][j - n - 1].value;
				cases.push_back(e);
			}
			// rect. maxima:
			if(i > 0) cases.push_back(f[i - 1][j]);
			if(j > 0) cases.push_back(f[i][j - 1]);
			// find largest score from back (i.e. equal score with lower n looses)
			f[i][j] = *max_element(cases.rbegin(), cases.rend());
		}

	// Traceback
	entry e = f[a.size() - 1][b.size() - 1];
	while(e.n > 0) {
		*out++ = e;
		if(e.n < e.i && e.n < e.j)
			e = f[e.i - e.n - 1][e.j - e.n - 1];
		else break;
	}

	return f;
}

#endif
