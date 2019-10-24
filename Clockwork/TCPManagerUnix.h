/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                                 [Unix]
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
#if(TARGET_UNIX)
#ifndef TCPMANAGERUNIX_H
#define TCPMANAGERUNIX_H 1
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>          //for fctl in SetNonBlocking
#include <pthread.h>        //for threads
#include <signal.h>         //for SIGKILL for pthread_kill
#include <errno.h>          //for errno
#include "TCPManager.h"



#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

#ifndef socklen_t
#define socklen_t   unsigned int
#endif

typedef INT SOCKET;



#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPManagerUnix
#endif



class TCPManagerUnix : public TCPManager
{
    friend class TCPManager;
    friend class TCPLinkUnix;
    friend class TCPFwdLookupUnix;
    friend class TCPRevLookupUnix;
    friend class TCPOpenConnectUnix;
    friend class TCPRecvConnectUnix;

    //** De/Constructors **//
    private:    TCPManagerUnix();
    public:     ~TCPManagerUnix();
    
    
    //** Inherited Member Functions **//
    public:
    virtual bool    Initialize();
    virtual void    Shutdown();


    //** Abstract Member Functions **//
    public:
    virtual STRING              GetMachineIP();
    virtual STRING              GetMachineName();
    virtual bool                StartListening(SHORT onPort, SHORT maxQueue = 5);
    virtual bool                StopListening(SHORT onPort = kPortsAll);
    virtual bool                IncomingConnections(SHORT onPort = kPortsAll);
    //Synchronous
    virtual STRING              ForwardLookup(const STRING& hostName);
    virtual STRING              ReverseLookup(const STRING& ipAddr);
    virtual TCPLink*            OpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy = true);
    virtual TCPLink*            RecieveConnection(SHORT onPort = kPortsAll);
    //Asynchronous
    virtual TCPFwdLookup*       AsyncForwardLookup(const STRING& hostName);
    virtual TCPRevLookup*       AsyncReverseLookup(const STRING& ipAddr);
    virtual TCPOpenConnect* AsyncOpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy = true);
    virtual TCPRecvConnect* AsyncRecieveConnection(SHORT onPort = kPortsAll);
    
    
    //** Protected Utility Functions **//
    protected:
    static void     SetBlocking(SOCKET nSocket);
    static void     SetNonBlocking(SOCKET nSocket);
    static TCPManagerUnix* Get() { return dynamic_cast<TCPManagerUnix*>(TCPManager::Get()); }


    //** Private Data Members **//
    private:
    map<USHORT, SOCKET> mPortDescMap;
};





///////////////////////////////////////////////////////////////////////////
// Asynchronous TCP Task classes:
// Created by the TCPManager for each OS these tasks can be passed around
// as objects while they execute asynchronously. There is more info in the
// TCPManager.h 
///////////////////////////////////////////////////////////////////////////


//Throw exception ptr while setting ours to NULL do it we don't delete
//it in the deconstructor
#define NullifyAndThrow(x)  { Exception* rTmp = (x); (x) = NULL; throw rTmp; }



#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPFwdLookupUnix
#endif


class TCPFwdLookupUnix : public TCPFwdLookup
{
    friend class TCPManagerUnix;
    
    //** De/Constructors **//
    protected: TCPFwdLookupUnix();
    public: virtual ~TCPFwdLookupUnix();


    //** Abstract TCPFLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();

    
    //** Protected Functions **//
    protected:
    static void*    ThreadForwardLookup(void* nFwdLookup);
    

    //** Protected Data Members **//
    protected:
    STRING          mHostName;
    STRING          mIPAddr;
    pthread_t       mThreadID;
    Exception*      mThreadExc;
    bool            mThreadAlive;
};


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRevLookupUnix
#endif


class TCPRevLookupUnix : public TCPRevLookup
{
    friend class TCPManagerUnix;
    
    //** De/Constructors **//
    protected: TCPRevLookupUnix();
    public:    virtual ~TCPRevLookupUnix();


    //** Abstract TCPRLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();
    

    //** Protected Functions **//
    protected:
    static void*    ThreadReverseLookup(void* nRevLookup);


    //** Protected Data Members **//
    protected:
    STRING          mIPAddr;
    STRING          mHostName;
    pthread_t       mThreadID;
    Exception*      mThreadExc;
    bool            mThreadAlive;
};



#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPOpenConnectUnix
#endif


class TCPOpenConnectUnix : public TCPOpenConnect
{
    friend class TCPManagerUnix;
    
    //** De/Constructors **//
    protected: TCPOpenConnectUnix();
    public: virtual ~TCPOpenConnectUnix();


    //** Abstract TCPOConnect Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    

    //** Protected Functions **//
    protected:
    enum { sIdle, sError, sLookup, sConnect };


    //** Protected Data Members **//
    protected:
    SHORT               mState;
    STRING              mUnresolvedAddr;
    TCPFwdLookup*       mFwdLookup;
    STRING              mIPAddress;
    USHORT              mPort;
    SOCKET              mSocket;
    struct sockaddr_in  mDestAddr;
    bool                mMadeSocket;
    bool                mRefused;
};


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRecvConnectUnix
#endif



class TCPRecvConnectUnix : public TCPRecvConnect
{
    friend class TCPManagerUnix;
    
    //** De/Constructors **//
    protected: TCPRecvConnectUnix();
    public:    virtual ~TCPRecvConnectUnix();


    //** Abstract TCPRConnect Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();


    //** Protected Functions **//
    protected:
    static void*    ThreadRecieveConnect(void* nRecvConnect);
    
    
    //** Protected Data Members **//
    protected:
    SHORT           mPort;
    TCPLink*        mResultLink;
    pthread_t       mThreadID;
    Exception*      mThreadExc;
    bool            mThreadAlive;
};




#endif //ifndef TCPMANAGERUNIX_H
#endif //(TARGET_UNIX)
