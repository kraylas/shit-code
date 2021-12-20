#pragma once
#include "utils.hpp"
#include <cstdint>
#include <utility>
#include <algorithm>
namespace AVL {
template <typename KeyType, typename ValueType>
struct AVLNode {
	using ptr_AVLNode = AVLNode<KeyType, ValueType> *;
	KeyType key;
	ValueType value;
	ptr_AVLNode lch, rch;
	std::int32_t height;
	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	AVLNode(K &&key, V &&value) :
		key{std::forward<K>(key)},
		value{std::forward<V>(value)},
		lch{nullptr},
		rch{nullptr},
		height{1} {
	}
};

template <typename KeyType, typename ValueType, typename CompType = std::less<KeyType>>
requires KUtils::Ops<KeyType, CompType>
class AVLTreeMap {
	using Node = AVLNode<KeyType, ValueType>;
	using AVLNodePtr = Node *;
	AVLNodePtr root;
	static std::int32_t height(AVLNodePtr p) {
		if (p == nullptr) return 0;
		return p->height;
	}
#define fwdk std::forward<K>(key)
#define fwdv std::forward<V>(value)
	static AVLNodePtr rightRotation(AVLNodePtr o) {
		AVLNodePtr newo = o->lch;
		o->lch = newo->rch;
		newo->rch = o;
		o->height = 1 + std::max(height(o->lch), height(o->rch));
		newo->height = 1 + std::max(height(newo->lch), height(newo->rch));
		return newo;
	}

	static AVLNodePtr leftRotation(AVLNodePtr o) {
		AVLNodePtr newo = o->rch;
		o->rch = newo->lch;
		newo->lch = o;
		o->height = 1 + std::max(height(o->lch), height(o->rch));
		newo->height = 1 + std::max(height(newo->lch), height(newo->rch));
		return newo;
	}
	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	static AVLNodePtr insertUtil(AVLNodePtr o, K &&key, V &&value) {
		if (o == nullptr) {
			AVLNodePtr temp = new Node(fwdk, fwdv);
			return temp;
		}
		if (CompType{}(key, o->key))
			o->lch = insertUtil(o->lch, fwdk, fwdv);
		else if (CompType{}(o->key, key))
			o->rch = insertUtil(o->rch, fwdk, fwdv);
		else {
			o->value = fwdv;
			return o;
		}
		o->height = 1 + std::max(height(o->lch), height(o->rch));
		std::int32_t bal = height(o->lch) - height(o->rch);
		if (bal > 1) {
			if (CompType{}(key, o->lch->key)) {
				return rightRotation(o);
			} else {
				o->lch = leftRotation(o->lch);
				return rightRotation(o);
			}
		} else if (bal < -1) {
			if (CompType{}(o->rch->key, key)) {
				return leftRotation(o);
			} else {
				o->rch = rightRotation(o->rch);
				return leftRotation(o);
			}
		}
		return o;
	}
	static AVLNodePtr removeUtil(AVLNodePtr o, const KeyType &x) {
		if (o == nullptr)
			return nullptr;
		if (CompType{}(x, o->key)) {
			o->lch = removeUtil(o->lch, x);
		} else if (CompType{}(o->key, x)) {
			o->rch = removeUtil(o->rch, x);
		} else {
			AVLNodePtr r = o->rch;
			if (o->rch == nullptr) {
				AVLNodePtr l = o->lch;
				delete o;
				o = l;
			} else if (o->lch == nullptr) {
				delete o;
				o = r;
			} else {
				while (r->lch != nullptr)
					r = r->lch;
				o->key = r->key;
				o->value = std::move(r->value);
				o->rch = removeUtil(o->rch, r->key);
			}
		}
		if (o == nullptr)
			return o;
		o->height = 1 + std::max(height(o->lch), height(o->rch));
		int bal = height(o->lch) - height(o->rch);
		if (bal > 1) {
			if (height(o->lch) >= height(o->rch)) {
				return rightRotation(o);
			} else {
				o->lch = leftRotation(o->lch);
				return rightRotation(o);
			}
		} else if (bal < -1) {
			if (height(o->rch) >= height(o->lch)) {
				return leftRotation(o);
			} else {
				o->rch = rightRotation(o->rch);
				return leftRotation(o);
			}
		}
		return o;
	}
	static void destroyTree(AVLNodePtr p) {
		if (p == nullptr) return;
		destroyTree(p->lch);
		destroyTree(p->rch);
		delete p;
	}
	static AVLNodePtr searchUtil(AVLNodePtr o, const KeyType &x) {
		if (o == NULL)
			return NULL;
		const KeyType &k = o->value;
		if (CompType{}(k, x))
			return searchUtil(o->rch, x);
		else if (CompType{}(x, k))
			return searchUtil(o->lch, x);
		else
			return o;
	}

public:
	AVLTreeMap() :
		root{nullptr} {
	}
	~AVLTreeMap() {
		destroyTree(root);
	}

	template <typename K, typename V>
	requires KUtils::is_rmcvref_same_v<K, KeyType> && KUtils::is_rmcvref_same_v<V, ValueType>
	void insert(K &&key, V &&value) {
		root = insertUtil(root, fwdk, fwdv);
	}
	void remove(const KeyType &key) {
		root = removeUtil(root, key);
	}
	ValueType *get(const KeyType &key) {
		auto p = searchUtil(root, key);
		return p ? &p->value : nullptr;
	}
#undef fwdk
#undef fwdv
};
} // namespace AVL
