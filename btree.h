#ifndef BTree_H
#define BTree_H
#include <iostream>
#include <vector>
#include <string>
#include "node.h"

using namespace std;

template <typename TK>
class BTree {
 private:
  Node<TK>* root;
  int M;  // grado u orden del arbol
  int n; // total de elementos en el arbol 

  // INORDER AUXILIAR
  void toStringNode(const Node<TK>* nodoActual, string& resultado, const string& separador, bool& esPrimero) const {
    if (!nodoActual) return;
    if (nodoActual->leaf) {//si es hoja
      for (int indiceClave = 0; indiceClave < nodoActual->count; ++indiceClave) {
        if (!esPrimero) resultado += separador;
        resultado += to_string(nodoActual->keys[indiceClave]);//agregamos la clave al resultado
        esPrimero = false;//indicamos que ya no es el primer elemento
      }
    } else {
      for (int indiceClave = 0; indiceClave < nodoActual->count; ++indiceClave) {//iteramos sobre las claves
        toStringNode(nodoActual->children[indiceClave], resultado, separador, esPrimero);
        if (!esPrimero) resultado += separador;//la condicion seria si no es el primer elemento se agrega el separador
        resultado += to_string(nodoActual->keys[indiceClave]);
        esPrimero = false;
      }
      toStringNode(nodoActual->children[nodoActual->count], resultado, separador, esPrimero);//llamamos al ultimo hijo
    }
  }

 public:
  BTree(int _M) : root(nullptr), M(_M), n(0) {}

  bool search(TK key);//indica si se encuentra o no un elemento
  void insert(TK key);//inserta un elemento
  void remove(TK key);//elimina un elemento

  int height() const { // altura del arbol y consideramos la altura de un arbol vacio como 0
    if (root == nullptr) return 0;
    int h = 1;
    const Node<TK>* cur = root;
    while (!cur->leaf) {
      cur = cur->children[0];
      ++h;
    }
    return h;
  }

  // recorrido inorder
  string toString(const string& sep) const {//
    if (root == nullptr) return string();
    string resultado;
    bool esPrimero = true;
    toStringNode(root, resultado, sep, esPrimero);
    return resultado;
  }


  vector<TK> rangeSearch(TK begin, TK end);

  TK minKey();  // minimo valor de la llave en el arbol
  TK maxKey();  // maximo valor de la llave en el arbol

  void clear() {
    if (root != nullptr) {
      root->killSelf();
      delete root;
      root = nullptr;
    }
    n = 0;
  }

  int size() { return n; } // retorna el total de elementos insertados

  // Construya un árbol B a partir de un vector de elementos ordenados
  static BTree* build_from_ordered_vector(const vector<TK>& elements, int order);
  // Verifique las propiedades de un árbol B
  bool check_properties();

  ~BTree() { clear(); }    // liberar memoria
};

#endif
