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
#ifndef TCPMANAGER_H 
#define TCPMANAGER_H 1


#define kPortsAll   -1

enum { kProxyUseNone = 0, kProxyUseSOCKSv4, kProxyUseSOCKSv5, kProxyUseHTTP };
struct SOCKSv5Prefs;
struct SOCKSv4Prefs;
struct HTTPProxyPrefs;


class TCPManager
{
    //** Singleton Access & De/Constructors**//
    public: static TCPManager* Get();
    public: static void DeleteManager();
    public: TCPManager();               
    public: virtual ~TCPManager();
    
    
    //** Inheireted Member Functions **//
    public:
    virtual bool    Initialize();   //Must be called from subclasses' Initliaze()
    virtual void    Shutdown();     //Must be called from subclasses' Shutdown()


    //** Abstract Member Functions **//
    public:
    virtual STRING              GetMachineIP() = 0;
    virtual STRING              GetMachineName() = 0;
    virtual bool                StartListening(SHORT onPort, SHORT maxQueue = 5) = 0;
    virtual bool                StopListening(SHORT onPort = kPortsAll) = 0;
    virtual bool                IncomingConnections(SHORT onPort = kPortsAll) = 0;
    //Synchronous
    virtual STRING              ForwardLookup(const STRING& hostName) = 0;
    virtual STRING              ReverseLookup(const STRING& ipAddr) = 0;
    virtual TCPLink*            OpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy = true) = 0;
    virtual TCPLink*            RecieveConnection(SHORT onPort = kPortsAll) = 0;
    //Asynchronous
    virtual TCPFwdLookup*       AsyncForwardLookup(const STRING& hostName) = 0;
    virtual TCPRevLookup*       AsyncReverseLookup(const STRING& ipAddr) = 0;
    virtual TCPOpenConnect* AsyncOpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy = true) = 0;
    virtual TCPRecvConnect* AsyncRecieveConnection(SHORT onPort = kPortsAll) = 0;
    
    //** Public Member Functions **//
    public:
    void    NoProxyPrefs();
    void    SetProxyPrefs(SOCKSv5Prefs& hPrefs);
    void    SetProxyPrefs(SOCKSv4Prefs& hPrefs);
    void    SetProxyPrefs(HTTPProxyPrefs& hPrefs);
    void    Add     (TCPLink *nLink)        { mTCPLinks.push_front(nLink);          };
    void    Remove  (TCPLink *nLink)        { mTCPLinks.remove(nLink);              };
    void    Add     (TCPFwdLookup *nTask)   { mForwardLookups.push_front(nTask);    };
    void    Remove  (TCPFwdLookup *nTask)   { mForwardLookups.remove(nTask);        };
    void    Add     (TCPRevLookup *nTask)   { mReverseLookups.push_front(nTask);    };
    void    Remove  (TCPRevLookup *nTask)   { mReverseLookups.remove(nTask);        };
    void    Add     (TCPOpenConnect *nTask){ mOpenConnections.push_front(nTask);        };
    void    Remove  (TCPOpenConnect *nTask){ mOpenConnections.remove(nTask);            };
    void    Add     (TCPRecvConnect *nTask){ mRecieveConnections.push_front(nTask); };
    void    Remove  (TCPRecvConnect *nTask){ mRecieveConnections.remove(nTask);     };

    //** Protected Member Functions **//
    protected:
    TCPLink*            ProxiedOpenConnection(const STRING& hAddr, SHORT hPort);
    TCPOpenConnect* ProxiedAsyncOpenConnection(const STRING& hAddr, SHORT hPort);

    //** Public Class Utility Functions **//
    public:
    static ULONG    StuffIP(STRING hIPAddr);        //Stuff an IP into a long
    static STRING   UnstuffIP(ULONG hDotDotNum);    //Extract an IP from a long
    static bool     IsInDottedIPForm(STRING& hAddr);
    


    
    //** Public Data Members **//
    public:
    list<TCPLink*>          mTCPLinks;          //Record of every TCPLink
    list<TCPFwdLookup*> mForwardLookups;    //Executing Asynchronous tasks
    list<TCPRevLookup*> mReverseLookups;
    list<TCPOpenConnect*>   mOpenConnections;
    list<TCPRecvConnect*>   mRecieveConnections;
    
    //** Protected Data Members **//
    protected:
    CHAR                    mProxyType;
    SOCKSv5Prefs*           mSOCKSv5Prefs;
    SOCKSv4Prefs*           mSOCKSv4Prefs;
    HTTPProxyPrefs*         mHTTPProxyPrefs;
    
    //** Singleton Member **//
    protected:
    static TCPManager*      sTCPManager;
};



////////////////////////////////////////////////////////////////////////////
// Proxy Settings Structures:
// Setup one of these and pass it in to SetProxyPrefs to get full proxied
// communication with zero changes to your code. You'd want to do this right
// after initializing the TCPManager. Proxies allow communications from a
// firewalled network to the outside world via an intermediary server.
////////////////////////////////////////////////////////////////////////////

struct SOCKSv5Prefs
{
    STRING  mServer;
    SHORT   mPort;
    STRING  mUsername;
    STRING  mPassword;
};

struct SOCKSv4Prefs
{
    STRING  mServer;
    SHORT   mPort;
    STRING  mUsername;
};

struct HTTPProxyPrefs
{
    STRING  mServer;
    SHORT   mPort;
    STRING  mUsername;
    STRING  mPassword;
};


////////////////////////////////////////////////////////////////////////////
// Asynchronous TCP Task classes:
// Created by the TCPManager for each OS these tasks can be passed around
// as objects while they execute asynchronously. They will use the native
// networking code for asynchronous TCP and continue processing when their
// Process() member function is called. This will allow the task to advance
// its state and undertake the next step of native processing if it needs 
// to.
//
// Each task may be canceled by calling its Cancel() member functions but
// that does not mean it is done canceling the task. You must still wait
// for Done() to return true before you know the task is canceled. Then
// you may dispose of the task object.
//
// When Done() returns true you can get the results with each task's
// specific Result() member function. This is only if WasCanceled()
// returns false and the operation completed successfully. You are only
// allowed to call Results() ONCE, you should then delete the task object.
//
// You should note that Process() can/does throw exceptions on errors. Be
// prepared to handle the exception. TCPManager keeps its own records of
// each task. Calling any task's destructor will remove it from the
// TCPManager.
////////////////////////////////////////////////////////////////////////////




class TCPTask
{
    //** De/Constructors **//
    public: TCPTask()           { mWasCanceled = false; mDone = false; mGotResults = false; }
    public: virtual ~TCPTask()  { }
    
    //** Public Abstract Functions **//
    public:
    virtual void    Cancel()        = 0;
    virtual void    Process()       = 0;
    virtual bool    Done()          { return mDone; }
    virtual bool    WasCanceled()   { return mWasCanceled; }
    
    //** Protected Data Members **//
    protected:
    bool    mGotResults;
    bool    mWasCanceled;
    bool    mDone;
};


class TCPFwdLookup : public TCPTask
{
    //** De/Constructors **//
    public: virtual ~TCPFwdLookup() { TCPManager::Get()->Remove(this); };

    //** Abstract Functions **//
    public:
    virtual void    Cancel()        = 0;
    virtual void    Process()       = 0;
    virtual STRING  Results()       = 0;
};


class TCPRevLookup : public TCPTask
{
    //** De/Constructors **//
    public: virtual ~TCPRevLookup() { TCPManager::Get()->Remove(this); };

    //** Abstract Functions **//
    public:
    virtual void    Cancel()        = 0;
    virtual void    Process()       = 0;
    virtual STRING  Results()       = 0;
};


class TCPOpenConnect : public TCPTask
{
    //** De/Constructors **//
    public: virtual ~TCPOpenConnect()   { TCPManager::Get()->Remove(this); };

    //** Abstract Functions **//
    public:
    virtual void        Cancel()        = 0;
    virtual void        Process()       = 0;
    virtual TCPLink*    Results()       = 0;
};


class TCPRecvConnect : public TCPTask
{
    //** De/Constructors **//
    public: virtual ~TCPRecvConnect()   { TCPManager::Get()->Remove(this); };

    //** Abstract Functions **//
    public:
    virtual void        Cancel()        = 0;
    virtual void        Process()       = 0;
    virtual TCPLink*    Results()       = 0;
};



////////////////////////////////////////////////////////////////////////////
// Asynch Proxy Connection Classes:
// Masquerading as normal TCPOpenConnect classes these will transparently
// be integrated into the Open* calls if the proxy settings require them to
// be.
////////////////////////////////////////////////////////////////////////////

class TCPOpenConnectSOCKS : public TCPOpenConnect
{
    //** De/Constructors **//
    friend class TCPManager;
    public: TCPOpenConnectSOCKS(const SOCKSv5Prefs* hPrefs, const STRING& hAddr, SHORT hPort);
    public: TCPOpenConnectSOCKS(const SOCKSv4Prefs* hPrefs, const STRING& hAddr, SHORT hPort);
    public: virtual ~TCPOpenConnectSOCKS();

    //** Abstract Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    
    //** Protected Data Members **//
    protected:
    struct SOCKSPrefsAny
    {
        bool    mVers4;
        STRING  mServer;
        SHORT   mPort;
        STRING  mUsername;
        STRING  mPassword;
    }                   mPrefs;
    LONG                mDestIP;
    SHORT               mDestPort;
    TCPFwdLookup*       mResolving;
    TCPOpenConnect* mConnecting;
    TCPLink*            mLink;
    CHAR                mResponse[10];
    CHAR                mState;
    enum
    {
        kFindDest, 
        kConnecting, 
        kSendV4Request, 
        kWaitV4Response, 
        kSendV5DetermineAuth, 
        kWaitV5AuthResponse,
        kSendV5UserPassAuth,
        kWaitV5UserPassResponse,
        kSendV5ConnectRequest,
        kWaitV5ConnectResponse,
        kIdle
    };
};



class TCPOpenConnectHTTP : public TCPOpenConnect
{
    //** De/Constructors **//
    friend class TCPManager;
    public: TCPOpenConnectHTTP(const HTTPProxyPrefs* hPrefs, const STRING& hAddr, SHORT hPort);
    public: virtual ~TCPOpenConnectHTTP();

    //** Abstract Functions **//
    public:
    virtual void        Cancel();
    virtual void        Process();
    virtual TCPLink*    Results();
    
    
    //** Protected Data Members **//
    protected:
    STRING              mDestAddr;          //Destination Server
    SHORT               mDestPort;
    STRING              mServer;            //Proxy Server
    SHORT               mPort;
    STRING              mUsername;
    STRING              mPassword;
    TCPFwdLookup*       mResolving;     //Misc
    TCPOpenConnect* mConnecting;
    TCPLink*            mLink;
    CHAR                mState;
    CHAR                mResponse[256]; //Buffers
    CHAR                mLastFour[4];
    SHORT               mReadForHeader;
    enum
    {
        kConnecting,
        kSendConnect,
        kWaitResponse,
        kFinishReadHeader,
        kIdle
    };
};




////////////////////////////////////////////////////////////////////////////
// TCP Exception Information
////////////////////////////////////////////////////////////////////////////
#include "Exception.h"
#define ThrowTCPMsg(x, y)       throw Exception(STRING("TCP Problem"), x, 0, STRING(__FILE__), __LINE__, y)
#define ThrowTCP(x, y, z)       throw Exception(STRING("TCP Problem"), x, y, STRING(__FILE__), __LINE__, z)
enum
{
    kProblemTCP_NoErr                   ,
    kProblemTCP_Network                 ,   //Generic network error.
    kProblemTCP_NoNetwork               ,   //Network is down or not setup.
    kProblemTCP_InvalidHostname         ,   //Invalid hostname passed.
    kProblemTCP_InvalidIP               ,   //Invalid IP address passed.
    kProblemTCP_NoMachineIP             ,   //Misconfigured machine. No IP address.
    kProblemTCP_NoMachineName           ,   //Misconfigured machine. No name.
    kProblemTCP_Listen                  ,   //Failed to listen.
    kProblemTCP_Poll                    ,   //Failed to poll a connection.
    kProblemTCP_AcceptConnect           ,   //Failed to accept a connection.
    kProblemTCP_OpenConnect             ,   //Failed to open a connection.
    kProblemTCP_CloseConnect            ,   //Failed to close a connection.
    kProblemTCP_Send                    ,   //Failed to send data.
    kProblemTCP_Recv                    ,   //Failed to recieve data.
    kProblemTCP_Proxy                       //Proxy failure.
};
enum /* kProblemTCP_Proxy Error Codes */
{
    kErrTCP_Proxy_Down = 100        ,   //Proxy server is not responding
    kErrTCP_Proxy_Negotiations      ,   //Proxy negotiation failed, wrong proxy type perhaps
    kErrTCP_Proxy_AuthFailed        ,   //Proxy authentication failed
    kErrTCP_Proxy_AuthRequired      ,   //Proxy requires authentication
    kErrTCP_Proxy_S4IdentFailed     ,   //SOCKS4: Failed connection to this machine's identd
    kErrTCP_Proxy_S4IdentMisMatch   ,   //SOCKS4: Identd user different than supplied user
    kErrTCP_Proxy_S5CannotAuth      ,   //SOCKS5: Server refused all our auth methods
    kErrTCP_Proxy_S5General         ,   //SOCKS5: General SOCKS server failure
    kErrTCP_Proxy_S5Ruleset         ,   //SOCKS5: Connection not allowed by ruleset
    kErrTCP_Proxy_S5NetworkUR       ,   //SOCKS5: Network unreachable
    kErrTCP_Proxy_S5HostUR          ,   //SOCKS5: Host unreachable
    kErrTCP_Proxy_S5TTLExpired      ,   //SOCKS5: TTL Expired
    kErrTCP_Proxy_S5CmdNotSupp      ,   //SOCKS5: Command not supported
    kErrTCP_Proxy_S5AddrNotSupp     ,   //SOCKS5: Address type not supported
    kErrTCP_Proxy_HTTP4xx               //HTTP: Misc failure (a 4xx code).
};

//Generic Exception Messages For Failed Tasks
#define kFailedTaskFwdLookup    "An error occured during forward lookup."
#define kFailedTaskRevLookup    "An error occured during reverse lookup."
#define kFailedTaskOpenConnect  "An error occured during the connection attempt."
#define kFailedTaskRecvConnect  "An error occured while recieving the connection."



#endif //ifndef TCPMANAGER_H
