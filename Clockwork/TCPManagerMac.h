/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                                [MacOS]
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
#if(TARGET_MAC)
#ifndef TCPMANAGERMAC_H
#define TCPMANAGERMAC_H 1
#include <Files.h>
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <OpenTransportProviders.h>
#include "TCPManager.h"




#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPManagerMac
#endif


class TCPManagerMac : public TCPManager
{
    friend class TCPManager;
    friend class TCPLinkMac;
    friend class TCPFwdLookupMac;
    friend class TCPRevLookupMac;
    friend class TCPOpenConnectMac;
    friend class TCPRecvConnectMac;
    

    //** De/Constructors/Misc **//
    private: TCPManagerMac();
    public: ~TCPManagerMac();


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
        
        
    //** Protected Notifier Funcs **//
    protected:
    static pascal void YieldingNotifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie);


    //** Protected Utility Functions **//
    protected:
    static void     SetSynch(ProviderRef nEP);
    static void     SetAsynch(ProviderRef nEP, OTNotifyProcPtr nNotif);
    static TCPManagerMac* Get() { return dynamic_cast<TCPManagerMac*>(TCPManager::Get()); }
    
    
    //** Private Data Members **//
    private:
    bool                        mOTInited;
    InetSvcRef                  mOTInetServices;
    map<USHORT, EndpointRef>    mPortEndpointMap;
};




///////////////////////////////////////////////////////////////////////////
// Asynchronous TCP Task classes:
// Created by the TCPManager for each OS these tasks can be passed around
// as objects while they execute asynchronously. There is more info in the
// TCPManager.h 
///////////////////////////////////////////////////////////////////////////


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPFwdLookupMac
#endif


class TCPFwdLookupMac : public TCPFwdLookup
{
    friend class TCPManagerMac;
    
    //** De/Constructors **//
    protected: TCPFwdLookupMac();
    public: virtual ~TCPFwdLookupMac();


    //** Abstract TCPFLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();

    
    //** Protected Functions **//
    protected:
    enum { sIdle, sError, sGetService, sForLookup };
    static pascal void Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie);
    

    //** Private Data Members **//
    private:
    SHORT           mState;
    STRING          mHostName;
    InetSvcRef      mProvider;
    InetHostInfo    mHostInfo;
};


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRevLookupMac
#endif


class TCPRevLookupMac : public TCPRevLookup
{
    friend class TCPManagerMac;
    
    //** De/Constructors **//
    protected: TCPRevLookupMac();
    public:    virtual ~TCPRevLookupMac();


    //** Abstract TCPRLookup Functions **//
    public:
    virtual void    Cancel();
    virtual void    Process();
    virtual STRING  Results();
    

    //** Protected Functions **//
    protected:
    enum { sIdle, sError, sGetService, sRevLookup };
    static pascal void Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie);


    //** Protected Data Members **//
    protected:
    SHORT       mState;
    InetHost    mDotDotNum;
    InetSvcRef  mProvider;
    CHAR        mResultAddr[1024];
};



#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPOpenConnectMac
#endif


class TCPOpenConnectMac : public TCPOpenConnect
{
    friend class TCPManagerMac;
    
    //** De/Constructors **//
    protected: TCPOpenConnectMac();
    public: virtual ~TCPOpenConnectMac();


    //** Abstract TCPOConnect Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    

    //** Protected Functions **//
    protected:
    enum { sIdle, sError, sLookup, sGetEndpoint, sBind, sConnect, sRcvConnect };
    static pascal void Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie);


    //** Protected Data Members **//
    protected:
    SHORT           mState;
    STRING          mUnresolvedAddr;
    TCPFwdLookup*   mFwdLookup;
    STRING          mIPAddress;
    EndpointRef     mEndpoint;
    TCall           mPeerRequest;
    InetAddress     mPeerAddr;
    USHORT          mPort;
    bool            mBound;
    bool            mConnecting;
    bool            mConnected;
    bool            mRefused;
    CHAR            mErrorBuf[255];
};


#if 0
///////////////////////////////////////////////////////////////////////////////////////
#pragma mark TCPRecvConnectMac
#endif



class TCPRecvConnectMac : public TCPRecvConnect
{
    friend class TCPManagerMac;
    
    //** De/Constructors **//
    protected: TCPRecvConnectMac()          { mDone = true; }
    public:    virtual ~TCPRecvConnectMac() {};

    //** Abstract TCPRConnect Functions **//
    public:
    virtual void        Cancel()            {};
    virtual void        Process()           {};
    virtual TCPLink*    Results();
    
    
    //** Private Data Members **//
    private:
    SHORT       mPort;
};




#endif //ifndef TCPMANAGERMAC_H
#endif //(TARGET_MAC)
