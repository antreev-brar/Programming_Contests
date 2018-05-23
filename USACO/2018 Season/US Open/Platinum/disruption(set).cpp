#include <bits/stdc++.h>

#define SQ(a) (a)*(a)

#define F0R(i, a) for(int i = 0; i < (a); i++)
#define FOR(i, a, b) for(int i = (a); i < (b); i++)
#define R0F(i, a) for(int i = (a) - 1; i >= 0; i--)
#define ROF(i, a, b) for(int i = (b) - 1; i >= (a); i--)

#define ran() (rand() & 0x7FFF)
#define rand31() ((ran() << 16) | (ran() << 1) | (ran() & 1))
#define rand32() ((ran() << 17) | (ran() << 2) | (ran() & 3))
#define rand63() (((ll)ran() << 48) | ((ll)ran() << 33) | ((ll)ran() << 18) | ((ll)ran() << 3) | ((ll)ran() & 7))
#define rand64() (((ll)ran() << 49) | ((ll)ran() << 34) | ((ll)ran() << 19) | ((ll)ran() << 4) | ((ll)ran() & 15))

#define F first
#define S second
#define PB push_back
#define MP make_pair
#define MT make_tuple
#define UB upper_bound
#define LB lower_bound
#define X real()
#define Y imag()
#define MAXN 50000

using namespace std;

typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;
typedef pair<int, int> pii;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<ull> vul;
typedef complex<ld> point;

int n, m, res[MAXN - 1];
vector<pii> connections[MAXN], tokens[MAXN];

void update(set<pii>* s, pii p) {
	if(s->count(p))
		s->erase(p);
	else
		s->insert(p);
}

set<pii>* dfs(int node, int par) {
	set<pii>* ret = new set<pii>();
	vector<set<pii>*> v;
	for(const pii edge : connections[node])
		if(edge.F != par) {
			v.PB(dfs(edge.F, node));
			if(v.back()->size() > 0) res[edge.S] = (*(v.back()->begin())).F;
			else res[edge.S] = -1;
			if(ret == NULL) {
				ret = v.back();
				v.pop_back();
			} else if(v.back()->size() > ret->size())
				swap(ret, v.back());
		}
	for(const set<pii> *s : v)
		for(const pii p : *s)
			update(ret, p);
	for(const pii p : tokens[node])
		update(ret, p);
	return ret;
}

int main() {
	ifstream fin("disrupt.in");
	ofstream fout("disrupt.out");
	fin >> n >> m;
	F0R(i, n - 1) {
		int u, v;
		fin >> u >> v;
		u--, v--;
		connections[u].PB(MP(v, i));
		connections[v].PB(MP(u, i));
	}
	F0R(i, m) {
		int u, v, d;
		fin >> u >> v >> d;
		u--, v--;
		tokens[u].PB(MP(d, i));
		tokens[v].PB(MP(d, i));
	}
	dfs(0, -1);
	F0R(i, n - 1) fout << res[i] << "\n";
}
