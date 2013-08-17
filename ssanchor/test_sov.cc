#include "sov.hh"
#include "formats.hh"
#include "score.hh"

using namespace std;

sequence str_to_seq(string a) {
	sequence s;
	for(const char c : a) s.emplace_back(residue::Xaa, char_to_sec(c));
	return s;
}

void test(string a, string b) {
	sequence sa = str_to_seq(a), sb = str_to_seq(b);
	const size_t len = sa.size();
	sov_score sym(true), asy(false);
	float s_ab = sym(sa.begin(), sb.begin(), len),
	      s_ba = sym(sb.begin(), sa.begin(), len),
	      a_ab = asy(sa.begin(), sb.begin(), len),
	      a_ba = asy(sb.begin(), sa.begin(), len),
	      q3 = q3_score()(sa.begin(), sb.begin(), len);
	std::cout <<
		a << "-" << b << ": "
		<< " " << a_ab
		<< ":" << a_ba
		<< ", " << s_ab
		<< ":" << s_ba
		<< " ~ " << (a_ab + a_ba) / 2
		<< "  q3=" << q3
		<< endl;
	
}

int main(int, char**) {
	const string obs = "CHHHHHHHHHHC";
	const vector<string> pred{{obs, "CHCHCHCHCHCC", "CCCHHHHHCCCC", "CHHHCHHHCHHC", "CHHCCHHHHHCC", "CCCHHHHHHCCC"}};
	for(string p : pred) test(obs, p);
}
