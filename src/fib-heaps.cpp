#include <iostream>
#include <cstring>
#include <stack>

struct Node {
	int val;
	Node *parent;
	Node *child;

	// Circular list
	Node *left;
	Node *right;
	int degree;
	bool marked;
};

struct Heap {
	int rootSize;
	int markedNodes;
	Node *nodes;
	Node **consolidate;
	int consolidateSize;
	Node *min;
	std::stack<Node*> freeNodes;
};

// Creates a new heap, where the maximum number of nodes is n.
Heap *createHeap(int n) {
	Heap *h = new Heap();
	h->consolidateSize = n;
	h->rootSize = 0;
	h->markedNodes = 0;
	h->nodes = new Node[n];
	memset(h->nodes, 0, sizeof(Node)*n);
	h->consolidate = new Node*[h->consolidateSize];
	h->min = NULL;
	for (int i = n-1; i >= 0; i--) {
		h->freeNodes.push(&h->nodes[i]);
	}
	return h;
}

// Returns the index (i of nodes[i]) of the next node to be used.
int nextNodeIndex(Heap *h) {
	Node *next = h->freeNodes.top();
	return next - h->nodes;  
}

// Creates a new node with the new value v.
Node *createNode(Heap *h, int v) {
	Node* n = h->freeNodes.top();
	h->freeNodes.pop();
	n->val = v;
	return n;
}

// Returns the node in the heap with the lowest value.
int min(Heap *h) {
	return h->min->val;
}

// Inserts a node in the root list.
void insertRoot(Heap *h, Node *n) {
	n->parent = NULL;
	if (n->marked) {
		n->marked = false;
		h->markedNodes--;
	}
	if (h->min == NULL) {
		h->min = n;
		n->left = n;
		n->right = n;
	} else {
		// inserts node at the left side of the min node
		n->left = h->min->left;
		n->right = h->min;
		n->left->right = n;
		n->right->left = n;
		if (h->min->val > n->val) {
			h->min = n;
		}
	}
	h->rootSize++;
}

// Removes a node from the root list.
static void removeRoot(Heap *h, Node* n) {
	n->left->right = n->right;
	n->right->left = n->left;
	h->rootSize--;
}

// Inserts a node n in the child of node p.
static void insertChild(Heap *h, Node *n, Node *p) {
	n->parent = p;
	p->degree++;
	if (p->child == NULL) {
		p->child = n;
		n->left = n;
		n->right = n;
	} else {
		n->left = p->child->left;
		n->right = p->child;
		n->left->right = n;
		n->right->left = n;
	}
}

// Removes node n from its parent.
static void removeChild(Heap *h, Node *n) {
	Node *p = n->parent;
	if (n == n->right) {
		p->child = NULL;
	} else {
		n->left->right = n->right;
		n->right->left = n->left;
		if (p->child == n) {
			p->child = n->right;
		}
	}			
	p->degree--;
	n->parent = NULL;
}

// Assembles the heap by connecting nodes and creating a new root list.
static void consolidate(Heap *h) {
	memset(h->consolidate, 0, sizeof(Node*) * h->consolidateSize);

	// Traverses the root list, starting at the min node and going in the right 
	// direction. Despite the changes in the root list, these only affect 
	// previous traversed nodes and not to-be-traversed nodes.
	Node *w = h->min;
	int rootSize = h->rootSize;
	for (int i = 0; i < rootSize; i++) {
		Node *next = w->right;
		Node *x = w;
		int d = x->degree;
		while (h->consolidate[d] != NULL) {
			Node *y = h->consolidate[d];
			if (x->val > y->val) {
				Node *tmp = x;
				x = y;
				y = tmp;
			}
			removeRoot(h, y);
			insertChild(h, y, x);
			h->consolidate[d] = NULL;
			d++;
		}
		h->consolidate[d] = x;
		w = next;
	}

	// create new root list
	h->min = NULL;
	h->rootSize = 0;
	for (int i = 0; i < h->consolidateSize; i++) {
		Node *n = h->consolidate[i];
		if (n != NULL) {
			insertRoot(h, n);
		}
	}
}

// Removes the node with the lowest value from the heap.
void extractMin(Heap *h) {
	Node* z = h->min;
	if (z != NULL) {
		// insert each child node in the root list
		Node* c = z->child;
		if (c != NULL) {
			do {
				Node *next = c->right;
				insertRoot(h, c);
				c = next;
			} while (c != z->child);
			z->child = NULL;
		}

		removeRoot(h, z);
		h->freeNodes.push(z);

		if (z == z->right) {
			h->min = NULL; 
		} else {
			h->min = z->right;
			consolidate(h);
		}
	}
}

// Moves a node from the "tree" to the root list.
static void cut(Heap *h, Node *n) {
	removeChild(h, n);
	insertRoot(h, n);
}

// Marks and/or removes from the tree parent nodes of n except the root node.
static void cascading_cut(Heap *h, Node *n) {
	Node *z = n->parent;
	if (z != NULL) {
		if (!n->marked) {
			n->marked = true;
			h->markedNodes++;
		} else {
			cut(h, n);
			cascading_cut(h, z);
		}
	}
}

// Decreases the value to v of the node with index i.
void decrease(Heap *h, int i, int v) {
	Node *n = &h->nodes[i];
	n->val = v;

	Node *p = n->parent;
	if (p != NULL && n->val < p->val) {
		cut(h, n);
		cascading_cut(h, p);
	} else if (h->min->val > n->val) {
		h->min = n;
	}
}

// Returns the potential of the heap, according to the formula: 
// P(H) = R(H) + 2*M(H), where R(H) is the number of nodes in the root list and
// M(H) is the number of marked nodes in the heap.
int potential(Heap *h) {
	return h->rootSize + 2*h->markedNodes;
}

// Returns the number of child of the node nodes[i].
int rank(Heap *h, int i) {
	return h->nodes[i].degree;
}

// Deallocates all the memory previously manually allocated.
void cleanup(Heap *h) {
	delete[] h->nodes;
	delete[] h->consolidate;
	delete h;
}

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	Heap *h = NULL;
	while (true) {
		char cmd;
		std::cin >> cmd;

		if (cmd == 'B') {
			int n;
			std::cin >> n;
			h = createHeap(n);
		} else if (cmd == 'G') {
			std::cout << min(h) << std::endl;
		} else if (cmd == 'X') {
			extractMin(h);
		} else if (cmd == 'I') {
			int idx = nextNodeIndex(h);
			int v;
			std::cin >> v;
			insertRoot(h, createNode(h, v));
			std::cout << idx << std::endl;
		} else if (cmd == 'D') {
			int i, v;
			std::cin >> i >> v;
			decrease(h, i, v);
		} else if (cmd == 'P') {
			std::cout << potential(h) << std::endl;
		} else if (cmd == 'R') {
			int i;
			std::cin >> i;
			std::cout << rank(h, i) << std::endl;
		} else if (cmd == 'E') {
			cleanup(h);
			h = NULL;
			break;
		}
	}

	return 0;
}