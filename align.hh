#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

struct entry {
	float value, run_value;
	size_t i, j, n;
	entry() : value(0), run_value(0), i(0), j(0), n(0) {}
	entry(size_t i, size_t j, size_t n) : value(0), run_value(0), i(i), j(j), n(n) {}
};

bool operator<(const entry &a, const entry &b) {
	if(a.value == b.value) return a.n < b.n;
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


template<typename T, typename T1>
void debug(std::string a, std::string b, const T &f, const T1 &runs) {
	// a/i rows=x. b/j columns=y.
	const double cell = 10;
	auto surf = cairo_pdf_surface_create("debug.pdf", (a.size() + 5) * cell, (b.size() + 5) * cell);
	auto ctx = cairo_create(surf);
	cairo_set_line_width(ctx, cell/20);
	cairo_translate(ctx, 2 * cell, 2 * cell);

	// runs
	cairo_save(ctx);
	cairo_set_source_rgba(ctx, 1, 0.75, 0, 1);
	cairo_set_line_width(ctx, cell);
	cairo_set_line_cap(ctx, CAIRO_LINE_CAP_ROUND);
	for(auto e : runs) {
		const double k = 0.49;
		cairo_save(ctx);
		cairo_translate(ctx, e.i * cell, e.j * cell);
		cairo_move_to(ctx, (1-k) * cell, (1-k) * cell);
		cairo_line_to(ctx, -(e.n - k) * cell, -(e.n - k) * cell);
		cairo_stroke(ctx);
		cairo_restore(ctx);
	}
	cairo_restore(ctx);

	// cells
	for(size_t i = 0 ; i < a.size() ; i++) {
		for(size_t j = 0 ; j < b.size() ; j++) {
			const auto e = f[i][j];
			const bool left = i == 0 || f[i - 1][j] != e;
			const bool top = j == 0 || f[i][j - 1] != e;
			cairo_save(ctx);
			cairo_translate(ctx, i * cell, j * cell);
			cairo_move_to(ctx, 0, 0);
			cairo_line_to(ctx, cell, 0);
			cairo_set_source_rgba(ctx, 0, 0, 0, top ? 1.0 : 0.1);
			cairo_stroke(ctx);

			cairo_move_to(ctx, 0, 0);
			cairo_line_to(ctx, 0, cell);
			cairo_set_source_rgba(ctx, 0, 0, 0, left ? 1.0 : 0.1);
			cairo_stroke(ctx);

			if(top & left) {
				cairo_move_to(ctx, 0.75 * cell, 0.75 * cell);
				cairo_line_to(ctx, -(e.n - 0.25) * cell, -(e.n - 0.25) * cell);
				cairo_set_source_rgba(ctx, 1, 0, 0, 1.0/(e.n + 1));
				cairo_set_line_width(ctx, cell/5);
				cairo_stroke(ctx);
			}

			cairo_restore(ctx);
		}
	}

	cairo_destroy(ctx);
	cairo_surface_destroy(surf);
}




/* f(i,j) = max {
 *   f(i-1, j)
 *   f(i, j-1)
 *   max { n = 0 .. max_run:
 *     f(i-n-1, j-n-1) + weight(sum_{k=0}^n score(a_{i-k}, b_{j-k}) )
 *   }
 * }
 */
void align(std::string a, std::string b, size_t max_run, blosum score, std::function<float(float)> weight) {
	using namespace std;
	vector<vector<entry>> f{a.size(), vector<entry>{b.size(), entry()}};

	// Find maximum score
	for(size_t i = 0 ; i < a.size() ; i++)
		for(size_t j = 0 ; j < b.size() ; j++) {
			vector<entry> cases;
			if(i > 0) cases.push_back(f[i - 1][j]);
			if(j > 0) cases.push_back(f[i][j - 1]);
			float run_value = 0;
			for(size_t n = 0 ; n <= max_run && n <= i && n <= j ; n++) {
				const float pos_score = score.at(a[i - n]).at(b[j - n]);
				run_value += pos_score;
				entry e{i, j, n};
				e.value = e.run_value = weight(run_value);
				if(n < i && n < j)
					e.value += f[i - n - 1][j - n - 1].value;
				//if(n > 1)
				cases.push_back(e);
			}
			if(cases.size() > 0)
			f[i][j] = *max_element(cases.begin(), cases.end());
		}

	// Traceback
	vector<entry> runs;
	entry e = f[a.size() - 1][b.size() - 1];
	while(true) {
		runs.push_back(e);
		cout << e << endl;
		cout << " a:" << a.substr(e.i - e.n, e.n + 1) << endl;
		cout << " b:" << b.substr(e.j - e.n, e.n + 1) << endl;
		if(e.n < e.i && e.n < e.j)
			e = f[e.i - e.n - 1][e.j - e.n - 1];
		else break;
	}

	// Debug
	debug(a, b, f, runs);

}

