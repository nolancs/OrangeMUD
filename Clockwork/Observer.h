/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                        [Crossplatform]
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
#ifndef OBSERVER_H
#define OBSERVER_H 1

#include <stdarg.h>

typedef INT ASPECT;
template<typename ObjT> class Subject;
template<typename ObjT> class Observer;



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Observer<ClassType>
//
//   Desc:: Derive any class from Observer<ClassType> and it can observe any
//          number of ClassType objects, assuming that the class specificed by
//          ClassType is derived from Subject<ClassType> so that it CAN be
//          observered. 
//
//          You implement an Update() method for handling updates on the
//          ClassType, it passes you a pointer to the object that is sending
//          the update message, and an ASPECT which specifies more specifically
//          what has changed and what the message is about. You define your
//          aspects since you will be sending and recieving them. It also
//          passes a va_list you can use to send specific things along with
//          the message, these can vary by each ASPECT if you want.
//
//          When an observer object is deleted it stops observing all objects
//          it was observing so you don't have to worry about a subject object
//          with pointers to non-existant observers.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

template<typename ObjT>
class Observer
{
    typedef Subject<ObjT>*  SubjPtr;
    public: Observer() {}
    public: virtual ~Observer()
    {
        List<SubjPtr>::iterator nScan = mSubjects.begin();
        List<SubjPtr>::iterator nScanNext;
    
    
        for(; nScan != mSubjects.end(); nScan = nScanNext)
        {
            nScanNext = nScan;
            nScanNext++;
            StopObserving(*nScan);
        }
    }


    //** Abstract Observer Functions **//
    public:
    virtual void HandleUpdate(ObjT* hObj, ASPECT hAspect, va_list hArgs)    = 0;



    //** Member Functions **//
    public:
    void StartObserving(SubjPtr hSubj)
    {
        hSubj->AttachObserver(this);
        mSubjects.Add(hSubj);
    }

    void StopObserving(SubjPtr hSubj)
    {
        hSubj->RemoveObserver(this);
        mSubjects.Remove(hSubj);
    }

    bool IsObserving(SubjPtr hSubj)
    {
        return mSubjects.Exists(hSubj);
    }
    
    
    
    //** Private Data Members **//
    protected:
    List<SubjPtr>   mSubjects;
};





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Subject<ClassTypeThatsDerivingFromThis>
//
//   Desc:: You can derive any object from Subject and this allows it to be
//          observed by an Observer class of similar type. You can NEVER call
//          Attach/RemoveObserver, you use the methods provided in Observer
//          to make the observer start observing a subject.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

template<typename ObjT>
class Subject
{
    friend class Observer<ObjT>;
    typedef Observer<ObjT>* ObsPtr;
    public: Subject()           {}
    public: virtual ~Subject()  {}
    
    
    
    //** Member Functions **//
    void UpdateObservers(ASPECT hAspect, ...)
    {
        List<ObsPtr> nObservers = mObservers; //Make a copy
        List<ObsPtr>::iterator nScan = nObservers.begin();
        va_list nArgs;
        
        va_start(nArgs, hAspect);       
        for(; nScan != nObservers.end(); nScan++)
            (*nScan)->HandleUpdate(dynamic_cast<ObjT*>(this), hAspect, nArgs);
    }
    
    
    //** Protect Member Functions **//
    protected:
    void AttachObserver(Observer<ObjT>* hObs)
    {
        mObservers.Add(hObs);
        cout << "Attached Observer\n";
    }
    
    void RemoveObserver(Observer<ObjT>* hObs)
    {
        mObservers.Remove(hObs);
        cout << "Removed Observer\n";
    }
    
    
    
    
    //** Private Data Members **//
    protected:
    List<ObsPtr>    mObservers;
};





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// EXAMPLE
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
class Account : public Subject<Account>
{ .... };
class User : public Subject<User>
{ .... };


//
// A class that can observe Accounts and Users
//
class TestObs : public Observer<Account>, public Observer<User>
{
    public:
    void UpdateObservers(Account* hAcct, ASPECT hAspect, ...)   { ... }
    void UpdateObservers(User* hUser, ASPECT hAspect, ...)      { ... }
};


//If you're observing more than one type of object (multiple Observer<>
//derivations) you need to specify which superclass you're calling the
//Start/StopObserving functions from. The compiler will complain about
//ambiguous access to the functions otherwise. 
//Example:
{
    TestObs*    nObs    = new TestObs();
    Account*    nAcct   = new Account();
    User*       nUser   = new User();
    
    nObs->Observer<Account>::StartObserving(nAcct);
    nObs->Observer<User>::StartObserving(nUser);
}
*/






#endif  //ifndef OBSERVER_H
