#ifndef __SOV_HH__
#define __SOV_HH__

/*
helix  H
strand E
other  C

s1, s2 : overlapping segments.

pairs of overlapping segments
S(i) = (s1, s2) : s1 n s2 != 0 && s1=i && s2=i

segments without overlapping s2:
S'(i) = s1 : forall s2 : s1 n s2 = 0 && s1=i && s2=i

sov-score for type i
Sov(i) = 100 * 1 / N(i) * Sov1(i)
Sov1(i) = sum_{(s1,s2) in S(i)} (minov(s1, s2) + delta(s1, s2)) / maxov(s1, s2) * len(s1)

normalization value
N(i) = sum_{(s1,_) in S(i)} len(s1)
       + sum_{s1 in S'(i)} len(s1)

length of actual overlap
minov(s1,s2) = min(right(s1), right(s2)) - max(left(s1), left(s2))

total extent of segments
maxov(s1,s2) = max(right(s1), right(s2)) - min(left(s1), left(s2))

delta(s1,s2) = min(
		maxov(s1,s2) - minov(s1,s2),
		minov(s1,s2),
		int( len(s1)/2 ),
		int( len(s2)/2 )
	)


Sov = 100 * 1 / (N(H) + N(E) + N(C)) * (Sov1(H) + Sov1(E) + Sov1(C))
*/


#include <vector>
#include <array>


struct sov_score : scorer {
	const bool symmetric;
	sov_score(bool s = true) : symmetric(s) {}

	struct segment {
		// [begin, begin+1, ..., end-2, end-1] are in the segment.
		size_t begin, end;
	};

	typedef std::vector<segment> segs_t;
	typedef std::array< segs_t, 3 > all_segs_t;
	typedef std::vector< std::pair<segment,segment> > segs_pair_t;
	typedef std::array< segs_pair_t, 3 > all_segs_pair_t;

	// split sequences into H/E/C-segments.
	void runs(const seq_it &data, size_t len, all_segs_t &out) const {
		auto prev = data[0].sec;
		size_t i_begin = 0;
		for(size_t i = 1 ; i < len ; i++) {
			const auto curr = data[i].sec;
			if(prev != curr) { // a segment just ended.
				out[static_cast<size_t>(prev)].push_back(segment{i_begin, i});
				i_begin = i;
				prev = curr;
			}
		}
		// last segment still open
		out[static_cast<size_t>(prev)].push_back(segment{i_begin, len});
	}

	// create lists of overlapping pairs and non-overlapped segments.
	void overlapping(const segs_t &as, const segs_t &bs,
	                 segs_pair_t &pairs, segs_t &a_single, segs_t &b_single) const {
		std::vector<bool> got_as(as.size(), false), got_bs(bs.size(), false);
		for(size_t ai = 0 ; ai < as.size() ; ai++) {
			const auto a = as[ai];
			for(size_t bi = 0 ; bi < bs.size() ; bi++) {
				const auto b = bs[bi];
				if(b.begin < a.end && b.end > a.begin) {
					pairs.emplace_back(a, b);
					got_as[ai] = got_bs[bi] = true;
				}
			}
		}
		for(size_t i = 0 ; i < as.size() ; i++) if(!got_as[i]) a_single.push_back(as[i]);
		for(size_t i = 0 ; i < bs.size() ; i++) if(!got_bs[i]) b_single.push_back(bs[i]);
	}

	// helpers for SOV
	inline size_t minov(const segment &a, const segment &b) const {
		return std::min(a.end, b.end) - std::max(a.begin, b.begin);
	}

	inline size_t maxov(const segment &a, const segment &b) const {
		return std::max(a.end, b.end) - std::min(a.begin, b.begin);
	}

	inline size_t len(const segment &a) const {
		return a.end - a.begin;
	}

	inline size_t delta(const segment &a, const segment &b) const {
		const size_t v0 = maxov(a, b) - minov(a, b),
		             v1 = minov(a, b),
		             v2 = len(a) / 2,
		             v3 = len(b) / 2;
		return std::min(v0, std::min(v1, std::min(v2, v3)));
	}

	// calculate SOV for one type of segment
	void sov_type(const segs_t &a, const segs_t &b, float &value, float &norm) const {
		segs_pair_t pairs; // pairs of overlapping segments
		segs_t a_single, b_single; // non-overlapping segments
		overlapping(a, b, pairs, a_single, b_single);
		// total SOV score
		for(const auto p : pairs) {
			const float n = minov(p.first, p.second) + delta(p.first, p.second);
			const float d = maxov(p.first, p.second);
			float l = len(p.first);
			if(symmetric) l += len(p.second);
			value += n / d * l;
		}
		// normalisation factor
		for(const auto p : pairs) {
			norm += len(p.first);
			if(symmetric) norm += len(p.second);
		}
		for(const auto s : a_single) norm += len(s);
		if(symmetric) for(const auto s : b_single) norm += len(s);
	}

	virtual float operator()(const seq_it &a_seq, const seq_it &b_seq, size_t len) const {
		using namespace std;
		// split string into segments
		all_segs_t a, b;
		runs(a_seq, len, a);
		runs(b_seq, len, b);

		// for each of H/E/C
		float value = 0, norm = 0;
		for(size_t i = 0 ; i < 3 ; i++)
			sov_type(a[i], b[i], value, norm);

		return value / norm;
	}
};


#endif
