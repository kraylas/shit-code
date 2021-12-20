#pragma once

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <type_traits>
#include <utility>
#include <iostream>
#include "utils.hpp"
namespace RBT {
enum class Color : std::uint8_t { RED,
								  BLACK };
template <typename KeyType, typename ValueType>
struct RBTNode {
	using ptr_RBTNode = RBTNode<KeyType, ValueType> *;
	KeyType key;
	ValueType value;
	ptr_RBTNode ch[2], fa;
	Color col{Color::RED};
	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	RBTNode(K &&key, V &&value) :
		fa{nullptr},
		key{std::forward<K>(key)},
		value{std::forward<V>(value)} {
		ch[0] = ch[1] = nullptr;
	}
	bool d() {
		return fa->ch[1] == this;
	}
	// void up() {
	// std::cout << "up" << std::endl;
	//}
};
template <typename KeyType, typename ValueType, typename CompType = std::less<KeyType>>
requires KUtils::Ops<KeyType, CompType>
class RBTreeMap {
public:
	using Node = RBTNode<KeyType, ValueType>;
	using ptr_Node = Node *;

	ptr_Node root{nullptr};
	void rot(ptr_Node x, bool d) {
		ptr_Node y = x->ch[!d], z = x->fa;
		x->ch[!d] = y->ch[d];
		if (y->ch[d] != NULL) y->ch[d]->fa = x;
		y->fa = z;
		if (z == NULL)
			root = y;
		else
			z->ch[x->d()] = y;
		y->ch[d] = x;
		x->fa = y;
		if constexpr (requires { x->up(); }) {
			x->up();
			y->up();
		}
	}
	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	static ptr_Node getNode(K &&key, V &&value) {
		return new Node(std::forward<K>(key), std::forward<V>(value));
	}
	static void destroyNode(ptr_Node p) {
		// std::cout << "destroy :{" << p->key << ", " << p->value << "}" << std::endl;
		delete p;
	}
	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	void insert(K &&key, V &&value) {
		if (root == nullptr) {
			root = getNode(std::forward<K>(key), std::forward<V>(value));
			root->col = Color::BLACK;
			return;
		}
		ptr_Node t = root, p = getNode(std::forward<K>(key), std::forward<V>(value)), z = nullptr;
		bool d;
		while (!eq(t->key, p->key) && t->ch[d = CompType{}(t->key, p->key)]) t = t->ch[d];
		if (eq(t->key, p->key)) {
			t->value = std::move(p->value);
			return;
		}
		p->fa = t;
		t->ch[d] = p;
		if constexpr (requires { t->up(); }) {
			t->up();
		}
		while ((t = p->fa) && (t->col == Color::RED)) {
			z = t->fa;
			bool d1 = t->d(), d2 = p->d();
			ptr_Node u = z->ch[!d1];
			if (u && u->col == Color::RED) {
				u->col = Color::BLACK;
				t->col = Color::BLACK;
				z->col = Color::RED;
				p = z;
				continue;
			}
			if (d1 ^ d2) {
				rot(t, d1);
				std::swap(p, t);
			}
			t->col = Color::BLACK;
			z->col = Color::RED;
			rot(z, !d1);
		}
		root->col = Color::BLACK;
	}
	static bool eq(const KeyType &l, const KeyType &r) {
		return (!CompType{}(l, r)) && (!CompType{}(r, l));
	}
	ptr_Node search(const KeyType &key) {
		ptr_Node t = root;
		while (t && !eq(key, t->key))
			t = t->ch[CompType{}(t->key, key)];
		return t;
	}
	ValueType *get(const KeyType &key) {
		auto p = search(key);
		return p ? &p->value : nullptr;
	}
	void fixup(ptr_Node t, ptr_Node z) {
		ptr_Node b;
		while ((!t || t->col == Color::BLACK) && t != root) {
			int d = z->ch[1] == t;
			b = z->ch[!d];
			if (b->col == Color::RED) {
				b->col = Color::BLACK;
				z->col = Color::RED;
				rot(z, d);
				b = z->ch[!d];
			}
			if ((b->ch[0] == NULL || b->ch[0]->col == Color::BLACK) && (b->ch[1] == NULL || b->ch[1]->col == Color::BLACK)) {
				b->col = Color::RED;
				t = z;
				z = t->fa;
			} else {
				if (!b->ch[!d] || b->ch[!d]->col == Color::BLACK) {
					b->ch[d]->col = Color::BLACK;
					b->col = Color::RED;
					rot(b, !d);
					b = z->ch[!d];
				}
				b->col = z->col;
				z->col = Color::BLACK;
				b->ch[!d]->col = Color::BLACK;
				rot(z, d);
				t = root;
				break;
			}
		}
		if (t) t->col = Color::BLACK;
	}
	void remove(const KeyType &key) {
		ptr_Node t = root, p, z, b, g;
		Color Tmp;
		if ((t = search(key)) == nullptr) {
			// std::cout << "rm :" << key << " Unsuccess" << std::endl;
			return;
		}
		if (t->ch[0] && t->ch[1]) {
			p = t->ch[1], z = t->fa;
			while (p->ch[0] != NULL) p = p->ch[0];
			if (z != NULL)
				z->ch[t->d()] = p;
			else
				root = p;
			g = p->ch[1];
			b = p->fa;
			Tmp = p->col;
			if (b == t)
				b = p;
			else {
				if (g) g->fa = b;
				b->ch[0] = g;
				p->ch[1] = t->ch[1];
				t->ch[1]->fa = p;
				if constexpr (requires { b->up(); }) {
					b->up();
				}
			}
			p->fa = z;
			p->col = t->col;
			p->ch[0] = t->ch[0];
			t->ch[0]->fa = p;
			if (Tmp == Color::BLACK) fixup(g, b);
			destroyNode(t);
			return;
		}
		p = t->ch[t->ch[1] != nullptr];
		z = t->fa;
		Tmp = t->col;
		if (p) p->fa = z;
		if (z)
			z->ch[t->d()] = p;
		else
			root = p;
		if (Tmp == Color::BLACK) fixup(p, z);
		destroyNode(t);
	}
	static void destroyTree(ptr_Node p) {
		if (p == nullptr) return;
		if (p->ch[0]) destroyTree(p->ch[0]);
		if (p->ch[1]) destroyTree(p->ch[1]);
		destroyNode(p);
	}
	//	void dfs(ptr_Node x, int dep) {
	//		if (x == nullptr) return;
	//		dfs(x->ch[0], dep + 1);
	//		// std::cout << "dep:" << dep << " {" << x->key << ", " << x->value << "} ";
	//		dfs(x->ch[1], dep + 1);
	//	}
	//	void travel() {
	//		dfs(root, 1);
	//		std::cout << std::endl;
	//	}
	~RBTreeMap() {
		destroyTree(root);
	}
};
} // namespace RBT
