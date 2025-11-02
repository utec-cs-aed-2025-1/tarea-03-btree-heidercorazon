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

  // --- Función auxiliar recursiva para verificar propiedades ---
  bool checkNodo(Node<TK>* node, int nivel, int minKeys, int& leafLevel) {
    if (!node) return true;

    // Reglas de cantidad de keys por nodo
    if (node == root) {
      if (node->count < 1 || node->count > M - 1) return false;
    } else {
      if (node->count < minKeys || node->count > M - 1) return false;
    }

    // Claves ordenadas
    for (int i = 1; i < node->count; ++i)
      if (node->keys[i - 1] >= node->keys[i])
        return false;

    // Si es hoja, verificar nivel consistente
    if (node->leaf) {
      if (leafLevel == -1)
        leafLevel = nivel;
      else if (leafLevel != nivel)
        return false;
      return true;
    }

    // Verificar hijos no nulos
    for (int i = 0; i <= node->count; ++i)
      if (node->children[i] == nullptr)
        return false;

    // Verificar relación entre claves e hijos
    for (int i = 0; i <= node->count; ++i) {
      if (i > 0) {
        // todas las claves en hijo[i-1] < keys[i-1]
        Node<TK>* left = node->children[i - 1];
        if (left->keys[left->count - 1] >= node->keys[i - 1])
          return false;
      }
      if (i < node->count) {
        // todas las claves en hijo[i] > keys[i]
        Node<TK>* right = node->children[i];
        if (right->keys[0] <= node->keys[i])
          return false;
      }
    }

    // Verificar recursivamente los hijos
    for (int i = 0; i <= node->count; ++i)
      if (!checkNode(node->children[i], nivel + 1, minKeys))
        return false;

    return true;
  }


 public:
  BTree(int _M) : root(nullptr), M(_M), n(0) {}

  bool search(TK key);//indica si se encuentra o no un elemento
  void insert(TK key);//inserta un elemento
  //elimina un elemento
  void remove(TK key){
    if(!search(key)) return; //si no se encuentra la clave no se hace nada
    if(root==nullptr) return; //arbol vacio
    if (root->count == 0) {
      Node<TK>* ptrRoot = root;
      if (root->leaf)
        root = nullptr;
      else
        root = root->children[0];
      delete ptrRoot;
    }
    //DEUDA TÉCNICA: FALTA PARA LOS DEMÁS CASOS removeKey(root, key);

  }


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
  static BTree* build_from_ordered_vector(const vector<TK>& elements, int order){
    BTree<TK>* arbolb = new BTree<TK>(order);
    if(elements.empty()) return arbolb;
    int maxKeys = order - 1;
    int n = elements.size();
    arbolb->n = n;
    vector<Node<TK>*> nivelActual;

    //lleno las hojas
    for(int i=0; i<n; i+=maxKeys){
      Node<TK>* hoja = new Node<TK>(order);
      hoja->leaf = true;
      for(int j=0; j<maxKeys && (i+j)<n; ++j){
        hoja->keys[j] = elements[i+j];
        hoja->count++;
      }
      nivelActual.push_back(hoja);
    }

    //construyo niveles superiores
    while(nivelActual.size() > 1){
      vector<Node<TK>*> nextLevel;

      for(int i=0; i<nivelActual.size(); i+=order){
        Node<TK>* padre = new Node<TK>(order);
        padre->leaf = false;
        int cantHijos = 0;
        for(int j=i; j<order + 1 && j < nivelActual.size(); ++j){
          padre->children[cantHijos++] = nivelActual[j];
        }
        for(int k=0; k<cantHijos - 1; ++k){
          padre->keys[k] = nivelActual[i + k]->keys[0];

        }
        nextLevel.push_back(padre);
        padre->count = cantHijos - 1;

      }
      nivelActual = nextLevel;
    }
    arbolb->root = nivelActual[0];
    return arbolb;

  }



  // Verifique las propiedades de un árbol B
  bool check_properties(){
    if(!root) return true; //arbol vacio cumple propiedades
    int minKeys = (M + 1) / 2 - 1; // mínimo permitido (excepto raíz)
    int nivelHoja= -1;            // inicializar hasta encontrar verdadero nivel de las hojas


    return checkNodo(root, 0, minKeys,nivelHoja);
  }

  ~BTree(){ clear(); }    // liberar memoria
};

#endif
