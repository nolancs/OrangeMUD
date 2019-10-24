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
#include "CommonTypesCW.h"
#if(TARGET_UNIX)
#include "TCPManagerUnix.h"
#include "TCPLinkUnix.h"






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
        sTCPManager =  new TCPManagerUnix();
        
    return sTCPManager;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



TCPManagerUnix::TCPManagerUnix()
{
}



TCPManagerUnix::~TCPManagerUnix()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::Initialize
//
//   Desc:: Specific initialization code for each OS specific TCPManager. It
//          must also call the superclass initialization code.
//
// Output:: True if TCP services could be initialized.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerUnix::Initialize()
{
    TCPManager::Initialize();
    //Hahaha, As if you'd need to init the unix network
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::Shutdown
//
//   Desc:: Specific shutdown code for each OS specific TCPManager. It must
//          also call the superclass shutdown code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerUnix::Shutdown()
{
    TCPManager::Shutdown();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::ForwardLookup
//
//   Desc:: Will lookup the 128.100.12.90 numbered address corresponding to
//          to the given hostname. It blocks unitl it's done or throws an
//          exception if it errors.
//
//  Input:: The hostname to lookup.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerUnix::ForwardLookup(const STRING& hostName)
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
//   Name:: TCPManagerUnix::ReverseLookup
//
//   Desc:: Will lookup the hostname corresponding to a 128.100.12.90 numbered 
//          address. Returns an empty string if there is no hostname associated
//          with the ip.
//
//  Input:: The IP address to reverse lookup.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerUnix::ReverseLookup(const STRING& ipAddr)
{
    const CHAR*         nHostIP     = ipAddr.c_str();
    struct hostent*     nHostInfo   = NULL;
    struct sockaddr_in  nSockAddr;


    memset(&nSockAddr, 0, sizeof(struct sockaddr_in));
    nSockAddr.sin_addr.s_addr = inet_addr(nHostIP);
    if(nSockAddr.sin_addr.s_addr == ((in_addr_t)-1))
        ThrowTCPMsg(kProblemTCP_InvalidIP, ipAddr);
    
    
    nHostInfo = gethostbyaddr((char*) &nSockAddr.sin_addr, sizeof(nSockAddr.sin_addr), AF_INET);
    if(!nHostInfo || !strcmp(nHostInfo->h_name, nHostIP))
        return "";  //Has no name, this is normal for a lot of IPs
    
    return nHostInfo->h_name;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::GetMachineIP
//
//   Desc:: Returns the IP address of the machine this code is running on.
//
// Output:: The IP address in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerUnix::GetMachineIP()
{
    struct hostent* nHostInfo       = NULL;
    STRING          nMachineName    = GetMachineName();
    const CHAR*     nMachineNameStr = nMachineName.c_str();
    
    
    if(!nHostInfo && !(nHostInfo = gethostbyname(nMachineNameStr)))
        ThrowTCPMsg(kProblemTCP_NoMachineIP, "gethostbyname()");
        
    return inet_ntoa(*((struct in_addr*)nHostInfo->h_addr));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::GetMachineName
//
//   Desc:: Returns the "name" of the machine this code is running on.
//
// Output:: The machine name in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerUnix::GetMachineName()
{
    #define kMaxHostNameLen 1024
    CHAR    ourName[kMaxHostNameLen];
    
    if(gethostname(ourName, kMaxHostNameLen) != 0)
        ThrowTCP(kProblemTCP_NoMachineName, errno, ErrorStr("gethostname()"));

    return ourName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::StartListening
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

bool TCPManagerUnix::StartListening(SHORT onPort, SHORT maxQueue)
{
    //** Already Listening? **//
    if(mPortDescMap.find(onPort) != mPortDescMap.end())
        return true;
    
    
    
    //** Listening Code **//
    struct sockaddr_in  nListenAddr;
    INT                 nListenSocket;
    INT                 nResult, yes = 1;
    
    
    nListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(nListenSocket == -1)
        ThrowTCP(kProblemTCP_Listen, errno, ErrorStr("Socket()"));
    
    
    nResult = setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if(nResult == -1)
        ThrowTCP(kProblemTCP_Listen, errno, ErrorStr("Setsockopt(SO_REUSEADDR)"));
    

    memset(&nListenAddr, 0, sizeof(struct sockaddr_in));
    nListenAddr.sin_family      = AF_INET;
    nListenAddr.sin_addr.s_addr = INADDR_ANY;
    nListenAddr.sin_port        = htons(onPort);

    nResult = ::bind(nListenSocket, (struct sockaddr*) &nListenAddr, sizeof(struct sockaddr));
    if(nResult == -1)
        ThrowTCP(kProblemTCP_Listen, errno, ErrorStr("Bind()"));
    

    nResult = listen(nListenSocket, maxQueue);
    if(nResult == -1)
        ThrowTCP(kProblemTCP_Listen, errno, ErrorStr("Listen()"));
    
    
    
    //** Add To Map & Return **//
    mPortDescMap[onPort] = nListenSocket;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::StopListening
//
//   Desc:: If the machine was listening on onPort it will stop listening an
//          discard any connections that happened to be waiting on the port. It
//          returns false if it wasn't listening on that port.
//
//  Input:: onPort: the port number to stop listening on.
//
// Output:: True if it success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerUnix::StopListening(SHORT onPort)
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
    
    
    close(mPortDescMap[onPort]);


    //** Remove From Map & Return **//
    mPortDescMap.erase(onPort);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::IncomingConnections
//
//   Desc:: Checks if there are any incoming connections on the given port. If
//          the kPortsAll default value is used then it will check if there
//          are any connections waiting on any of the listening ports.
//
//  Input:: onPort = kPortsAll: the port number to check.
//
// Output:: True if there are connections waiting.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerUnix::IncomingConnections(SHORT onPort)
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
    static struct timeval noWait;
    INT         nListenDesc = mPortDescMap[onPort];
    fd_set      nSet;
    
    
    FD_ZERO(&nSet);
    FD_SET(nListenDesc, &nSet);
    if(select(nListenDesc+1, &nSet, NULL, NULL, &noWait) == -1)
        ThrowTCP(kProblemTCP_Poll, errno, ErrorStr("Select()"));
    
    return FD_ISSET(nListenDesc, &nSet) ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::RecieveConnection
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

TCPLink* TCPManagerUnix::RecieveConnection(SHORT onPort)
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
    //int wasnt good enough so some asses decided to make it socklen_t in some unixes
    //and different glibc's. No matter that it breaks code and gives coders anurisms.  
    #if defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 0))
    #define ADDRSIZEARG_T socklen_t
    #elif defined(__GNUC__) && (__GNUC__ > 2)
    #define ADDRSIZEARG_T socklen_t
    #else 
    #define ADDRSIZEARG_T int
    #endif
    
    struct sockaddr_in  nPeerAddr;
    ADDRSIZEARG_T       nAddrSize       = sizeof(nPeerAddr);
    INT                 nListeningDesc  = mPortDescMap[onPort];
    INT                 nAcceptedDesc;
    
    
    nAcceptedDesc = accept(nListeningDesc, (struct sockaddr*) &nPeerAddr, &nAddrSize);
    if(nAcceptedDesc < 0)
        ThrowTCP(kProblemTCP_AcceptConnect, errno, ErrorStr("Accept()"));


    //** Setup & Add New TCPLink **//
    STRING      iAddr       = inet_ntoa(*((struct in_addr*) &nPeerAddr.sin_addr));
    USHORT      iPort       = onPort;
    TCPLinkUnix *nUnixLink  = new TCPLinkUnix(nAcceptedDesc, iAddr, iPort);
    
    SetBlocking(nAcceptedDesc);
    Add(nUnixLink);
    return nUnixLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::OpenConnection
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

TCPLink* TCPManagerUnix::OpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedOpenConnection(hAddr, hPort);

    struct sockaddr_in  nDestAddr;
    SOCKET              nSocket;
    INT                 nResult;
    STRING              nIPAddress = ForwardLookup(hAddr);


    //Make a socket
    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(nSocket == -1)
        ThrowTCPMsg(kProblemTCP_OpenConnect, ErrorStr("Socket()"));

    //Setup address
    memset(&nDestAddr, 0, sizeof(struct sockaddr_in));
    nDestAddr.sin_family        = AF_INET;
    nDestAddr.sin_port          = htons(hPort);
    nDestAddr.sin_addr.s_addr   = inet_addr(nIPAddress.c_str());
    
    //Start blocking connect
    nResult = connect(nSocket, (struct sockaddr*)&nDestAddr, sizeof(struct sockaddr));
    if(nResult != 0)
    {
        close(nSocket);
        if(errno == ECONNREFUSED || errno == ETIMEDOUT)
            return NULL;
        
        ThrowTCP(kProblemTCP_OpenConnect, errno, ErrorStr("Connect()"));
    }


    //** Setup & Add New TCPLink **//
    TCPLinkUnix* nUnixLink  = new TCPLinkUnix(nSocket, nIPAddress, hPort);
    
    SetBlocking(nSocket);
    Add(nUnixLink);
    return nUnixLink;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::AsynchForwardLookup
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
//
//   Note:: Just threads the synchronous ForwardLookup call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPFwdLookup* TCPManagerUnix::AsyncForwardLookup(const STRING& hostName)
{
    TCPFwdLookupUnix*   nTask = new TCPFwdLookupUnix();
    pthread_attr_t      nAttribs;
    INT                 nResult;
    
    //** Setup Data **//
    nTask->mHostName = hostName;


    //** Create Thread **//
    if(0 != (nResult = pthread_attr_init(&nAttribs)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_init()");
    if(0 != (nResult = pthread_attr_setdetachstate(&nAttribs, PTHREAD_CREATE_DETACHED)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_setdetachstate()");
    if(0 != (nResult = pthread_create(&nTask->mThreadID, &nAttribs, TCPFwdLookupUnix::ThreadForwardLookup, (void*) nTask)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_create()");
    else
        nTask->mThreadAlive = true;
    
    //Done
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::AsynchReverseLookup
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
//
//   Note:: Just threads the synchronous ReverseLookup call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPRevLookup* TCPManagerUnix::AsyncReverseLookup(const STRING& ipAddr)
{
    TCPRevLookupUnix*   nTask = new TCPRevLookupUnix();
    pthread_attr_t      nAttribs;
    INT                 nResult;
    
    
    //** Setup Data **//
    nTask->mIPAddr = ipAddr;


    //** Create Thread **//
    if(0 != (nResult = pthread_attr_init(&nAttribs)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_init()");
    if(0 != (nResult = pthread_attr_setdetachstate(&nAttribs, PTHREAD_CREATE_DETACHED)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_setdetachstate()");
    if(0 != (nResult = pthread_create(&nTask->mThreadID, &nAttribs, TCPRevLookupUnix::ThreadReverseLookup, (void*) nTask)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_create()");
    else
        nTask->mThreadAlive = true;
    
    //Done
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::AsyncOpenConnection
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
//
//   Note:: Uses POSIX connect() on a non-blocking socket, no threading.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPOpenConnect* TCPManagerUnix::AsyncOpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedAsyncOpenConnection(hAddr, hPort);

    TCPOpenConnectUnix* nTask = new TCPOpenConnectUnix();

    nTask->mPort            = hPort;
    nTask->mUnresolvedAddr  = hAddr;
    nTask->mFwdLookup       = AsyncForwardLookup(hAddr);
    nTask->mState           = TCPOpenConnectUnix::sLookup;
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::AsyncRecieveConnection
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

TCPRecvConnect* TCPManagerUnix::AsyncRecieveConnection(SHORT onPort)
{
    TCPRecvConnectUnix* nTask = new TCPRecvConnectUnix();
    pthread_attr_t      nAttribs;
    INT                 nResult;
    
    
    //** Setup Data **//
    nTask->mPort = onPort;


    //** Create Thread **//
    if(0 != (nResult = pthread_attr_init(&nAttribs)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_init()");
    if(0 != (nResult = pthread_attr_setdetachstate(&nAttribs, PTHREAD_CREATE_DETACHED)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_attr_setdetachstate()");
    if(0 != (nResult = pthread_create(&nTask->mThreadID, &nAttribs, TCPRecvConnectUnix::ThreadRecieveConnect, (void*) nTask)))
        ThrowTCP(kProblemTCP_Network, nResult, "pthread_create()");
    else
        nTask->mThreadAlive = true;
    
    //Done
    return nTask;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::SetBlocking
//
//   Desc:: Utility function. Makes the socket operate in blocking mode. Will
//          throw an exception if it failed.
//
//  Input:: The socket.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerUnix::SetBlocking(SOCKET nSocket)
{
    INT flags;

    flags = fcntl(nSocket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    if(fcntl(nSocket, F_SETFL, flags) == -1)
        ThrowTCP(kProblemTCP_Network, errno, ErrorStr("SetBlocking:fcntl(~O_NONBLOCK)"));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerUnix::SetNonBlocking
//
//   Desc:: Utility function. Makes the socket operate in non-blocking mode.
//          Will throw an exception if it failed.
//
//  Input:: The socket.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerUnix::SetNonBlocking(SOCKET nSocket)
{
    INT flags;

    flags = fcntl(nSocket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if(fcntl(nSocket, F_SETFL, flags) == -1)
        ThrowTCP(kProblemTCP_Network, errno, ErrorStr("SetNonBlocking:fcntl(O_NONBLOCK)"));
}



#if 0
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//###########################################################################//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#pragma mark -
#endif



TCPFwdLookupUnix::TCPFwdLookupUnix()
{
    mThreadID       = 0;
    mThreadExc      = NULL;
    mThreadAlive    = false;
}



TCPFwdLookupUnix::~TCPFwdLookupUnix()
{   
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupUnix::Cancel
//
//   Desc:: Cancels an asyncronous TCPFwdLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: There may be memory leaks incured when SIGKILLing gethostbyname.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupUnix::Cancel()
{
    if(mWasCanceled || mDone) return;
    
    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThreadAlive)
    {
        //pthread_kill(mThreadID, SIGKILL); //doesnt exist on BSD unixes
        pthread_cancel(mThreadID);
        mThreadAlive = false;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupUnix::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupUnix::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupUnix::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPFwdLookupUnix::Results()
{
    ASSERT(!mGotResults);
    
    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mIPAddr;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupUnix::ThreadForwardLookup
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous forward lookup.
//
//  Input:: A void ptr to the TCPFwdLookupUnix task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void* TCPFwdLookupUnix::ThreadForwardLookup(void* nFwdLookup)
{
    TCPFwdLookupUnix* nThis = (TCPFwdLookupUnix*) nFwdLookup;

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
    
    return NULL;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



TCPRevLookupUnix::TCPRevLookupUnix()
{
    mThreadID       = 0;
    mThreadExc      = NULL;
    mThreadAlive    = false;
}



TCPRevLookupUnix::~TCPRevLookupUnix()
{
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupUnix::Cancel
//
//   Desc:: Cancels an asyncronous TCPRevLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: There may be memory leaks incured when SIGKILLing gethostbyaddr.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupUnix::Cancel()
{
    if(mWasCanceled || mDone) return;
    
    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThreadAlive)
    {
        //pthread_kill(mThreadID, SIGKILL); //doesnt exist on BSD unixes
        pthread_cancel(mThreadID);
        mThreadAlive = false;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupUnix::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupUnix::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupUnix::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPRevLookupUnix::Results()
{
    ASSERT(!mGotResults);
    
    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mHostName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupUnix::ThreadReverseLookup
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous reverse lookup.
//
//  Input:: A void ptr to the TCPRevLookupUnix task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void* TCPRevLookupUnix::ThreadReverseLookup(void* nRevLookup)
{
    TCPRevLookupUnix* nThis = (TCPRevLookupUnix*) nRevLookup;

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
    
    return NULL;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



TCPOpenConnectUnix::TCPOpenConnectUnix()
{
    mState          = sError;
    mFwdLookup      = NULL;
    mMadeSocket     = false;
    mSocket         = 0;
    mRefused        = false;
}



TCPOpenConnectUnix::~TCPOpenConnectUnix()
{
    if(mFwdLookup)
    {
        mFwdLookup->Cancel();
        delete mFwdLookup;
    }
    
    if(mMadeSocket)
        close(mSocket);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectUnix::Cancel
//
//   Desc:: Cancels an asyncronous TCPOpenConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectUnix::Cancel()
{
    if(mWasCanceled || mDone) return;

    mDone           = true;
    mWasCanceled    = true;
    mState          = sIdle;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectUnix::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectUnix::Process()
{
    if(mState == sIdle) 
        return;
    if(mState == sError)
        ThrowTCPMsg(kProblemTCP_OpenConnect, kFailedTaskOpenConnect);
        
    
    //** Asynchronously Lookup The IP Address **//
    if(mState == sLookup)
    {
        mFwdLookup->Process();
        if(mFwdLookup->Done())
        {
            mIPAddress  = mFwdLookup->Results();
            mState      = sConnect;
            return;
        }
    }
    
    
    //** Connect To The Machine **//
    if(mState == sConnect)
    {
        if(!mMadeSocket)
        {
            //Make a socket
            mSocket = socket(AF_INET, SOCK_STREAM, 0);
            if(mSocket == -1)
                ThrowTCP(kProblemTCP_OpenConnect, errno, ErrorStr("Socket()"));
            mMadeSocket = true;
            TCPManagerUnix::SetNonBlocking(mSocket);
        
            //Setup address
            memset(&mDestAddr, 0, sizeof(struct sockaddr_in));
            mDestAddr.sin_family        = AF_INET;
            mDestAddr.sin_port          = htons(mPort);
            mDestAddr.sin_addr.s_addr   = inet_addr(mIPAddress.c_str());
            
            //Start non-blocking connect
            INT nResult;
            nResult = connect(mSocket, (struct sockaddr*)&mDestAddr, sizeof(struct sockaddr));
            if(nResult < 0 && errno != EINPROGRESS) //Error
            {
                ThrowTCP(kProblemTCP_OpenConnect, errno, ErrorStr("connect(START)"));
            }else 
            if(nResult == 0) //Connected right away, woohoo!
            {
                mState  = sIdle;
                mDone   = true;
                return;
            }
        }
        
        
        //** Query Connecting Socket **//
        static struct timeval noWait;
        fd_set inSet, outSet, excSet;
        
        FD_ZERO(&inSet);
        FD_ZERO(&outSet);
        FD_ZERO(&excSet);
        FD_SET(mSocket, &inSet);
        FD_SET(mSocket, &outSet);
        FD_SET(mSocket, &excSet);       
        if(select(mSocket+1, &inSet, &outSet, &excSet, &noWait) == -1)
            ThrowTCP(kProblemTCP_OpenConnect, errno, ErrorStr("Select()"));
        
        if(FD_ISSET(mSocket, &excSet))
            ThrowTCPMsg(kProblemTCP_OpenConnect, "Exception on non-blocking connecting socket.");
        
        //Any in/out set action counts as a connection
        if(!(FD_ISSET(mSocket, &inSet) || FD_ISSET(mSocket, &outSet)))
            return;
        
        
        //** Finish Connection **//
        INT nResult;
        nResult = connect(mSocket, (struct sockaddr*)&mDestAddr, sizeof(struct sockaddr));
        if(nResult != 0 && (errno == ECONNREFUSED || errno == ETIMEDOUT))
        {
            //Refused connection, dispose of socket
            close(mSocket);     
            mMadeSocket = false;
            mRefused    = true;
            mState      = sIdle;
            mDone       = true;
            return;
        }else
        if(nResult != 0)
            ThrowTCP(kProblemTCP_OpenConnect, errno, ErrorStr("connect(FINISH)"));
    
        mState  = sIdle;
        mDone   = true;
        return;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectUnix::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPOpenConnectUnix::Results()
{
    ASSERT(!mGotResults);
    if(!mMadeSocket || mRefused)
    {
        mGotResults = true;
        return NULL;
    }
    
    //** Setup & Add New TCPLink **//
    STRING      iAddr       = mIPAddress;
    USHORT      iPort       = mPort;
    TCPLinkUnix *nUnixLink  = new TCPLinkUnix(mSocket, iAddr, iPort);
    
    
    mMadeSocket = false; //So the socket isn't closed in the destructor
    mGotResults = true;

    TCPManagerUnix::SetBlocking(mSocket);
    TCPManager::Get()->Add(nUnixLink);
    return nUnixLink;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



TCPRecvConnectUnix::TCPRecvConnectUnix()
{
    mResultLink     = NULL;
    mThreadID       = 0;
    mThreadExc      = NULL;
    mThreadAlive    = false;
}



TCPRecvConnectUnix::~TCPRecvConnectUnix()
{   
    Cancel();

    //Destroy unthrown exception
    if(mThreadExc)
        delete mThreadExc;
        
    //Destroy unretrieved TCPLink
    if(mResultLink && !mGotResults)
        delete mResultLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectUnix::Cancel
//
//   Desc:: Cancels an asyncronous TCPRecvConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//
//   Note:: Again, this may incur a tiny memory leak inside the POSIX functions
//          when it is SIGKILLed from outside.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRecvConnectUnix::Cancel()
{
    if(mWasCanceled || mDone) return;
    
    mDone           = true;
    mWasCanceled    = true;

    //Destroy thread
    if(mThreadAlive)
    {
        //pthread_kill(mThreadID, SIGKILL); //doesnt exist on BSD unixes
        pthread_cancel(mThreadID);
        mThreadAlive = false;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectUnix::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRecvConnectUnix::Process()
{
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectUnix::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: NULL if no connections were waiting. Otherwise it returns a TCPLink
//          to the new connection and adds it to the TCPManager list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPRecvConnectUnix::Results()
{
    ASSERT(!mGotResults);
    
    //Return results or exception
    if(mThreadExc)
        NullifyAndThrow(mThreadExc);

    mGotResults = true;
    return mResultLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectUnix::ThreadRecieveConnect
//
//   Desc:: Utility function. Entry point for the thread that will do the
//          asynchronous connection accepting.
//
//  Input:: A void ptr to the TCPRecvConnectUnix task.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void* TCPRecvConnectUnix::ThreadRecieveConnect(void* nRecvConnect)
{
    TCPRecvConnectUnix* nThis = (TCPRecvConnectUnix*) nRecvConnect;

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
    
    return NULL;
}




#endif //(TARGET_UNIX)
