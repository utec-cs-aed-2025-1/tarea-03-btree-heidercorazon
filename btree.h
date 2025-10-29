#ifndef BTree_H
#define BTree_H
#include <iostream>
#include <vector>

#include "node.h"

using namespace std;

template <typename TK>
class BTree {
 private:
  Node<TK>* root;
  int M;  // grado u orden del arbol
  int n; // total de elementos en el arbol

 public:
  BTree(int _M) : root(nullptr), M(_M) {}

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
    }//indica si se encuentra o no un elemento

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

        z->count = t - 1;
        for (int j = 0; j < t - 1; ++j)
            z->keys[j] = y->keys[j + t];

        if (!y->leaf) {
            for (int j = 0; j < t; ++j)
                z->children[j] = y->children[j + t];
        }

        y->count = t - 1;

        for (int j = s->count; j >= 0; --j)
            s->children[j + 1] = s->children[j];
        s->children[1] = z;
        for (int j = s->count - 1; j >= 0; --j)
            s->keys[j + 1] = s->keys[j];

        s->keys[0] = y->keys[t - 1];
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

            z->count = t - 1;
            for (int j = 0; j < t - 1; ++j)
                z->keys[j] = y->keys[j + t];

            if (!y->leaf) {
                for (int j = 0; j < t; ++j)
                    z->children[j] = y->children[j + t];
            }

            y->count = t - 1;

            for (int j = cur->count; j >= i + 1; --j)
                cur->children[j + 1] = cur->children[j];
            cur->children[i + 1] = z;

            for (int j = cur->count - 1; j >= i; --j)
                cur->keys[j + 1] = cur->keys[j];

            cur->keys[i] = y->keys[t - 1];
            cur->count++;

            if (key > cur->keys[i]) {
                i++;
                child = cur->children[i];
            }
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
    }//inserta un elemento

  void remove(TK key);//elimina un elemento
  int height();//altura del arbol. Considerar altura 0 para arbol vacio
  string toString(const string& sep);  // recorrido inorder
  vector<TK> rangeSearch(TK begin, TK end);

   TK minKey(){
    if (root == nullptr || root->count == 0) return TK{};
    const Node<TK>* x = root;
    while (!x->leaf) x = x->children[0];
        return x->keys[0];
   } // minimo valor de la llave en el arbol
   TK maxKey(){
  if (root == nullptr || root->count == 0) return TK{};

  const Node<TK>* x = root;
  while (!x->leaf) x = x->children[x->count];
  return x->keys[x->count - 1];
 }  // maximo valor de la llave en el arbol
  void clear(); // eliminar todos lo elementos del arbol
  int size(); // retorna el total de elementos insertados

  // Construya un árbol B a partir de un vector de elementos ordenados
  static BTree* build_from_ordered_vector(vector<TK> elements);
  // Verifique las propiedades de un árbol B
  bool check_properties();

  ~BTree();     // liberar memoria
};

#endif
