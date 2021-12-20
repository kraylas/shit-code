#pragma once

#include "utils.hpp"
#include <utility>
namespace plain_bst {
template <typename K, typename V>
struct tree_node {
	using node_ptr = tree_node *;
	K k;
	V v;
	node_ptr fa, lch, rch;
	template <typename KT, typename VT>
	requires KUtils::is_rmcvref_same_v<K, KT> && KUtils::is_rmcvref_same_v<V, VT>
	tree_node(KT &&k, VT &&v) :
		k{std::forward<KT>(k)}, v{std::forward<VT>(v)}, fa{nullptr}, lch{nullptr}, rch{nullptr} {
	}
};

template <typename K, typename V>
struct tree {
	using node = tree_node<K, V>;
	using node_ptr = node *;
	node_ptr root{nullptr};
	tree() {
	}
	template <typename KT, typename VT>
	requires KUtils::is_rmcvref_same_v<K, KT> && KUtils::is_rmcvref_same_v<V, VT>
	void insert(KT &&k, VT &&v) {
		node_ptr p = new node(std::forward<KT>(k), std::forward<VT>(v));
		if (root == nullptr) {
			root = p;
			return;
		}
		node_ptr t = root;
		while (1) {
			if (p->k == t->k) {
				t->v = std::move(p->v);
				return;
			}
			if (p->k < t->k) {
				if (t->lch == nullptr) {
					t->lch = p;
					p->fa = t;
					return;
				}
				t = t->lch;
			} else {
				if (t->rch == nullptr) {
					t->rch = p;
					p->fa = t;
					return;
				}
				t = t->rch;
			}
		}
	}
	node_ptr find(const K &k) {
		node_ptr t = root;
		while (t && t->k != k) {
			if (k < t->k)
				t = t->lch;
			else
				t = t->rch;
		}
		return t;
	}
	void remove(const K &k) {
		node_ptr t = find(k), p, z, b, g;
		if (t == nullptr) return;
		if (t->lch && t->rch) {
			p = t->rch;
			z = t->fa;
			while (p->lch) p = p->lch;
			if (z) {
				if (z->lch == t) {
					z->lch = p;
				} else {
					z->rch = p;
				}
			} else {
				root = p;
			}
			g = p->rch;
			b = p->fa;
			if (b != t) {
				if (g) g->fa = b;
				b->lch = g;
				p->rch = t->rch;
				t->rch->fa = p;
			}
			p->fa = z;
			p->lch = t->lch;
			t->lch->fa = p;
			delete t;
			return;
		}
		p = [t] {if(t->lch) return t->lch; else return t->rch; }();
		z = t->fa;
		if (p) p->fa = z;
		if (z) {
			if (z->lch == t)
				z->lch = p;
			else
				z->rch = p;
		} else {
			root = p;
		}
		delete t;
	}
	static void destroyTree(node_ptr x) {
		if (x == nullptr) return;
		destroyTree(x->lch);
		destroyTree(x->rch);
		delete x;
		x = nullptr;
	}
	~tree() {
		destroyTree(root);
	}
};
} // namespace plain_bst
