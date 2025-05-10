#include <bits/stdc++.h>
constexpr int N = 2e5 + 10;
using u32 = unsigned int;
int n, q, rt[N], a[N];
namespace treap {
	constexpr int V = 3 * N;
	int ls[V], rs[V];
	u32 rnd[V];
	void init() {
		std::mt19937 hua(1);
		for(int i = 1; i <= 3 * n; i ++) {
			rnd[i] = hua();
		}
	}
	int merge(int u, int v) {
		if(!u || !v) return u + v;
		if(rnd[u] < rnd[v]) {
			rs[u] = merge(rs[u], v); return u;
		} else {
			ls[v] = merge(u, ls[v]); return v;
		}
	}
	void split(int u, int &x, int &y, int k) {
		if(!u) return void(x = y = 0);
		if(u > n && u <= 2 * n || u <= k) x = u, split(rs[u], rs[x], y, k);
		else y = u, split(ls[u], x, ls[y], k); 
	}
}
int ans[N];
namespace ds {
	constexpr int V = 8e6 + 10;
	struct oper {
		int o, x, y, k;
	}a[V];
	constexpr int B = 5;
	struct bit {
		int c[N], last[N], ts;
		int up[N], down[N];
		void init() {
			for(int i = 1; i <= n; i ++) {
				if(i % B == 0) up[i] = up[i / B] * B, down[i] = down[i / B] * B;
				else up[i] = (i + B - 1) / B * B, down[i] = i - 1;
			}
		}
		void upd(int p, int w) {
			for(; p <= n; p = up[p]) {
				if(last[p] != ts) last[p] = ts, c[p] = 0;
				c[p] += w;
			}
		}
		int qry(int p) {
			int ans = 0;
			for(; p; p = down[p]) ans += last[p] == ts ? c[p] : 0;
			return ans;
		}
		void clear() {
			ts ++;
		}
	}ds;
	int cnt;
	void update(int x, int y, int w) {
		if(x == 0 || y == n + 1) return ;
 		a[++ cnt] = {0, n - x + 1, y, w};
		assert(cnt < V - 10);
	}
	void query(int x, int y, int id) {
		ans[id] = y - x + 1;
		a[++ cnt] = {1, n - x + 1, y, id};
		assert(cnt < V - 10);
	}
	void solve(int l, int r) {
		if(l == r) return ;
		int mid = l + r >> 1;
		solve(l, mid), solve(mid + 1, r);
		ds.clear();
 		for(int i = l, j = mid + 1; j <= r; j ++) if(a[j].o) {
			while(i <= mid && a[i].x <= a[j].x) {
				if(!a[i].o) ds.upd(a[i].y, a[i].k);
				i ++;
			}
			ans[a[j].k] -= ds.qry(a[j].y);
		}
		std::inplace_merge(a + l, a + mid + 1, a + r + 1, [&] (const oper &x, const oper &y) {return x.x < y.x;});
	}
	void work() {
		ds.init();
		ds.clear();
		if(cnt) solve(1, cnt);
	}
}
int min(int u) {
	assert(u);
	while(treap::ls[u]) u = treap::ls[u];
	if(u > n && u <= 2 * n ) return 0;
	else if(u > 2 * n) return n + 1;
	else return u;
}
int max(int u) {
	assert(u);
	while(treap::rs[u]) u = treap::rs[u];
	if(u > n && u <= 2 * n ) return 0;
	else if(u > 2 * n) return n + 1;
	else return u;
}
void debug(int u) {
	return ;
	std::function<void(int)> dfs = [&] (int u) {
		if(!u) return ;
		dfs(treap::ls[u]);
		std::cout << u << ' ';
		dfs(treap::rs[u]);
	};
	std::cout << u << ":" ;
	dfs(rt[u]);
	std::cout << std::endl;
}
int main() {
	// freopen("in.txt", "r", stdin);
	std::ios::sync_with_stdio(false), std::cin.tie(0);
	std::cin >> n >> q;
	treap::init();
	for(int i = 1; i <= n; i ++) {
		std::cin >> a[i];
		if(rt[a[i]]) ds::update(max(rt[a[i]]), i, 1);
		rt[a[i]] = treap::merge(rt[a[i]], i);
	}
	for(int i = 1; i <= n; i ++) {
		rt[i] = treap::merge(n + i, rt[i]);
		rt[i] = treap::merge(rt[i], 2 * n + i);
	}
	int tot = 0;
	for(int i = 0; i < q; i ++) {
		int o;
		std::cin >> o;
		if(o == 1) {
			int l, r, x, y;
			std::cin >> l >> r >> x >> y;
			if(x == y) continue;
			int AL, A, AR;
			int BL, B, BR;
			treap::split(rt[x], AL, A, l - 1);
			treap::split(A, A, AR, r);
			treap::split(rt[y], BL, B, l - 1);
			treap::split(B, B, BR, r);
			if(!A && !B) {
				rt[x] = treap::merge(AL, AR);
				rt[y] = treap::merge(BL, BR);
				continue;
			}
			int C = 0;
			int minA = A ? min(A) : min(AR), minB = B ? min(B) : min(BR);
			if(A) {
				ds::update(max(AL), minA, -1);
				ds::update(max(AL), min(AR), 1);
			}
			if(minA < minB) {
				ds::update(max(BL), minB, -1);
				ds::update(max(BL), minA, 1);
			}
			while(A && B) {
				if(minA < minB) { 
					int a, b;
					treap::split(A, a, b, minB);
					assert(a);
					minA = b ? min(b) : min(AR);
					ds::update(max(a), minA, -1);
					ds::update(max(a), minB, 1);
					C = treap::merge(C, a);
					A = b;
				} else {
					int a, b;
					treap::split(B, a, b, minA);
					assert(a);
					minB = b ? min(b) : min(BR);
					ds::update(max(a), minB, -1);
					ds::update(max(a), minA, 1);
					C = treap::merge(C, a);
					B = b;
				}
			}
			if(A) {
				ds::update(max(A), min(AR), -1);
				ds::update(max(A), minB, 1);
				C = treap::merge(C, A);
			}
			if(B) {
				C = treap::merge(C, B);
			}
			rt[x] = treap::merge(AL, AR);
			rt[y] = treap::merge(BL, treap::merge(C, BR));
			debug(x);
			debug(y);
		} else {
			int l, r;
			std::cin >> l >> r;
			ds::query(l, r, ++ tot);
		}
	}
	ds::work();
	for(int i = 1; i <= tot; i ++) {
		std::cout << ans[i] << '\n';
	}
	std::cerr << 1.0 * clock() / CLOCKS_PER_SEC << std::endl;
	return 0;
}