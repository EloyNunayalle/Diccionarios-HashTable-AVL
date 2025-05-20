#ifndef AVLTree_H
#define AVLTree_H
#include <iostream>
#include "AVL_Node.h"
#include "AVL_Iterator.h"

using namespace std;

template <typename T>
class AVLTree
{
public:
    typedef AVLIterator<T> iterator;
    iterator begin(AVLIterator<int>::Type _){// Retorna el inicio del iterador
        return iterator(root, static_cast<typename AVLIterator<T>::Type>(_));
    }
    iterator end(){// Retorna el final del iterador
        return iterator(nullptr);
    }

private:
    NodeAVL<T> *root;
public:
    AVLTree() : root(nullptr) {}
    void insert(T value){//O(log n)
        auto add=[&](auto&& self, NodeAVL<T>*& node)->void{
            if(!node){ node=new NodeAVL<T>(value); return; }
            if(value<node->data)    self(self,node->left);
            else if(value>node->data) self(self,node->right);
            balance(node);
        };
        add(add,root);
    }
    bool find(T value){//O(log n)
        NodeAVL<T>* node=root;
        while(node){
            if(node->data==value) return true;
            node = value<node->data ? node->left : node->right;
        }
        return false;
    }
    string getInOrder(){
        string out="";
        auto rec=[&](auto&& self,NodeAVL<T>* node)->void{
            if(!node) return;
            self(self,node->left);
            out+=to_string(node->data)+" ";
            self(self,node->right);
        };
        rec(rec,root);
        return out;
    }
    string getPreOrder(){
        string out="";
        auto rec=[&](auto&& self,NodeAVL<T>* node)->void{
            if(!node) return;
            out+=to_string(node->data)+" ";
            self(self,node->left);
            self(self,node->right);
        };
        rec(rec,root);
        return out;
    }
    string getPostOrder(){
        string out="";
        auto rec=[&](auto&& self,NodeAVL<T>* node)->void{
            if(!node) return;
            self(self,node->left);
            self(self,node->right);
            out+=to_string(node->data)+" ";
        };
        rec(rec,root);
        return out;
    }
    int height(){
        return root ? root->height : -1;
    }
    T minValue(){//O(log n)
        NodeAVL<T>* node=root;
        while(node && node->left) node=node->left;
        return node->data;
    }
    T maxValue(){//O(log n)
        NodeAVL<T>* node=root;
        while(node && node->right) node=node->right;
        return node->data;
    }
    bool isBalanced(){//O(n)
        auto chk=[&](auto&& self,NodeAVL<T>* node)->bool{
            if(!node) return true;
            int bf=balancingFactor(node);
            return bf>-2&&bf<2&&self(self,node->left)&&self(self,node->right);
        };
        return chk(chk,root);
    }
    int size(){
        auto cnt=[&](auto&& self,NodeAVL<T>* node)->int{
            return node?1+self(self,node->left)+self(self,node->right):0;
        };
        return cnt(cnt,root);
    }
    void remove(T value){ //Use el predecesor para cuando el nodo a eliminar tiene dos hijos
        auto del=[&](auto&& self,NodeAVL<T>*& node, T& key)->void{
            if(!node) return;
            if(key<node->data)       self(self,node->left,key);
            else if(key>node->data)  self(self,node->right,key);
            else{
                if(!node->left || !node->right){
                    NodeAVL<T>* tmp = node->left ? node->left : node->right;
                    delete node; node = tmp;
                }else{
                    NodeAVL<T>* pred=node->left;
                    while(pred->right) pred=pred->right;
                    node->data = pred->data;
                    key = pred->data;
                    self(self,node->left,key);
                }
            }
            if(node) balance(node);
        };
        del(del,root,value);
    }

    /*Adicionales*/
    T successor(T value){ // Retornar el valor siguiente de "value" en el arbol
        NodeAVL<T>* node=root;
        NodeAVL<T>* succ=nullptr;
        while(node){
            if(value<node->data){ succ=node; node=node->left; }
            else node=node->right;
        }
        return succ?succ->data:T();
    }
    T predecessor(T value){ // Retornar el valor anterior de "value" en el arbol
        NodeAVL<T>* node=root;
        NodeAVL<T>* pred=nullptr;
        while(node){
            if(value>node->data){ pred=node; node=node->right; }
            else node=node->left;
        }
        return pred?pred->data:T();
    }
    void clear(){ // Liberar todos los nodos (usar root->KillSelf)
        if(root){ root->killSelf(); root=nullptr; }
    }
    void displayPretty(){//Muestra el arbol visualmente atractivo
        auto show=[&](auto&& self,NodeAVL<T>* node,int depth)->void{
            if(!node) return;
            self(self,node->right,depth+1);
            for(int i=0;i<depth;++i) cout<<"   ";
            cout<<node->data<<"\n";
            self(self,node->left,depth+1);
        };
        show(show,root,0);
    }

    ~AVLTree(){
        if(this->root != nullptr){
            this->root->killSelf();
        }
    }

private:
    /*Rotaciones del AVL*/
    int balancingFactor(NodeAVL<T> *node){//Obtiene el factor de balanceo O(1)
        int l=node->left ? node->left->height : -1;
        int r=node->right? node->right->height: -1;
        return l-r;
    }
    void updateHeight(NodeAVL<T> *node){//Actualiza la altura de un nodo O(1)
        int l=node->left ? node->left->height : -1;
        int r=node->right? node->right->height: -1;
        node->height = 1 + (l>r?l:r);
    }
    void balance(NodeAVL<T> *&node){//Agoritmo principal que verifica el balanceo del nodo y aplica las rotaciones O(1)
        updateHeight(node);
        int bf=balancingFactor(node);
        if(bf>1){
            if(balancingFactor(node->left)<0) left_rota(node->left);
            right_rota(node);
        }else if(bf<-1){
            if(balancingFactor(node->right)>0) right_rota(node->right);
            left_rota(node);
        }
    }
    void left_rota(NodeAVL<T> *&node){ //Rotación a la izquier O(1)
        NodeAVL<T>* y=node->right;
        node->right=y->left;
        y->left=node;
        updateHeight(node);
        updateHeight(y);
        node=y;
    }
    void right_rota(NodeAVL<T> *&node){ //Rotación a la derecha O(1)
        NodeAVL<T>* x=node->left;
        node->left=x->right;
        x->right=node;
        updateHeight(node);
        updateHeight(x);
        node=x;
    }
};

#endif
