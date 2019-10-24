/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                              [Windows]
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
#if(TARGET_WIN)
#ifndef TCPMANAGERWIN_H
#define TCPMANAGERWIN_H 1
#include "TCPManager.h"


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPManagerWin
#endif



class TCPManagerWin : public TCPManager
{
    friend class TCPManager;
    friend class TCPLinkWin;
    friend class TCPFwdLookupWin;
    friend class TCPRevLookupWin;
    friend class TCPOpenConnectWin;
    friend class TCPRecvConnectWin;

    //** De/Constructors **//
    private:    TCPManagerWin();
    public:     ~TCPManagerWin();
    
    
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
    static TCPManagerWin* Get() { return dynamic_cast<TCPManagerWin*>(TCPManager::Get()); }


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


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPFwdLookupWin
#endif


//Throw exception ptr while setting ours to NULL do it we don't delete
//it in the deconstructor
#define NullifyAndThrow(x)  { Exception* rTmp = (x); (x) = NULL; throw rTmp; }



class TCPFwdLookupWin : public TCPFwdLookup
{
    friend class TCPManagerWin;
    
    //** De/Constructors **//
    protected: TCPFwdLookupWin();
    public: virtual ~TCPFwdLookupWin();


    //** Abstract TCPFLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();

    
    //** Protected Functions **//
    protected:
    static UINT ThreadForwardLookup(void* nFwdLookup);


    //** Private Data Members **//
    private:
    STRING          mHostName;
    STRING          mIPAddr;
    CWinThread*     mThread;
    Exception*      mThreadExc;
};

#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRevLookupWin
#endif


class TCPRevLookupWin : public TCPRevLookup
{
    friend class TCPManagerWin;
    
    //** De/Constructors **//
    protected: TCPRevLookupWin();
    public:    virtual ~TCPRevLookupWin();


    //** Abstract TCPRLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();
    

    //** Protected Functions **//
    protected:
    static UINT ThreadReverseLookup(void* nRevLookup);


    //** Protected Data Members **//
    protected:
    STRING      mIPAddr;
    STRING      mHostName;
    CWinThread* mThread;
    Exception*  mThreadExc;
};



#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPOpenConnectWin
#endif


class TCPOpenConnectWin : public TCPOpenConnect
{
    friend class TCPManagerWin;
    
    //** De/Constructors **//
    protected: TCPOpenConnectWin();
    public: virtual ~TCPOpenConnectWin();


    //** Abstract TCPOConnect Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    

    //** Protected Functions **//
    protected:
    static UINT ThreadOpenConnect(void* nRevLookup);


    //** Protected Data Members **//
    protected:
    STRING          mAddress;
    USHORT          mPort;
    TCPLink*        mResultLink;
    CWinThread*     mThread;
    Exception*      mThreadExc;
};


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRecvConnectWin
#endif



class TCPRecvConnectWin : public TCPRecvConnect
{
    friend class TCPManagerWin;
    
    //** De/Constructors **//
    protected: TCPRecvConnectWin();
    public:    virtual ~TCPRecvConnectWin();

    //** Abstract TCPRConnect Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    
    
    //** Protected Functions **//
    protected:
    static UINT ThreadRecvConnect(void* nRecvConnect);

    
    //** Private Data Members **//
    private:
    SHORT           mPort;
    TCPLink*        mResultLink;
    CWinThread*     mThread;
    Exception*      mThreadExc;
};



#endif //ifndef TCPMANAGERWIN_H
#endif //(TARGET_WIN)
