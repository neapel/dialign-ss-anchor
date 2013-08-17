#ifndef __DEBUG_HH__
#define __DEBUG_HH__
#if CAIRO_FOUND

#include "formats.hh"
#include "align.hh"

#include <vector>
#include <string>
#include <sstream>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

template<typename T, typename T1>
void debug(std::string filename, sequence a, sequence b, const T &f, const T1 &runs) {
	using namespace std;
	const float colors[][3] = {
		{196/255.0, 117/255.0, 126/255.0},
		{235/255.0, 177/255.0, 61/255.0},
		{157/255.0, 184/255.0, 210/255.0}
	};
	const float line_c[] = {110/255.0, 158/255.0, 110/255.0};
	const float mark_c[] = {238/255.0, 214/255.0, 128/255.0, 0.5};

	// a/i rows=x. b/j columns=y.
	const double cell = 10, hcell = cell/2;
	const float aux_margin = hcell;
	auto surf = cairo_pdf_surface_create(filename.c_str(), (a.size() + 3) * cell + aux_margin, (b.size() + 3) * cell + aux_margin);
	auto ctx = cairo_create(surf);
	cairo_set_line_width(ctx, cell/20);
	cairo_translate(ctx, 2 * cell, 2 * cell);
	cairo_translate(ctx, aux_margin, aux_margin);

	// aux info
	cairo_save(ctx);
	const float off = -2 * cell;
	for(size_t i = 0 ; i < a.size() ; i++) {
		const size_t k = static_cast<size_t>(a[i].sec);
		cairo_rectangle(ctx, i * cell, off, cell, hcell);
		cairo_set_source_rgb(ctx, colors[k][0], colors[k][1], colors[k][2]);
		cairo_fill(ctx);
	}
	for(size_t j = 0 ; j < b.size() ; j++) {
		const size_t k = static_cast<size_t>(b[j].sec);
		cairo_rectangle(ctx, off, j * cell, hcell, cell);
		cairo_set_source_rgb(ctx, colors[k][0], colors[k][1], colors[k][2]);
		cairo_fill(ctx);
	}
	cairo_restore(ctx);

	// sequences
	cairo_save(ctx);
	cairo_select_font_face(ctx, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(ctx, cell);
	const double margin = 0.3 * cell;
	for(size_t i = 0 ; i < a.size() ; i++) {
		char text[2] = {aa_to_char(a[i].value), '\0'};
		cairo_text_extents_t extents;
		cairo_text_extents(ctx, text, &extents);
		cairo_move_to(ctx, (i + 0.5) * cell - extents.width/2 - extents.x_bearing, -margin);
		cairo_show_text(ctx, text);
	}

	cairo_font_extents_t extents;
	cairo_font_extents(ctx, &extents);
	cairo_translate(ctx, -cell, 0.5 * cell + (extents.ascent - extents.descent)/2);

	for(size_t j = 0 ; j < b.size() ; j++) {
		char text[2] = {aa_to_char(b[j].value), '\0'};
		cairo_move_to(ctx, 0, j * cell);
		cairo_show_text(ctx, text);
	}
	cairo_restore(ctx);
	

	// grid
	cairo_save(ctx);
	for(size_t i = 0 ; i <= a.size() ; i++) {
		cairo_move_to(ctx, i * cell, 0);
		cairo_rel_line_to(ctx, 0, b.size() * cell);
	}
	for(size_t j = 0 ; j <= b.size() ; j++) {
		cairo_move_to(ctx, 0, j * cell);
		cairo_rel_line_to(ctx, a.size() * cell, 0);
	}
	cairo_set_source_rgba(ctx, 0, 0, 0, 0.05);
	cairo_set_line_width(ctx, cell/40);
	cairo_stroke(ctx);
	cairo_restore(ctx);
	
	// cell borders
	vector<entry> corners;
	cairo_save(ctx);
	for(size_t i = 0 ; i < a.size() ; i++) {
		for(size_t j = 0 ; j < b.size() ; j++) {
			const auto e = f[i][j];
			const bool left = i == 0 || f[i - 1][j] != e;
			const bool top = j == 0 || f[i][j - 1] != e;
			if(left) {
				cairo_move_to(ctx, i * cell, j * cell);
				cairo_rel_line_to(ctx, 0, cell);
			}
			if(top) {
				cairo_move_to(ctx, i * cell, j * cell);
				cairo_rel_line_to(ctx, cell, 0);
			}
			if(left & top)
				corners.push_back(e);
		}
	}
	cairo_rectangle(ctx, 0, 0, a.size() * cell, b.size() * cell);
	cairo_set_source_rgb(ctx, 0, 0, 0);
	cairo_stroke(ctx);
	cairo_restore(ctx);

	// runs
	cairo_save(ctx);
	cairo_set_source_rgba(ctx, mark_c[0], mark_c[1], mark_c[2], mark_c[3]);
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
	cairo_save(ctx);
	cairo_set_line_width(ctx, cell/5);
	for(auto e : corners) {
		cairo_save(ctx);
		cairo_translate(ctx, e.i * cell, e.j * cell);
		cairo_move_to(ctx, 0.75 * cell, 0.75 * cell);
		cairo_line_to(ctx, -(e.n - 0.25) * cell, -(e.n - 0.25) * cell);
		cairo_set_source_rgba(ctx, line_c[0], line_c[1], line_c[2], 1.0/(e.n + 1));
		cairo_stroke(ctx);
		cairo_restore(ctx);
	}
	cairo_restore(ctx);

	// scores
	cairo_save(ctx);
	cairo_select_font_face(ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(ctx, cell/4);
	for(auto e : corners) {
		cairo_move_to(ctx, (e.i + 0.1) * cell, (e.j + 0.9) * cell);
		ostringstream text;
		text << e.value << ", " << e.run_value << ", n=" << e.n;
		cairo_show_text(ctx, text.str().c_str());
	}
	cairo_restore(ctx);

	cairo_destroy(ctx);
	cairo_surface_destroy(surf);
}


#endif
#endif
