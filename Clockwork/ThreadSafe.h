/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2001                                         [Crossplatform]
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
/******************************************************************************
 THREADSAFE DOCUMENTATION
 
 Quick Examples:
    Declared as before...             ThreadSafe Declaration
    -------------------------------   -----------------------------------------
    static LONG nFoo;                 static CThreadSafe<LONG> nFoo;
    static LONG nFoo = -1;            static CThreadSafe<LONG> nFoo(-1);
    static STRING nFoo = "Bar";       static CThreadSafe<STRING> nFoo("Bar");
    static CPureAutoPtr<Obj> nFoo;    static CThreadSafePtr<Obj> nFoo;
    static Obj* nFoo = NULL;          static CThreadSafePtr<Obj> nFoo;
    
 Description:
    This implements a CThreadSafe<> template class that can wrap around any
    type of object/data that used to be static. If something was declared as
    'static SHORT gThing' you can declare it to have a different instance in
    every thread by wrapping this class around it like
    'static CThreadSafe<SHORT> gThing'. CThreadSafe<> has a typecast and
    = operator so passing off gThing as a SHORT or assigning a SHORT to it
    works fine, hopefully it will not require much if any code using the
    newly-thread-safed stuff to be changed.
            
    ThreadDying:
    Since anything in a CThreadSafe<> has a different copy of the data it
    houses depending on which thread you access it from then there needs to
    be a way to deallocate the copies that are for a thread that's going to
    die. You can do this by calling...
    CThreadSafeManager::Get()->ThreadDying();
    You call this inside the thread just before you terminate it. The manager
    keeps a list of every single CThreadSafe<> in the app and tells them all
    to erase their data copy pertaining to the current thread (which is about
    to die). And so, no memory is leaked.
            
    CThreadSafePtr:
    NOTE: You will likely wrap a lot of pointers for Singletons in
    CThreadSafe<>, which is fine since you'll have a ptr for each thread --
    BUT there's no way for the ptr to be deleted automatically even when the
    map erases it during normal ThreadDying() deallocation, so if you EVER
    want to wrap a ptr in a CThreadSafe<> be sure you use a CThreadSafePtr<>
    instead. This is basically an AutoPtr version of CThreadSafe since weird
    things start happening if you try to do a CThreadSafe<CPureAutoPtr<Type>>.
    There is no "Detach()" function, just set it = NULL if you want to
    deallocate the ptr.
            
    I'm also guessing a lot of the Singletons dont have real destructors for
    deallocating memory since there was only ever one copy that lasted the
    entire execution time, so be sure to check that out as well.
******************************************************************************/
#ifndef THREADSAFE_H
#define THREADSAFE_H
#include <vector>
#include <algorithm>
#include <iostream>
#if(TARGET_UNIX)
#include <pthread.h>
#endif
namespace PortLib
{

class CThreadSafeManager;
class CThreadSafeBase;




class CThreadSafeManager
{
    //## Singleton Access ##//
    public: static CThreadSafeManager* Get();
    protected: CThreadSafeManager() {}
    public: ~CThreadSafeManager()   {}

    //## Member Functions ##//
    public:
    void    ThreadDying();
    void    Register(CThreadSafeBase* hThis);
    void    Unregister(CThreadSafeBase* hThis);
    
    //## Data Members ##//
    protected:
    vector<CThreadSafeBase*>    m_ThreadSafes;
};




class CThreadSafeBase
{
    //## De/constructors ##//
    public: CThreadSafeBase()           { CThreadSafeManager::Get()->Register(this); }
    public: virtual ~CThreadSafeBase()  { CThreadSafeManager::Get()->Unregister(this); }
    friend class CThreadSafeManager;
    
    //## Public Member Functions ##//
    public:
    virtual LONG GetThreadID();
    
    //## Protected Member Functions ##//
    protected:
    virtual void OnThreadDie() = 0;
};




template<typename ObjT, typename InValueT = ObjT>
class CThreadSafe : public CThreadSafeBase
{
    //## De/constructors ##//
    public: CThreadSafe()   {}
    public: CThreadSafe(InValueT hInitial) : m_InitialValue(hInitial)   {}
    public: ~CThreadSafe()  {}
    typedef std::map<LONG,ObjT> MapType;
    
    //## Public Member Functions ##//
    //Obtain A Reference To The ObjT
    ObjT& GetRef()
    {
        LONG                nThreadID   = GetThreadID();
        MapType::iterator   nFound      = m_Map.find(nThreadID);
        
        if(nFound == m_Map.end())
            return (m_Map[nThreadID] = m_InitialValue);
        else
            return (*nFound).second;
    }

    //Typecast Operator:
    //You can pass it off anywhere you can pass ObjT*
    operator ObjT() { return GetRef(); }

    //Operator =:
    //You can assign to it just like it was an ObjT*
    CThreadSafe<ObjT>& operator=(ObjT hAssign) { GetRef() = hAssign; return *this; }
    
    
    
    //## Protected Member Functions ##//
    protected:
    virtual void OnThreadDie()
    {
        MapType::iterator nFound;
        if((nFound = m_Map.find(GetThreadID())) == m_Map.end())
            return;
        
        m_Map.erase(nFound);
        std::cout << "Deallocated" << endl;
    }


    
    //## Protected Data Members ##//
    protected:
    MapType     m_Map;
    InValueT    m_InitialValue;
};


//Acts as both AutoPtr and ThreadSafe
template<typename ObjT>
class CThreadSafePtr : public CThreadSafeBase
{
    //## De/constructors ##//
    public: CThreadSafePtr()    {}
    public: ~CThreadSafePtr()   { DeleteAll(); }
    typedef std::map<LONG,ObjT*> MapType;
    
    //## Public Member Functions ##//
    public:
    //Obtain A Reference To The Pointer
    ObjT*& GetRef()
    {
        MapType::iterator   nFound;
        LONG                nThreadID = GetThreadID();
        
        if((nFound = m_Map.find(nThreadID)) == m_Map.end())
            m_Map[nThreadID] = NULL;

        return m_Map[GetThreadID()];
    }
    
    //Typecast Operator:
    //You can pass it off anywhere you can pass ObjT*
    operator ObjT*() { return GetRef(); }
    
    //Operator =:
    //You can assign to it just like it was an ObjT*
    CThreadSafePtr<ObjT>& operator=(ObjT* hAssign)
    { 
        ObjT*& nRef = GetRef();
        if(nRef != NULL)
            delete nRef;
            
        nRef = hAssign;
        return *this;
    }
    
    
    
    //## Protected Member Functions ##//
    protected:
    void DeleteAll()
    {
        MapType::iterator nScan;
        for(nScan = m_Map.begin(); nScan != m_Map.end(); nScan++)
        if((*nScan).second)
            delete (*nScan).second;
    }
    
    virtual void OnThreadDie()
    {
        MapType::iterator nFound;
        if((nFound = m_Map.find(GetThreadID())) == m_Map.end())
            return;
        
        delete (*nFound).second;
        m_Map.erase(nFound);
        std::cout << "Deallocated" << endl;
    }

    
    
    //## Protected Data Members ##//
    protected:
    MapType     m_Map;
};


}
#endif //ifndef THREADSAFE_H
