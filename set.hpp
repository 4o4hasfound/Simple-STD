#ifndef SSTD_SET_INCLUDED
#define SSTD_SET_INCLUDED

#include "core.hpp"

SSTD_BEGIN

template<typename T>
class set {
private:
	enum _Node_Color {
		Red,
		Black
	};
	struct Node {
		Node *left, *right, *parent;
		T elt;
		_Node_Color color;
	};
	struct Tree {
		Node *root, * neel;
	};
public:

private:
	Node* m_root, m_neel;
};

SSTD_END

#endif