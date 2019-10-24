/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                         [Crossplatform]
 License: MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Copyright 2000-2019 Matthew Nolan, All Rights Reserved
******************************************************************************/
#ifndef STLFACADES_H
#define STLFACADES_H 1
#include <ctime>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <algorithm>
#include <stack>

//***************************************************************************//
//**        Facades To Nasty STL Classes, Our Own Template Classes         **//
//***************************************************************************//
//Contents: Vector, List, Map, APtr (auto_ptr)
using namespace std;

template<typename Type>
class Vector : public vector<Type>
{
    public:
    Vector() : vector<Type>() {}
    Vector(LONG hSize) : vector<Type>(hSize) {}
    void Add(Type hThing)       { this->push_back(hThing); }
    void AddBack(Type hThing)   { this->push_back(hThing); }
    void AddFront(Type hThing)  { this->insert(this->begin(), hThing); }
    void AddAt(Type hThing, long hIndex)
    {
        if(hIndex >= this->size())
            this->resize(hIndex+1);
        this->operator[](hIndex) = hThing;
    }
    void AddPtrAt(Type hThing, long hIndex)
    {
        while(hIndex >= this->size())
            this->push_back(NULL);
        assert(this->operator[](hIndex) == NULL);
        this->operator[](hIndex) = hThing;
    }
    bool Exists(Type hThing)
    {
        typename vector<Type>::iterator nFind = std::find(this->begin(), this->end(), hThing);
        return nFind == this->end() ? false : true;
    }
    bool Remove(Type hThing)
    {
        typename vector<Type>::iterator nFind = std::find(this->begin(), this->end(), hThing);
        
        if(nFind != this->end())
            { this->erase(nFind); return true; }
        
        return false;
    }
    void RemoveAt(long index) { this->erase(this->begin()+index); }
};




template<typename Type>
class List : public list<Type>
{
    public:
//  typedef class iterator Iterator;

    void    Add(Type hThing)        { AddFront(hThing); }
    void    AddFront(Type hThing)   { this->push_front(hThing); }
    void    AddBack(Type hThing)    { this->push_back(hThing); }
    bool    Exists(Type hThing)     { return Find(hThing) == this->end() ? false : true; }
    void    RemoveAt(LONG hIndex)   { this->erase(this->begin()+hIndex); }

    typename list<Type>::iterator Find(Type hThing)
    {
        typename list<Type>::iterator nFind = std::find(this->begin(), this->end(), hThing);
        return nFind;
    }

    bool Remove(Type hThing)
    {
        typename list<Type>::iterator nFind = std::find(this->begin(), this->end(), hThing);
        
        if(nFind != this->end())
            { this->erase(nFind); return true; }
            
        return false;
    }
    
};




template<typename T1, typename T2>
class Map : public map<T1, T2>
{
    public:
    bool    Exists(T1 thing)    { return this->find(thing) == this->end() ? false : true; }
    bool    Remove(T1 thing)
    {
        typename map<T1, T2>::iterator nRef = this->find(thing);
        
        if(nRef == this->end())
            return false;
            
        this->erase(nRef);
        return true;
    }
    void    Add(T1 hKey, T2 hData)
    {
        this->operator[](hKey) = hData;
    }
};




template<typename Type>
class APtr
{
    public:
    APtr()              { mPtr = NULL; }
    APtr(Type* hPtr)    { mPtr = hPtr; }
    ~APtr()             { Dispose(); }
    
    bool    IsNull() const          { return mPtr == NULL; }
    Type*   Ptr()                   { return mPtr; }
    void    Dispose()               { if(mPtr) delete mPtr; mPtr = NULL; }
    Type*   Detach()                { Type* nTmp = mPtr; mPtr = NULL; return nTmp; }
    APtr&   operator=(Type* hPtr)   { Dispose(); mPtr = hPtr; return *this; }
    Type&   operator*() const       { return *mPtr; }
    Type*   operator->() const      { return  mPtr; }
    bool    operator!() const       { return IsNull(); }
    
    //You can pass the autoptr off as the ptr type anywhere, be careful though
    operator Type*()                { return mPtr; }
    
    private:
    Type* mPtr;
};



#endif //ifndef STLFACADES_H
