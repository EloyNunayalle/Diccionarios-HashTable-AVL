#include <iostream>
#include <vector>
using namespace std;

const int maxColision = 3;


//itera sobre el hashtable manteniendo el orden de insercion
template <typename TK, typename TV> class HashTable;

template <typename TK, typename TV>
class HashIterator
{
private:
    using Entry = typename HashTable<TK,TV>::Entry;
    Entry* cur;

public:
    HashIterator(Entry* p=nullptr): cur(p) {}
    HashIterator<TK, TV> &operator=(HashIterator<TK, TV> other){ cur=other.cur; return *this; }
    bool operator!=(HashIterator<TK, TV> other){ return cur!=other.cur; }
    HashIterator<TK, TV> &operator++(){ if(cur) cur=cur->ins; return *this; }   //++it
    pair<TK,TV> operator*(){ return {cur->k,cur->v}; }
};

template <typename TK, typename TV>
class HashTable
{
public:
    struct Entry{ TK k; TV v; Entry* next; Entry* ins; };
    typedef HashIterator<TK, TV>  iterator;
    iterator begin(){ return iterator(head); }// Retorna el inicio del iterador
    iterator end(){ return iterator(nullptr);} // Retorna el final del iterador

private:

    int capacity;//capacidad del hash table
    int size;//total de elementos
    Entry** buckets;
    Entry* head;
    Entry* tail;

public:
    HashTable(int _cap=5): capacity(_cap), size(0)  {
        buckets=new Entry*[capacity];
        for(int i=0;i<capacity;i++) buckets[i]=nullptr;
        head=tail=nullptr;
    }
    void insert(TK key, TV value){ insert({key,value}); }
    void insert(pair<TK, TV> item){
        int idx=std::hash<TK>{}(item.first)%capacity;
        Entry* p=buckets[idx];
        while(p){ if(p->k==item.first){ p->v=item.second; return;} p=p->next; }
        Entry* n=new Entry{item.first,item.second,buckets[idx],nullptr};
        buckets[idx]=n;
        if(!head){ head=tail=n; }
        else{ tail->ins=n; tail=n; }
        size++;
        int col=0; for(p=buckets[idx];p;p=p->next) col++;
        if(col>maxColision) rehashing();
    }
    TV& at(TK key){
        int idx=std::hash<TK>{}(key)%capacity;
        for(Entry* p=buckets[idx];p;p=p->next) if(p->k==key) return p->v;
        throw std::out_of_range("key");
    }
    TV& operator[](TK key){
        int idx=std::hash<TK>{}(key)%capacity;
        for(Entry* p=buckets[idx];p;p=p->next) if(p->k==key) return p->v;
        insert(key,TV());
        return at(key);
    }
    bool find(TK key){
        int idx=std::hash<TK>{}(key)%capacity;
        for(Entry* p=buckets[idx];p;p=p->next) if(p->k==key) return true;
        return false;
    }
    bool remove(TK key){
        int idx=std::hash<TK>{}(key)%capacity;
        Entry* prev=nullptr; Entry* p=buckets[idx];
        while(p){
            if(p->k==key){
                if(prev) prev->next=p->next; else buckets[idx]=p->next;
                if(p==head) head=p->ins;
                else{
                    Entry* q=head;
                    while(q&&q->ins!=p) q=q->ins;
                    if(q) q->ins=p->ins;
                }
                if(p==tail) tail=nullptr;
                delete p; size--; return true;
            }
            prev=p; p=p->next;
        }
        return false;
    }
    int getSize(){ return size; }

    /*itera sobre el hashtable manteniendo el orden de insercion*/
    vector<TK> getAllKeys(){
        vector<TK> res; res.reserve(size);
        for(auto it=begin();it!=end();++it) res.push_back((*it).first);
        return res;
    }
    vector<pair<TK, TV>> getAllElements(){
        vector<pair<TK,TV>> res; res.reserve(size);
        for(auto it=begin();it!=end();++it) res.push_back(*it);
        return res;
    }
private:
    /*Si una lista colisionada excede maxColision, redimensionar el array*/
    void rehashing(){
        int newCap=capacity*2;
        Entry** newB=new Entry*[newCap];
        for(int i=0;i<newCap;i++) newB[i]=nullptr;
        for(Entry* p=head;p;p=p->ins){
            int idx=std::hash<TK>{}(p->k)%newCap;
            p->next=newB[idx];
            newB[idx]=p;
        }
        delete[] buckets;
        buckets=newB;
        capacity=newCap;
    }
};

