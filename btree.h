#ifndef BTree_H
#define BTree_H
#include <iostream>
#include <vector>
#include <sstream>

#include "node.h"

using namespace std;

template <typename TK>
class BTree {
 private:
  Node<TK>* root;
  int M;
  int n;

  void toStringNode(const Node<TK>* nodoActual, string& resultado, const string& separador, bool& esPrimero) const {
    if (!nodoActual) return;
    if (nodoActual->leaf) {
      for (int indiceClave = 0; indiceClave < nodoActual->count; ++indiceClave) {
        if (!esPrimero) resultado += separador;
        resultado += to_string(nodoActual->keys[indiceClave]);
        esPrimero = false;
      }
    } else {
      for (int indiceClave = 0; indiceClave < nodoActual->count; ++indiceClave) {
        toStringNode(nodoActual->children[indiceClave], resultado, separador, esPrimero);
        if (!esPrimero) resultado += separador;
        resultado += to_string(nodoActual->keys[indiceClave]);
        esPrimero = false;
      }
      toStringNode(nodoActual->children[nodoActual->count], resultado, separador, esPrimero);
    }
  }

  bool checkNodo(Node<TK>* node, int nivel, int minKeys, int& leafLevel) {
    if (!node) return true;

    if (node == root) {
      if (node->count < 1 || node->count > M - 1) {
        return false;
      }
    } else {
      if (node->count < 0 || node->count > M - 1) {
        return false;
      }
      if (node->count > 0 && node->count < minKeys && nivel > 1) {
        return false;
      }
    }

    for (int i = 1; i < node->count; ++i) {
      if (node->keys[i - 1] >= node->keys[i]) {
        return false;
      }
    }

    if (node->leaf) {
      if (leafLevel == -1)
        leafLevel = nivel;
      else if (leafLevel != nivel) {
        return false;
      }
      return true;
    }

    for (int i = 0; i <= node->count; ++i) {
      if (node->children[i] == nullptr) {
        return false;
      }
    }

    if (node->count > 0) {
      for (int i = 0; i < node->count; ++i) {
        Node<TK>* left = node->children[i];
        Node<TK>* right = node->children[i + 1];
        if (left->count > 0 && left->keys[left->count - 1] >= node->keys[i]) {
          return false;
        }
        if (right->count > 0 && right->keys[0] < node->keys[i]) {
          return false;
        }
      }
    }

    for (int i = 0; i <= node->count; ++i) {
      if (!checkNodo(node->children[i], nivel + 1, minKeys, leafLevel)) {
        return false;
      }
    }

    return true;
  }

  void clearNode(Node<TK>* node) {
    if (!node) return;
    if (!node->leaf) {
      for (int i = 0; i <= node->count; ++i) {
        if (node->children[i] != nullptr) {
          clearNode(node->children[i]);
          node->children[i] = nullptr;
        }
      }
    }
    delete node;
  }

 public:
  BTree(int _M) : root(nullptr), M(_M), n(0) {}

  bool search(TK key){
    const Node<TK>* x = root;
    while (x != nullptr) {
      int i = 0;
      while (i < x->count && key > x->keys[i]) i++;
      if (i < x->count && key == x->keys[i]) return true;
      if (x->leaf) return false;
      x = x->children[i];
    }
    return false;
  }

  void insert(TK key){
    if (search(key)) return;

    if (root == nullptr) {
      root = new Node<TK>(M);
      root->leaf = true;
      root->keys[0] = key;
      root->count = 1;
      n = 1;
      return;
    }

    const int t = (M + 1) / 2;

    if (root->count == M - 1) {
      Node<TK>* s = new Node<TK>(M);
      s->leaf = false;
      s->count = 0;
      s->children[0] = root;

      Node<TK>* y = root;
      Node<TK>* z = new Node<TK>(M);
      z->leaf = y->leaf;

      int mid = y->count / 2;
      TK claveMedia = y->keys[mid];

      int rightCount = y->count - mid - 1;
      for (int j = 0; j < rightCount; ++j)
        z->keys[j] = y->keys[mid + 1 + j];

      if (!y->leaf) {
        for (int j = 0; j <= rightCount; ++j)
          z->children[j] = y->children[mid + 1 + j];
        for (int j = mid + 1; j < M; ++j)
          y->children[j] = nullptr;
      }

      z->count = rightCount;
      y->count = mid;

      s->children[1] = z;
      s->keys[0] = claveMedia;
      s->count = 1;

      root = s;
    }

    Node<TK>* cur = root;
    while (!cur->leaf) {
      int i = 0;
      while (i < cur->count && key > cur->keys[i]) i++;

      Node<TK>* child = cur->children[i];

      if (child->count == M - 1) {
        Node<TK>* y = child;
        Node<TK>* z = new Node<TK>(M);
        z->leaf = y->leaf;

        int mid = y->count / 2;
        TK claveMedia = y->keys[mid];

        int rightCount = y->count - mid - 1;
        for (int j = 0; j < rightCount; ++j)
          z->keys[j] = y->keys[mid + 1 + j];

        if (!y->leaf) {
          for (int j = 0; j <= rightCount; ++j)
            z->children[j] = y->children[mid + 1 + j];
          for (int j = mid + 1; j < M; ++j)
            y->children[j] = nullptr;
        }

        z->count = rightCount;
        y->count = mid;

        for (int j = cur->count; j >= i + 1; --j)
          cur->children[j + 1] = cur->children[j];
        cur->children[i + 1] = z;

        for (int j = cur->count - 1; j >= i; --j)
          cur->keys[j + 1] = cur->keys[j];

        cur->keys[i] = claveMedia;
        cur->count++;

        if (key > claveMedia) {
          i++;
        }
        child = cur->children[i];
      }
      cur = child;
    }

    int i = cur->count - 1;
    while (i >= 0 && key < cur->keys[i]) {
      cur->keys[i + 1] = cur->keys[i];
      --i;
    }
    cur->keys[i + 1] = key;
    cur->count++;

    ++n;
  }

  void remove(TK key){
    return;
  }

  int height() const {
    if (root == nullptr) return 0;
    int levels = 0;
    const Node<TK>* cur = root;
    while (cur) {
      ++levels;
      if (cur->leaf) break;
      cur = cur->children[0];
    }
    return (levels > 0) ? (levels - 1) : 0;
  }

  string toString(const string& sep) const {
    if (root == nullptr) return string();
    string resultado;
    bool esPrimero = true;
    toStringNode(root, resultado, sep, esPrimero);
    return resultado;
  }

  vector<TK> rangeSearch(TK begin, TK end);

  TK minKey(){
    if (root == nullptr || root->count == 0) return TK{};
    const Node<TK>* x = root;
    while (!x->leaf) x = x->children[0];
    return x->keys[0];
  }

  TK maxKey(){
    if (root == nullptr) return TK{};

    const Node<TK>* cur = root;
    while (cur && !cur->leaf) {
      int idx = cur->count;
      while (idx >= 0 && cur->children[idx] == nullptr) --idx;
      if (idx < 0) break;
      cur = cur->children[idx];
    }
    if (cur && cur->count > 0) return cur->keys[cur->count - 1];

    bool found = false;
    TK best = TK();
    std::vector<const Node<TK>*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
      const Node<TK>* node = stack.back();
      stack.pop_back();
      if (!node) continue;
      for (int i = 0; i < node->count; ++i) {
        if (!found) { best = node->keys[i]; found = true; }
        else if (node->keys[i] > best) best = node->keys[i];
      }
      if (!node->leaf) {
        for (int i = 0; i <= node->count; ++i) {
          if (node->children[i]) stack.push_back(node->children[i]);
        }
      }
    }
    return best;
  }

  void clear() {
    clearNode(root);
    root = nullptr;
    n = 0;
  }

  int size() {
    return n;
  }

  static BTree* build_from_ordered_vector(const vector<TK>& elements, int order){
    BTree<TK>* arbolb = new BTree<TK>(order);
    for (const TK& e : elements) arbolb->insert(e);
    return arbolb;
  }

  bool check_properties(){
    if(!root) return true;
    int minKeys = (M + 1) / 2 - 1;
    int nivelHoja= -1;
    bool ok = checkNodo(root, 0, minKeys, nivelHoja);
    return ok;
  }

  ~BTree(){ clear(); }
};

#endif
