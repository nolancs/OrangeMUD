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
#include "CommonTypesCW.h"
#if(TARGET_WIN)
#include "TCPManagerWin.h"
#include "TCPLinkWin.h"






//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManager::Get
//
//   Desc:: Access to the singleton class TCPManager. It returns which ever 
//          TCPManager it's using for the current OS, so each OS implements 
//          it's own version of the Get() function for TCPManager.
//
// Output:: The TCPManager object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPManager* TCPManager::Get()
{
    if(!sTCPManager)
        sTCPManager =  new TCPManagerWin();
        
    return sTCPManager;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



TCPManagerWin::TCPManagerWin()
{
}



TCPManagerWin::~TCPManagerWin()
{
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::Initialize
//
//   Desc:: Specific initialization code for each OS specific TCPManager. It
//          must also call the superclass initialization code.
//
// Output:: True if TCP services could be initialized.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerWin::Initialize()
{
    TCPManager::Initialize();

    //** Initialize WinSock **//
    WSADATA nWinSockInfo;
    SHORT   nRequestVersion = MAKEWORD(2, 2);
    INT     nResult;


    nResult = WSAStartup(nRequestVersion, &nWinSockInfo);
    if(nResult != 0)
        ThrowTCPMsg(kProblemTCP_NoNetwork, "WinSock 1.0 not available.");

    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::Shutdown
//
//   Desc:: Specific shutdown code for each OS specific TCPManager. It must
//          also call the superclass shutdown code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerWin::Shutdown()
{
    TCPManager::Shutdown();

    WSACleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::ForwardLookup
//
//   Desc:: Will lookup the 128.100.12.90 numbered address corresponding to
//          to the given hostname. It blocks unitl it's done or throws an
//          exception if it errors.
//
//  Input:: The hostname to lookup.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerWin::ForwardLookup(const STRING& hostName)
{
    if(IsInDottedIPForm((STRING&)hostName))
        return hostName;

    struct hostent* nHostInfo   = NULL;
    const CHAR*     nHostName   = hostName.c_str();
    
    
    if(!(nHostInfo = gethostbyname(nHostName)))
        ThrowTCPMsg(kProblemTCP_InvalidHostname, hostName);
    
    return inet_ntoa(*((struct in_addr*)nHostInfo->h_addr));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::ReverseLookup
//
//   Desc:: Will lookup the hostname corresponding to a 128.100.12.90 numbered 
//          address. Returns an empty string if there is no hostname associated
//          with the ip.
//
//  Input:: The IP address to reverse lookup.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerWin::ReverseLookup(const STRING& ipAddr)
{
    const CHAR*         nHostIP     = ipAddr.c_str();
    struct hostent*     nHostInfo   = NULL;
    struct sockaddr_in  nSockAddr;


    memset(&nSockAddr, 0, sizeof(struct sockaddr_in));
    nSockAddr.sin_addr.s_addr = inet_addr(nHostIP);
    if(nSockAddr.sin_addr.s_addr == (ULONG) -1)
        ThrowTCPMsg(kProblemTCP_InvalidIP, ipAddr);
    
    
    nHostInfo = gethostbyaddr((char*) &nSockAddr.sin_addr, sizeof(nSockAddr.sin_addr), AF_INET);
    if(!nHostInfo)
        return "";  //Has no name, this is normal for a lot of IPs

    return nHostInfo->h_name;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::GetMachineIP
//
//   Desc:: Returns the IP address of the machine this code is running on.
//
// Output:: The IP address in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerWin::GetMachineIP()
{
    hostent*        nHostInfo       = NULL;
    STRING          nMachineName    = GetMachineName();
    const CHAR*     nMachineNameStr = nMachineName.c_str();
    

    if(!(nHostInfo = gethostbyname(nMachineNameStr)))
        ThrowTCPMsg(kProblemTCP_NoMachineIP, "gethostbyname()");
        
    return inet_ntoa(*((in_addr*)nHostInfo->h_addr));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::GetMachineName
//
//   Desc:: Returns the "name" of the machine this code is running on.
//
// Output:: The machine name in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerWin::GetMachineName()
{
    #define kMaxHostNameLen  1024
    CHAR    ourName[kMaxHostNameLen];
    
    if(gethostname(ourName, kMaxHostNameLen) == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_NoMachineName, WSAGetLastError(), "gethostname()");
    
    return ourName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::StartListening
//
//   Desc:: Will start the machine listening for connection on onPort. You can
//          also specify how many connections the OS will queue up for you if
//          multiple people have connected before you've checked the port. The
//          default value is 5 since some OSes can't queue more.
//
//  Input:: onPort: the port number to start listening on.
//          maxQueue = 5: amount of connections to queue up.
//
// Output:: True if it successfully listened. Otherwise it will throw an
//          exception.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerWin::StartListening(SHORT onPort, SHORT maxQueue)
{
    //** Already Listening? **//
    if(mPortDescMap.find(onPort) != mPortDescMap.end())
        return true;
    
    
    
    //** Listening Code **//
    struct sockaddr_in  nListenAddr;
    INT                 nListenSocket;
    INT                 nResult, yes = 1;
    
    
    nListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(nListenSocket == INVALID_SOCKET)
        ThrowTCP(kProblemTCP_Listen, WSAGetLastError(), ErrorStr("Socket()"));
    
    
    nResult = setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &yes, sizeof(yes));
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Listen, WSAGetLastError(), ErrorStr("Setsockopt(SO_REUSEADDR)"));
    

    memset(&nListenAddr, 0, sizeof(struct sockaddr_in));
    nListenAddr.sin_family      = AF_INET;
    nListenAddr.sin_addr.s_addr = INADDR_ANY;
    nListenAddr.sin_port        = htons(onPort);

    nResult = bind(nListenSocket, (struct sockaddr*) &nListenAddr, sizeof(struct sockaddr));
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Listen, WSAGetLastError(), ErrorStr("Bind()"));
    

    nResult = listen(nListenSocket, maxQueue);
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Listen, WSAGetLastError(), ErrorStr("Listen()"));
    
    
    
    //** Add To Map & Return **//
    mPortDescMap[onPort] = nListenSocket;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::StopListening
//
//   Desc:: If the machine was listening on onPort it will stop listening an
//          discard any connections that happened to be waiting on the port. It
//          returns false if it wasn't listening on that port.
//
//  Input:: onPort: the port number to stop listening on.
//
// Output:: True if it success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerWin::StopListening(SHORT onPort)
{
    //** Close All Endpoints **//
    if(onPort == kPortsAll)
    {
        map<USHORT, SOCKET>::iterator nScan, nScanNext;
        
        for(nScan = mPortDescMap.begin(); nScan != mPortDescMap.end(); nScan = nScanNext)
        {
            nScanNext = nScan;  //Store the next iterator cause the current one will
            nScanNext++;        //be erased after the StopListening call.
            StopListening((*nScan).first);
        }
        return true;
    }


    //** Are We Even Listening Here? **//
    if(mPortDescMap.find(onPort) == mPortDescMap.end())
        return false;
    
    
    closesocket(mPortDescMap[onPort]);


    //** Remove From Map & Return **//
    mPortDescMap.erase(onPort);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::IncomingConnections
//
//   Desc:: Checks if there are any incoming connections on the given port. If
//          the kPortsAll default value is used then it will check if there
//          are any connections waiting on any of the listening ports.
//
//  Input:: onPort = kPortsAll: the port number to check.
//
// Output:: True if there are connections waiting.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerWin::IncomingConnections(SHORT onPort)
{
    //** Check All Endpoints **//
    if(onPort == kPortsAll)
    {
        map<USHORT, SOCKET>::iterator nScan;
        
        for(nScan = mPortDescMap.begin(); nScan != mPortDescMap.end(); nScan++)
        if(IncomingConnections((*nScan).first))
            return true;

        return false;
    }
    

    //** Are We Even Listening Here? **//
    if(mPortDescMap.find(onPort) == mPortDescMap.end())
        return false;


    //** Query Socket Code **//
    static  timeval noWait;
    INT         nListenDesc = mPortDescMap[onPort];
    fd_set      nSet;
    
    
    FD_ZERO(&nSet);
    FD_SET(nListenDesc, &nSet);
    if(select(nListenDesc+1, &nSet, NULL, NULL, &noWait) == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Poll, WSAGetLastError(), ErrorStr("select()"));
    
    return FD_ISSET(nListenDesc, &nSet) ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::RecieveConnection
//
//   Desc:: Accepts a connection from onPort. If onPort is the default value of
//          kPortsAll it accepts the first connection it can from any port. It
//          will throw an exception if it fails when trying to accept.
//
//  Input:: onPort = kPortsAll: the port to recieve the connection from.
//
// Output:: NULL if no connections were waiting. Otherwise it returns a TCPLink
//          to the new connection and adds it to the TCPManager list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPManagerWin::RecieveConnection(SHORT onPort)
{
    //** Accept A Connection From Any Port **//
    if(onPort == kPortsAll)
    {
        map<USHORT, SOCKET>::iterator nScan;
        
        for(nScan = mPortDescMap.begin(); nScan != mPortDescMap.end(); nScan++)
        if(IncomingConnections((*nScan).first))
            return RecieveConnection((*nScan).first);
            
        return NULL;
    }
    

    
    //** Are We Listening? **//
    if(mPortDescMap.find(onPort) == mPortDescMap.end())
        return NULL;



    //** Accept Connection Code **//
    sockaddr_in nPeerAddr;
    INT         nAddrSize = sizeof(nPeerAddr);
    INT         nListeningDesc = mPortDescMap[onPort];
    INT         nAcceptedDesc;
    
    
    nAcceptedDesc = accept(nListeningDesc, (sockaddr*) &nPeerAddr, &nAddrSize);
    if(nAcceptedDesc == INVALID_SOCKET)
        ThrowTCP(kProblemTCP_AcceptConnect, WSAGetLastError(), ErrorStr("Accept()"));


    //** Setup & Add New TCPLink **//
    STRING      iAddr       = inet_ntoa(*((struct in_addr*) &nPeerAddr.sin_addr));
    USHORT      iPort       = onPort;
    TCPLinkWin  *nWinLink   = new TCPLinkWin(nAcceptedDesc, iAddr, iPort);
    
    SetBlocking(nAcceptedDesc);
    Add(nWinLink);
    return nWinLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::OpenConnection
//
//   Desc:: Attempts to open a connection to an address on a port. This is
//          assumed to block indefinetly if something has gone wrong or the
//          connection wasn't immediately refused. It will throw an exception
//          if something went wrong.
//
//  Input:: hAddr: Where to connect, IP or hostname.
//          hPort: On what port to connect.
//          hViaProxy: Go through proxy server if any? Always leave true.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPManagerWin::OpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedOpenConnection(hAddr, hPort);

    struct sockaddr_in  nDestAddr;
    SOCKET              nSocket;
    INT                 nResult;
    STRING              nIPAddress = ForwardLookup(hAddr);


    //Make a socket
    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(nSocket == INVALID_SOCKET)
        ThrowTCP(kProblemTCP_OpenConnect, WSAGetLastError(), ErrorStr("Socket()"));

    //Setup address
    memset(&nDestAddr, 0, sizeof(struct sockaddr_in));
    nDestAddr.sin_family        = AF_INET;
    nDestAddr.sin_port          = htons(hPort);
    nDestAddr.sin_addr.s_addr   = inet_addr(nIPAddress.c_str());
    
    //Start blocking connect
    nResult = connect(nSocket, (struct sockaddr*) &nDestAddr, sizeof(struct sockaddr));
    if(nResult == SOCKET_ERROR)
    {
        closesocket(nSocket);
        if(WSAGetLastError() == WSAECONNREFUSED || WSAGetLastError() == WSAETIMEDOUT)
            return NULL;

        ThrowTCP(kProblemTCP_OpenConnect, WSAGetLastError(), ErrorStr("Connect()"));
    }


    //** Setup & Add New TCPLink **//
    TCPLinkWin  *nWinLink   = new TCPLinkWin(nSocket, nIPAddress, hPort);
    
    SetBlocking(nSocket);
    Add(nWinLink);
    return nWinLink;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif

//Section Note:
//All of the windows async functions just thread the blocking calls




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::AsynchForwardLookup
//
//   Desc:: Asynchronous version of ForwardLookup. It returns immediately with
//          a TCPTask of type TCPFwdLookup with which you can query Done() to 
//          determine the status. All tasks require their Process() function to
//          be called occasionally while they execute. They are not preemptive.
//          An exception will be thrown if the task failed to be created.
//
//  Input:: Hostname to lookup.
//
// Output:: The TCPFwdLookup task associated with the lookup.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPFwdLookup* TCPManagerWin::AsyncForwardLookup(const STRING& hostName)
{
    TCPFwdLookupWin*    nTask   = new TCPFwdLookupWin();
    AFX_THREADPROC      nProc   = (AFX_THREADPROC) TCPFwdLookupWin::ThreadForwardLookup;
    void*               nData   = (void*) nTask;
    INT                 nPrior  = THREAD_PRIORITY_NORMAL;
    UINT                nSize   = 0;
    DWORD               nFlags  = CREATE_SUSPENDED;


    nTask->mHostName    = hostName;
    nTask->mDone        = false;
    nTask->mThread      = AfxBeginThread(nProc, nData, nPrior, nSize, nFlags);

    if(!nTask->mThread)
        ThrowTCPMsg(kProblemTCP_Network, "AfxBeginThread()");

    nTask->mThread->ResumeThread();
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::AsynchReverseLookup
//
//   Desc:: Asynchronous version of ReverseLookup. It returns immediately with
//          a TCPTask of type TCPRevLookup with which you can query Done() to 
//          determine the status. All tasks require their Process() function to
//          be called occasionally while they execute. They are not preemptive.
//          An exception will be thrown if the task failed to be created.
//
//  Input:: The IP address to lookup.
//
// Output:: The TCPRevLookup task associated with the lookup.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPRevLookup* TCPManagerWin::AsyncReverseLookup(const STRING& ipAddr)
{
    TCPRevLookupWin*    nTask   = new TCPRevLookupWin();
    AFX_THREADPROC      nProc   = (AFX_THREADPROC) TCPRevLookupWin::ThreadReverseLookup;
    void*               nData   = (void*) nTask;
    INT                 nPrior  = THREAD_PRIORITY_NORMAL;
    UINT                nSize   = 0;
    DWORD               nFlags  = CREATE_SUSPENDED;
    

    nTask->mIPAddr  = ipAddr;
    nTask->mDone    = false;
    nTask->mThread  = AfxBeginThread(nProc, nData, nPrior, nSize, nFlags);

    if(!nTask->mThread)
        ThrowTCPMsg(kProblemTCP_Network, "AfxBeginThread()");

    nTask->mThread->m_bAutoDelete = false;
    nTask->mThread->ResumeThread();
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::AsyncOpenConnection
//
//   Desc:: Asynchronous version of OpenConnection. It returns immediately with
//          a TCPTask of type TCPOpenConnect with which you can query Done() to 
//          determine the status. All tasks require their Process() function to
//          be called occasionally while they execute. They are not preemptive.
//          An exception will be thrown if the task failed to be created.
//
//  Input:: hAddr: Where to connect, IP or hostname.
//          hPort: On what port to connect.
//          hViaProxy: Go through proxy server if any? Always leave true.
//
// Output:: The TCPOpenConnect task associated with the connection attempt.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPOpenConnect* TCPManagerWin::AsyncOpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedAsyncOpenConnection(hAddr, hPort);

    TCPOpenConnectWin*  nTask   = new TCPOpenConnectWin();
    AFX_THREADPROC      nProc   = (AFX_THREADPROC) TCPOpenConnectWin::ThreadOpenConnect;
    void*               nData   = (void*) nTask;
    INT                 nPrior  = THREAD_PRIORITY_NORMAL;
    UINT                nSize   = 0;
    DWORD               nFlags  = CREATE_SUSPENDED;


    nTask->mPort    = hPort;
    nTask->mAddress = hAddr;
    nTask->mThread  = AfxBeginThread(nProc, nData, nPrior, nSize, nFlags);

    if(!nTask->mThread)
        ThrowTCPMsg(kProblemTCP_Network, "AfxBeginThread()");

    nTask->mThread->m_bAutoDelete = false;
    nTask->mThread->ResumeThread();
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::AsyncRecieveConnection
//
//   Desc:: Asynchronous version of RecieveConnection. It returns immediately
//          with a TCPTask of type TCPRecvConnect with which you can query
//          Done() to determine the status. All tasks require their Process()
//          function to be called occasionally while they execute. They are not
//          preemptive. An exception will be thrown if the task failed to be 
//          created.
//
//  Input:: onPort = kPortsAll: the port to recieve the connection from.
//
// Output:: The TCPRecvConnect task associated with the acception.
//
//   Note:: Threads synchronous RecieveConnection call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPRecvConnect* TCPManagerWin::AsyncRecieveConnection(SHORT onPort)
{
    TCPRecvConnectWin*  nTask   = new TCPRecvConnectWin();
    AFX_THREADPROC      nProc   = (AFX_THREADPROC) TCPRecvConnectWin::ThreadRecvConnect;
    void*               nData   = (void*) nTask;
    INT                 nPrior  = THREAD_PRIORITY_NORMAL;
    UINT                nSize   = 0;
    DWORD               nFlags  = CREATE_SUSPENDED;


    nTask->mPort    = onPort;
    nTask->mThread  = AfxBeginThread(nProc, nData, nPrior, nSize, nFlags);

    if(!nTask->mThread)
        ThrowTCPMsg(kProblemTCP_Network, "AfxBeginThread()");

    nTask->mThread->m_bAutoDelete = false;
    nTask->mThread->ResumeThread();
    return nTask;
}

#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::SetBlocking
//
//   Desc:: Utility function. Makes the socket operate in blocking mode. Will
//          throw an exception if it failed.
//
//  Input:: The socket.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerWin::SetBlocking(SOCKET nSocket)
{
    ULONG   nOff = 0;   //Turn Non-Blocking OFF

    if(ioctlsocket(nSocket, FIONBIO, &nOff) == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Network, WSAGetLastError(), "ioctlsocket()");
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerWin::SetNonBlocking
//
//   Desc:: Utility function. Makes the socket operate in non-blocking mode.
//          Will throw an exception if it failed.
//
//  Input:: The socket.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerWin::SetNonBlocking(SOCKET nSocket)
{
    ULONG   nOn = 1;    //Turn Non-Blocking ON

    if(ioctlsocket(nSocket, FIONBIO, &nOn) == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Network, WSAGetLastError(), "ioctlsocket()");
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPFwdLookupWin::TCPFwdLookupWin()
{
    mThread     = NULL;
    mThreadExc  = NULL;
}




TCPFwdLookupWin::~TCPFwdLookupWin()
{
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupWin::Cancel
//
//   Desc:: Cancels an asyncronous TCPFwdLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: There may be memory leaks incured by killing the thread.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupWin::Cancel()
{
    if(mWasCanceled || mDone) return;
    
    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThread)
        delete mThread, mThread = NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupWin::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupWin::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupWin::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPFwdLookupWin::Results()
{
    ASSERT(!mGotResults);
    
    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mIPAddr;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupWin::ThreadForwardLookup
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous forward lookup.
//
//  Input:: A void ptr to the TCPFwdLookupWin task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UINT TCPFwdLookupWin::ThreadForwardLookup(void* nFwdLookup)
{
    TCPFwdLookupWin* nThis = (TCPFwdLookupWin*) nFwdLookup;

    try
    {
        nThis->mIPAddr  = TCPManager::Get()->ForwardLookup(nThis->mHostName);
        nThis->mDone    = true;
    }
    catch(Exception *nExc)
    {
        nThis->mThreadExc   = nExc;
        nThis->mDone        = true;
    }
    
    return 0;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPRevLookupWin::TCPRevLookupWin()
{
    mThread     = NULL;
    mThreadExc  = NULL;
}




TCPRevLookupWin::~TCPRevLookupWin()
{
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupWin::Cancel
//
//   Desc:: Cancels an asyncronous TCPRevLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: There may be memory leaks incured by killing the thread.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupWin::Cancel()
{
    if(mWasCanceled || mDone) return;
    
    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThread)
        delete mThread, mThread = NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupWin::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupWin::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupWin::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPRevLookupWin::Results()
{
    ASSERT(!mGotResults);

    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
    
    mGotResults = true;
    return mHostName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupWin::ThreadReverseLookup
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous reverse lookup.
//
//  Input:: A void ptr to the TCPRevLookupWin task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UINT TCPRevLookupWin::ThreadReverseLookup(void* nRevLookup)
{
    TCPRevLookupWin* nThis = (TCPRevLookupWin*) nRevLookup;

    try
    {
        nThis->mHostName    = TCPManager::Get()->ReverseLookup(nThis->mIPAddr);
        nThis->mDone        = true;
    }
    catch(Exception *nExc)
    {
        nThis->mThreadExc   = nExc;
        nThis->mDone        = true;
    }
    
    return 0;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPOpenConnectWin::TCPOpenConnectWin()
{
    mResultLink     = NULL;
    mThread         = NULL;
    mThreadExc      = NULL;
}




TCPOpenConnectWin::~TCPOpenConnectWin()
{
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;

    //Ack, didn't come pick up your new TCPLink
    if(mResultLink && !mGotResults)
        delete mResultLink, mResultLink = NULL;     
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectWin::Cancel
//
//   Desc:: Cancels an asyncronous TCPOpenConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: There may be memory leaks incured by killing the thread.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectWin::Cancel()
{
    if(mWasCanceled || mDone) return;
    

    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThread)
        delete mThread, mThread = NULL;

}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectWin::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectWin::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectWin::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPOpenConnectWin::Results()
{
    ASSERT(!mGotResults);

    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mResultLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectWin::ThreadOpenConnect
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous open connection.
//
//  Input:: A void ptr to the TCPOpenConnectWin task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UINT TCPOpenConnectWin::ThreadOpenConnect(void* nOpenConnect)
{
    TCPOpenConnectWin* nThis = (TCPOpenConnectWin*) nOpenConnect;

    try
    {
        nThis->mResultLink  = TCPManager::Get()->OpenConnection(nThis->mAddress, nThis->mPort);
        nThis->mDone        = true;
    }
    catch(Exception *nExc)
    {
        nThis->mThreadExc   = nExc;
        nThis->mDone        = true;
    }
    
    return 0;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPRecvConnectWin::TCPRecvConnectWin()
{
    mResultLink     = NULL;
    mThread         = NULL;
    mThreadExc      = NULL;
}




TCPRecvConnectWin::~TCPRecvConnectWin()
{
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;

    //Ack, didn't come pick up your new TCPLink
    if(mResultLink && !mGotResults)
        delete mResultLink, mResultLink = NULL;     
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectWin::Cancel
//
//   Desc:: Cancels an asyncronous TCPRecvConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: Again, this may incur a tiny memory leak inside the POSIX functions
//          when their host thread is demolished from the outside.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRecvConnectWin::Cancel()
{
    if(mWasCanceled || mDone) return;
    

    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThread)
        delete mThread, mThread = NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectWin::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRecvConnectWin::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectWin::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: NULL if no connections were waiting. Otherwise it returns a TCPLink
//          to the new connection and adds it to the TCPManager list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPRecvConnectWin::Results()
{
    ASSERT(!mGotResults);

    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mResultLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectWin::ThreadRecvConnect
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous connection accepting.
//
//  Input:: A void ptr to the TCPRecvConnectWin task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UINT TCPRecvConnectWin::ThreadRecvConnect(void* nRecvConnect)
{
    TCPRecvConnectWin* nThis = (TCPRecvConnectWin*) nRecvConnect;

    try
    {
        nThis->mResultLink  = TCPManager::Get()->RecieveConnection(nThis->mPort);
        nThis->mDone        = true;
    }
    catch(Exception *nExc)
    {
        nThis->mThreadExc   = nExc;
        nThis->mDone        = true;
    }
    
    return 0;
}




#endif //(TARGET_WIN)
