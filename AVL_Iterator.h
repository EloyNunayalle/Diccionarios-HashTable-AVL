//
// Created by LUIS on 19/05/2025.
//
#ifndef AVL_ITERATOR_H
#define AVL_ITERATOR_H

#include "AVL_Node.h"
#include<stack>
#include<queue>
//Forward Iterator for the AVL/
template <typename T>
class AVLIterator
{
public:
    enum Type {
        PreOrder, InOrder, PostOrder, BFS
    };
private:
    NodeAVL<T> *current;
    std::stack<NodeAVL<T>*> primero; //Con el primero vamos a recorrer en DFS, inorder, preorder, postorder
    std::queue<NodeAVL<T>*> segundo; //Con el segundo vamos a recorrer en BFS, profundidad.
    std::stack<NodeAVL<T>*> s1, s2;
    Type type;

    void pushL(NodeAVL<T>* node) {
        while(node) {
            primero.push(node);
            node = node->left;
        }
    }

    void postorder(NodeAVL<T>* root) {
        if(!root) return;
        std::stack<NodeAVL<T>*> temp;
        temp.push(root);
        while(!temp.empty()) {
            NodeAVL<T>* node = temp.top(); temp.pop();
            s2.push(node);
            if(node->left)  temp.push(node->left);
            if(node->right) temp.push(node->right);
        }
    }
public:
    AVLIterator() : current(nullptr), type(InOrder) {};
    AVLIterator(NodeAVL<T>* root, Type type = InOrder) : current(nullptr), type(type) {
        if(type == InOrder  && root) pushL(root);
        else if(type == PreOrder && root)  primero.push(root);
        else if(type == PostOrder && root) postorder(root);
        else if(type == BFS && root) segundo.push(root);
        if(root) ++(*this);
    }
    AVLIterator<T> &operator=(AVLIterator<T> other){ current = other.current; primero = other.primero;
        segundo = other.segundo; s1 = other.s1; s2 = other.s2;
        type = other.type; return *this; }
    bool operator!=(AVLIterator<T> other) {
        return this->current != other.current;
    }
    //++it
    AVLIterator<T> &operator++() {
        if(type == InOrder) {
            if(primero.empty()) { current = nullptr; return *this; }
            current = primero.top(); primero.pop();
            if(current->right) pushL(current->right);
        }
        else if(type == PreOrder) {
            if(primero.empty()) { current = nullptr; return *this; }
            current = primero.top(); primero.pop();
            if(current->right) primero.push(current->right);
            if(current->left)  primero.push(current->left);
        }
        else if(type == PostOrder) {
            if(s2.empty()) { current = nullptr; return *this; }
            current = s2.top(); s2.pop();
        }
        else if(type == BFS) {
            if(segundo.empty()) { current = nullptr; return *this; }
            current = segundo.front(); segundo.pop();
            if(current->left)  segundo.push(current->left);
            if(current->right) segundo.push(current->right);
        }
        return *this;
    }
    T operator*() { return current->data; }
};

#endif //AVL_ITERATOR_H
