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
#include "CommonTypesCW.h"
#if(TARGET_MAC)
#include "TCPManagerMac.h"
#include "TCPLinkMac.h"
#include "Events.h"



//Mac includes
#include <Gestalt.h>    //For Checking OT Version w/ Gestalt
#include <Resources.h>  //For ReleaseResource
#include <TextUtils.h>  //For GetString
#include <Memory.h>     //For HLock
#include <Threads.h>    //For YieldToAnyThread

static OTResult OT_SetFourByteOption(EndpointRef ep, OTXTILevel level, OTXTIName name, UInt32 value);
static OTResult OT_GetFourByteOption(EndpointRef ep, OTXTILevel level, OTXTIName name, UInt32 *value);




//
// Patch MacOS 9.2's bugged Open Transport cleanup code?
//
#define MACOS92_OPENTRANSPORTCLOSE_FIX 1
//
// Semi-working fix to attempt calling CloseOpenTransportInContext() like it needs to
// be called to deallocate all of the listening endpoints and other resources. MacOS
// used to do this for you if you crashed or exited without shutting down but theres
// barely sort of workaround to the current state b/c the 9.2 MacOS does no cleanup.
//
#if(MACOS92_OPENTRANSPORTCLOSE_FIX)
    class Terminate_MacOS_9_2_Fix_Installer
    {
        public: Terminate_MacOS_9_2_Fix_Installer() {}
        public: ~Terminate_MacOS_9_2_Fix_Installer()
        {
            TCPManager::Get()->Shutdown();
        }
    };
    Terminate_MacOS_9_2_Fix_Installer gMacOS_9_2_Fix;
#endif




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
        sTCPManager =  new TCPManagerMac();
        
    return sTCPManager;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPManagerMac::TCPManagerMac()
{
    mOTInited       = false;
    mOTInetServices = NULL;
}




TCPManagerMac::~TCPManagerMac()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::Initialize
//
//   Desc:: Specific initialization code for each OS specific TCPManager. It
//          must also call the superclass initialization code.
//
// Output:: True if TCP services could be initialized.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerMac::Initialize()
{
    if(mOTInited)
        return true;

    if(!TCPManager::Initialize())
        return false;

    
    //** Initialize Open Transport Services **//
    OSErr                   nErr;
    OSStatus                nResult;
    ULONG                   nOTVersion;
    OTInitializationFlags   nFlags = kInitOTForApplicationMask;


    //Check if Open Transport is installed
    nErr = Gestalt('otvr', (LONG*) &nOTVersion);
    if(nErr != noErr || nOTVersion < 0x01130000)
        ThrowTCPMsg(kProblemTCP_NoNetwork, "OpenTransport 1.1.3 or greater is not installed.");
    
    //Initialize Open Transport
    nResult = InitOpenTransportInContext(nFlags, NULL);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_NoNetwork, nResult, "InitOpenTransportInContext()");


    //Get a service reference for DNS resolving
    mOTInetServices = OTOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, &nResult, NULL);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_NoNetwork, nResult, "OTOpenInternetServicesInContext()");

    
    //Worked
    mOTInited = true;
    return mOTInited;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::Shutdown
//
//   Desc:: Specific shutdown code for each OS specific TCPManager. It must
//          also call the superclass shutdown code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerMac::Shutdown()
{
    TCPManager::Shutdown();
    
    //** Mac Shutdown Code **//
    if(!mOTInited)
        return;


    //Close down our Open Transport DNS/Inet services
    OTCloseProvider(mOTInetServices);

    
    //Shutdown all our Open Transport services
    CloseOpenTransportInContext(NULL);
    mOTInited = false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::ForwardLookup
//
//   Desc:: Will lookup the 128.100.12.90 numbered address corresponding to
//          to the given hostname. It blocks unitl it's done or throws an
//          exception if it errors.
//
//  Input:: The hostname to lookup.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerMac::ForwardLookup(const STRING& hostName)
{
    char*           nHostName = (char*) hostName.c_str();
    InetHostInfo    nHostInfo;
    OSStatus        nErr;
    
    
    nErr = OTInetStringToAddress(mOTInetServices, nHostName, &nHostInfo);
    if(nErr != noErr)
        ThrowTCP(kProblemTCP_InvalidHostname, nErr, hostName);


    return UnstuffIP(nHostInfo.addrs[0]);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::ReverseLookup
//
//   Desc:: Will lookup the hostname corresponding to a 128.100.12.90 numbered 
//          address. Returns an empty string if there is no hostname associated
//          with the ip.
//
//  Input:: The IP address to reverse lookup.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerMac::ReverseLookup(const STRING& ipAddr)
{
    InetDomainName  nHostNameBuf;
    InetHost        nDotDotNum;
    OSStatus        nResult;

        
    
    nDotDotNum  = StuffIP(ipAddr);
    nResult     = OTInetAddressToName(mOTInetServices, nDotDotNum, nHostNameBuf);
    if(nResult != noErr)
        return "";
    
    //Fully quailified domain names have a period at the end, according to apple's
    //site. So yank it off.
    
    if(nHostNameBuf[strlen(nHostNameBuf)-1] == '.')
        nHostNameBuf[strlen(nHostNameBuf)-1] = '\0';
    
    return nHostNameBuf;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::GetMachineIP
//
//   Desc:: Returns the IP address of the machine this code is running on.
//
// Output:: The IP address in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerMac::GetMachineIP()
{
    InetInterfaceInfo   nMachineInfo;
    OSStatus            nResult;
    
    
    //Request info
    nResult = OTInetGetInterfaceInfo(&nMachineInfo, kDefaultInetInterface);
    if(nResult != noErr)
            ThrowTCP(kProblemTCP_NoMachineIP, nResult, "OTInetGetInterfaceInfo()");

    return UnstuffIP(nMachineInfo.fAddress);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::GetMachineName
//
//   Desc:: Returns the "name" of the machine this code is running on.
//
// Output:: The machine name in a STRING object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManagerMac::GetMachineName()
{   
    //** Get AppleTalk Machine Name **//
    #define kMacUserNameID  -16096
    #define kMacCompNameID  -16413
    StringHandle            nComputerNameHandle;
    static bool             nGotName = false;
    static CHAR             nComputerName[1024];        
    
    
    //** Use Cached Copy **//
    if(nGotName)
        return nComputerName;
        
    
    //** Get Computer Name String **//
    //Get String and Lock It
    if(!(nComputerNameHandle = GetString(kMacCompNameID)))
        ThrowTCPMsg(kProblemTCP_NoMachineName, "GetString(-16413)");
    HLock((Handle) nComputerNameHandle);
    //Brandon was here


    //Copy P-String Into a C-String
    memcpy(nComputerName, &nComputerNameHandle[0][1], nComputerNameHandle[0][0]);
    nComputerName[nComputerNameHandle[0][0]] = '\0';


    //Unlock String and Release It
    HUnlock((Handle) nComputerNameHandle);
//  ReleaseResource((Handle) nComputerNameHandle);
//  if(ResError()  != noErr)
//      ThrowTCP(kProblemTCP_NoMachineName, ResError(), "ReleaseResource()");

    nGotName = true;
    return nComputerName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::StartListening
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

bool TCPManagerMac::StartListening(SHORT onPort, SHORT maxQueue)
{
    //** Already Listening? **//
    if(mPortEndpointMap.find(onPort) != mPortEndpointMap.end())
        return true;



    //** Open Transport Listening Code **//
    OTConfigurationRef  nConf;
    OSStatus            nResult;
    EndpointRef         nEndpoint;
    
    
    
    //Create special listening endpoint
    nConf = OTCreateConfiguration("tilisten,tcp");
    nEndpoint = OTOpenEndpointInContext(nConf, 0, NULL, &nResult, NULL);
    if(nResult != kOTNoError)
        ThrowTCP(kProblemTCP_Listen, nResult, "OTOpenEndpointInContext()"); 
    SetSynch(nEndpoint);
    nResult = OT_SetFourByteOption(nEndpoint, INET_IP, IP_REUSEADDR, true);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_Listen, nResult, "SetFourByteOption(INET_IP,IP_REUSEADDR)");   


    //Bind endpoint to our address & port
    TBind               nBindReq;
    InetAddress         nAddr;

    nAddr.fAddressType  = AF_INET;
    nAddr.fPort         = onPort;
    nAddr.fHost         = 0;
    
    nBindReq.addr.len   = sizeof(InetAddress);
    nBindReq.addr.buf   = (unsigned char*) &nAddr;
    nBindReq.qlen       = maxQueue; //Max connection queue

    nResult = OTBind(nEndpoint, &nBindReq, NULL);
    if(nResult != kOTNoError)
    {
        OTCloseProvider(nEndpoint);
        ThrowTCP(kProblemTCP_Listen, nResult, "OTBind()");  
    }
    
    //Note:
    //With Unix sockets you would listen here and you could poll
    //your descriptor later to see if someone connected. With mac
    //you poll your socket/descriptor/endpoint FIRST, if it gets a 
    //T_LISTEN event you THEN "listen" to the socket and get the
    //connection. All that is in IncomingConnection and 
    //RecieveConnection
    
    
    //Add the address to our PortEndpointMap
    mPortEndpointMap[onPort] = nEndpoint;   
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::StopListening
//
//   Desc:: If the machine was listening on onPort it will stop listening an
//          discard any connections that happened to be waiting on the port. It
//          returns false if it wasn't listening on that port.
//
//  Input:: onPort: the port number to stop listening on.
//
// Output:: True if it success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerMac::StopListening(SHORT onPort)
{
    //** Close All Endpoints **//
    if(onPort == kPortsAll)
    {
        map<USHORT, EndpointRef>::iterator nScan, nScanNext;
        
        for(nScan = mPortEndpointMap.begin(); nScan != mPortEndpointMap.end(); nScan = nScanNext)
        {
            nScanNext = nScan;  //Store the next iterator cause the current one will
            nScanNext++;        //be erased after the StopListening call.
            StopListening((*nScan).first);
        }
        return true;
    }
    

    //** Are We Even Listening? **//
    if(mPortEndpointMap.find(onPort) == mPortEndpointMap.end())
        return false;




    //** Open Transport Stop Listening Code **//
    EndpointRef nEndpoint   = mPortEndpointMap[onPort]; 
    OSStatus    nResult;
    
    
    
    //Refuse any incoming connections
    //It is not assumed ANY of these exceptions will ever be thrown if the
    //code is perfect, so they don't do full cleanups before the throw.
    while(IncomingConnections(onPort))
    {
        TCall   nCall;
        
        nResult = OTListen(nEndpoint, &nCall);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_Listen, nResult, "OTListen()");    
            
        nResult = OTSndDisconnect(nEndpoint, &nCall);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_Listen, nResult, "OTSndDisconnect()"); 
    }
    

    //Unbind & destroy endpoint
    nResult = OTUnbind(nEndpoint);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_Listen, nResult, "OTUnbind()");    
    
    nResult = OTCloseProvider(nEndpoint);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_Listen, nResult, "OTCloseProvider()"); 
    

    //Remove from our PortEndpoint map
    mPortEndpointMap.erase(onPort);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::IncomingConnections
//
//   Desc:: Checks if there are any incoming connections on the given port. If
//          the kPortsAll default value is used then it will check if there
//          are any connections waiting on any of the listening ports.
//
//  Input:: onPort = kPortsAll: the port number to check.
//
// Output:: True if there are connections waiting.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManagerMac::IncomingConnections(SHORT onPort)
{
    //** Check All Endpoints **//
    if(onPort == kPortsAll)
    {
        map<USHORT, EndpointRef>::iterator nScan;
        
        for(nScan = mPortEndpointMap.begin(); nScan != mPortEndpointMap.end(); nScan++)
        if(IncomingConnections((*nScan).first))
            return true;

        return false;
    }


    //Are we even listening there?
    if(mPortEndpointMap.find(onPort) == mPortEndpointMap.end())
        return false;


    EndpointRef nEndpoint   = mPortEndpointMap[onPort]; 
    OSStatus    nResult;


    nResult = OTLook(nEndpoint);
    if(nResult < 0)
        ThrowTCP(kProblemTCP_Poll, nResult, "OTLook()");
    
    return nResult == T_LISTEN ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::RecieveConnection
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

TCPLink* TCPManagerMac::RecieveConnection(SHORT onPort)
{
    //** Accept A Connection From Any Port **//
    if(onPort == kPortsAll)
    {
        map<USHORT, EndpointRef>::iterator nScan;
        
        for(nScan = mPortEndpointMap.begin(); nScan != mPortEndpointMap.end(); nScan++)
        if(IncomingConnections((*nScan).first))
            return RecieveConnection((*nScan).first);
            
        return NULL;
    }
    

    
    //** Are We Listening? **//
    if(mPortEndpointMap.find(onPort) == mPortEndpointMap.end())
        return NULL;



    //** Acception Code **//
    OTConfigurationRef  nConf       = OTCreateConfiguration(kTCPName);
    EndpointRef         nEndpoint   = mPortEndpointMap[onPort];
    EndpointRef         nAcceptedEndpoint;
    TCall               nCall;
    InetAddress         nInetAddr;
    OSStatus            nResult;

    
    OTMemzero(&nCall, sizeof(TCall));
    nCall.addr.buf      = (UCHAR*) &nInetAddr;
    nCall.addr.maxlen   = sizeof(InetAddress);
    
    
    nResult = OTListen(nEndpoint, &nCall);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_AcceptConnect, nResult, "OTListen()");
    
    nAcceptedEndpoint = OTOpenEndpointInContext(nConf, 0, NULL, &nResult, NULL);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_AcceptConnect, nResult, "OTOpenEndpointInContext()");
    nResult = OT_SetFourByteOption(nEndpoint, XTI_GENERIC, XTI_RCVLOWAT, 1);
    if(nResult != noErr)
        ThrowTCP(kProblemTCP_Listen, nResult, "SetFourByteOption(XTI_GENERIC,XTI_RCVLOWAT)");   
    
    
    nResult = OTAccept(nEndpoint, nAcceptedEndpoint, &nCall);
    if(nResult != noErr)
    {
        if(OTLook(nEndpoint) == T_DISCONNECT)
            OTRcvDisconnect(nEndpoint, NULL);

        if(OTLook(nAcceptedEndpoint) == T_DISCONNECT)
            OTRcvDisconnect(nAcceptedEndpoint, NULL);

        OTCloseProvider(nAcceptedEndpoint);
        ThrowTCP(kProblemTCP_AcceptConnect, nResult, "OTAccept()");
    }

    
    //** Setup & Add New TCPLink **//
    TCPLinkMac  *nMacLink = new TCPLinkMac(nAcceptedEndpoint, UnstuffIP(nInetAddr.fHost), onPort);
    
    SetSynch(nAcceptedEndpoint);
    Add(nMacLink);
    return nMacLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::OpenConnection
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

TCPLink* TCPManagerMac::OpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedOpenConnection(hAddr, hPort);

    #define kConnAccepted   kOTNoError
    #define kConnRejected   kOTLookErr

    TCall               nPeerRequest;
    InetAddress         nPeerAddr;
    EndpointRef         nEndpoint;
    OTConfigurationRef  nConf;
    OSStatus            nResult;
    STRING              nIP;
    
    

    //Find Address
    nIP                     = ForwardLookup(hAddr);
    nPeerAddr.fAddressType  = AF_INET;
    nPeerAddr.fHost         = StuffIP(nIP);
    nPeerAddr.fPort         = hPort;

    
    //Setup address in TCall structure
    OTMemzero(&nPeerRequest, sizeof(TCall));
    nPeerRequest.addr.buf   = (UCHAR*) &nPeerAddr;
    nPeerRequest.addr.len   = sizeof(InetAddress);
        
    

    //Create the endpoint
    nConf       = OTCreateConfiguration(kTCPName);
    nEndpoint   = OTOpenEndpointInContext(nConf, 0, NULL, &nResult, NULL);
    if(nResult != kOTNoError)
        ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTOpenEndpointInContext()");    
    SetSynch(nEndpoint);
    
    
    //Bind to local machine
    nResult = OTBind(nEndpoint, NULL, NULL);
    if(nResult != kOTNoError)
        ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTBind()"); 
    

    //Attempt to connect
    nResult = OTConnect(nEndpoint, &nPeerRequest, NULL);
    if(nResult != kOTCanceledErr && nResult != kConnRejected && nResult != kConnAccepted)
        ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTConnect(), Unexpected result.");
    
    //Connection rejected or failed
    if(nResult != kConnAccepted)
    {
        //Was rejected by server, otherwise nothing came back
        if(nResult == kConnRejected)
        {
            nResult = OTRcvDisconnect(nEndpoint, NULL);
            if(nResult != noErr)
            {
                OTCloseProvider(nEndpoint);
                ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTRcvDisconnect()");
            }
        }
                
        nResult = OTCloseProvider(nEndpoint);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTCloseProvider()");    

        return NULL;
    }

    //Connection accepted

    
    //** Add & Return CLink **//
    TCPLinkMac  *nMacLink = new TCPLinkMac(nEndpoint, nIP, hPort);
    
    SetSynch(nEndpoint);
    Add(nMacLink);
    return nMacLink;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::AsynchForwardLookup
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

TCPFwdLookup* TCPManagerMac::AsyncForwardLookup(const STRING& hostName)
{
    TCPFwdLookupMac*    nTask;

    nTask               = new TCPFwdLookupMac();
    nTask->mHostName    = hostName;
    nTask->mState       = TCPFwdLookupMac::sGetService;
    
    Add(nTask);
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::AsynchReverseLookup
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

TCPRevLookup* TCPManagerMac::AsyncReverseLookup(const STRING& ipAddr)
{
    TCPRevLookupMac*    nTask;
    InetHost        nDotDotNum;

    nDotDotNum          = StuffIP(ipAddr);
    nTask               = new TCPRevLookupMac();
    nTask->mDotDotNum   = nDotDotNum;
    nTask->mState       = TCPRevLookupMac::sGetService;
    
    Add(nTask);
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::AsyncOpenConnection
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

TCPOpenConnect* TCPManagerMac::AsyncOpenConnection(const STRING& hAddr, SHORT hPort, bool hViaProxy)
{
    if(hViaProxy) return ProxiedAsyncOpenConnection(hAddr, hPort);

    TCPOpenConnectMac*  nTask;
    
    nTask                   = new TCPOpenConnectMac();
    nTask->mUnresolvedAddr  = hAddr;
    nTask->mPort            = hPort;
    nTask->mFwdLookup       = AsyncForwardLookup(hAddr);
    nTask->mState           = TCPOpenConnectMac::sLookup;

    Add(nTask);
    return nTask;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::AsyncRecieveConnection
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
//   Note:: Still uses blocking call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPRecvConnect* TCPManagerMac::AsyncRecieveConnection(SHORT onPort)
{
    TCPRecvConnectMac*  nTask;
    
    nTask           = new TCPRecvConnectMac();
    nTask->mPort    = onPort;
    nTask->mDone    = true;
    
    return nTask;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::SetAsynch
//
//   Desc:: Sets an endpoint to asynchronous mode and installs the notifier to
//          handle the asynchronous events.
//
//  Input:: nEP: the endpoint.
//          nNotif: the notification function ptr
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerMac::SetAsynch(ProviderRef nEP, OTNotifyProcPtr nNotif)
{
    OSStatus nResult;
    
    //Blocking
    if(!OTIsBlocking(nEP))
    if(noErr != (nResult = OTSetBlocking(nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTSetBlocking()");
    
    //Asynchronous
    if(OTIsSynchronous(nEP))
    if(noErr != (nResult = OTSetAsynchronous(nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTSetAsynchronous()");
    
    //With botifier
    OTRemoveNotifier(nEP);
    if(nNotif)
    if(noErr != (nResult = OTInstallNotifier(nEP, NewOTNotifyUPP(nNotif), nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTInstallNotifier()");
    
    //And no sync events
    if(noErr != (nResult = OTUseSyncIdleEvents(nEP, false)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTUseSyncIdleEvents()");
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::SetSynch
//
//   Desc:: Sets an endpoint to synchronous mode.
//
//  Input:: nEP: the endpoint.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPManagerMac::SetSynch(ProviderRef nEP)
{
    INT nResult;
    
    //Blocking
    if(!OTIsBlocking(nEP))
    if(noErr != (nResult = OTSetBlocking(nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTSetBlocking()");

    //Synchronous
    if(!OTIsSynchronous(nEP))
    if(noErr != (nResult = OTSetSynchronous(nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTSetSynchronous()");
    
    //Thread yielding notifier
    OTRemoveNotifier(nEP);
    if(noErr != (nResult = OTInstallNotifier(nEP, NewOTNotifyUPP(TCPManagerMac::YieldingNotifier), nEP)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTInstallNotifier()");
    
    //With sync events
    if(noErr != (nResult = OTUseSyncIdleEvents(nEP, true)))
        ThrowTCP(kProblemTCP_Network, nResult, "OTUseSyncIdleEvents()");
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManagerMac::YieldingNotifier
//
//   Desc:: This little jem will allow the MacOS the same degree of flexibility
//          that other preemptive systems get when running blocking/synchronous
//          sockets. That nothing else can go on in the app but at least the
//          rest of the machine can keep going. So all synchronous endpoints
//          get this notifier installed. I mean, if your app doesn't care that
//          it's blocking until whatever call completes, what does it care if
//          it gives up time to the MacOS while it waits?
//
//  Input:: Read the OT docs if you want to know.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pascal void TCPManagerMac::YieldingNotifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie)
{
    OSStatus nResult;

    switch(hCode) 
    {
        case kOTSyncIdleEvent:
            //EventRecord  ehWhoCares;
            //GetNextEvent(nullEvent, &ehWhoCares);
            nResult = YieldToAnyThread();
            if(nResult != noErr)
                ThrowTCP(kProblemTCP_Network, nResult, "YieldingNotifier():YieldToAnyThread()");
            break;

    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPFwdLookupMac::TCPFwdLookupMac()
{
    mState          = sError;
    mProvider       = NULL;
}




TCPFwdLookupMac::~TCPFwdLookupMac()
{
    if(mProvider)
        OTCloseProvider(mProvider);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupMac::Cancel
//
//   Desc:: Cancels an asyncronous TCPFwdLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupMac::Cancel()
{
    if(mWasCanceled || mGotResults) return;
    
    OTCloseProvider(mProvider);
    mProvider       = NULL;
    
    mDone           = true;
    mWasCanceled    = true;
    mState          = sIdle;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupMac::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPFwdLookupMac::Process()
{
    //Waiting for our Notifier to advance the state
    if(mState == sIdle) 
        return;
    if(mState == sError)
        ThrowTCPMsg(kProblemTCP_Network, kFailedTaskFwdLookup);
    

    //** Open Asynch Service Provider **//
    if(mState == sGetService)
    {
        OSStatus    nResult;
        
        
        mState = sIdle;
        nResult = OTAsyncOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, NewOTNotifyUPP(Notifier), this, NULL);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_Network, nResult, "OTAsyncOpenInternetServicesInContext()");
        return;
    }
    
    
    //** Request Asynch Reverse Lookup **//
    if(mState == sForLookup)
    {
        char*       nHost = (char*) mHostName.c_str();
        OSStatus    nResult;
        
        if(mProvider == NULL)
            ThrowTCPMsg(kProblemTCP_Network, "TCPFwdLookupMac: No provider!");
    
        mState = sIdle;
        nResult = OTInetStringToAddress(mProvider, nHost, &mHostInfo);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_InvalidHostname, nResult, mHostName);
        return;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupMac::Notifier
//
//   Desc:: Utility function. Asynchronous provider notifier function for the
//          provider doing the asynchronous forward lookup.
//
//  Input:: Read the OT docs if you want to know.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pascal void TCPFwdLookupMac::Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie)
{
    TCPFwdLookupMac*    nThis = (TCPFwdLookupMac*) hContextPtr;
    
    //** Get Provider **//
    if(hCode == T_OPENCOMPLETE)
    {
        if(hResult != noErr || hCookie == NULL)
        {
            nThis->mState = sError;
            return;
        }
        
        nThis->mProvider    = (InetSvcRef) hCookie;
        nThis->mState       = sForLookup;
        return;
    }
    

    //** Done Resolving **//
    else if(hCode == T_DNRSTRINGTOADDRCOMPLETE)
    {
        if(hCookie == NULL)
        {
            nThis->mState = sError;
            return;
        }

        nThis->mState   = sIdle;
        nThis->mDone    = true;
        return;
    }


    //** Bad **//
    else
    {
        nThis->mState = sError;
        return;     
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPFwdLookupMac::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The ip address stored in a STRING object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPFwdLookupMac::Results()
{
    ASSERT(!mGotResults);

    mGotResults = true;
    return TCPManagerMac::UnstuffIP(mHostInfo.addrs[0]);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPRevLookupMac::TCPRevLookupMac()
{
    mResultAddr[0]  = '\0';
    mProvider       = NULL;
    mState          = sError;
    mDotDotNum      = 0;
}




TCPRevLookupMac::~TCPRevLookupMac()
{
    if(mProvider)
        OTCloseProvider(mProvider);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupMac::Cancel
//
//   Desc:: Cancels an asyncronous TCPRevLookup task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupMac::Cancel()
{
    if(mWasCanceled || mGotResults) return;
    
    OTCloseProvider(mProvider);
    mProvider       = NULL;
    mResultAddr[0]  = '\0';
    
    mDone           = true;
    mWasCanceled    = true;
    mState          = sIdle;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupMac::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPRevLookupMac::Process()
{
    //Waiting for our Notifier to advance the state
    if(mState == sIdle) 
        return;
    if(mState == sError)
        ThrowTCPMsg(kProblemTCP_Network,  kFailedTaskRevLookup);
    

    //** Open Asynch Service Provider **//
    if(mState == sGetService)
    {
        OSStatus    nResult;
        
        
        mState = sIdle;
        nResult = OTAsyncOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, NewOTNotifyUPP(Notifier), this, NULL);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_Network, nResult, "OTAsyncOpenInternetServicesInContext()");
        return;
    }
    
    
    //** Request Asynch Reverse Lookup **//
    if(mState == sRevLookup)
    {
        OSStatus    nResult;
        
        if(mProvider == NULL)
            ThrowTCPMsg(kProblemTCP_Network, "TCPRevLookupMac: No provider!");
    
        mState = sIdle;
        nResult = OTInetAddressToName(mProvider, mDotDotNum, mResultAddr);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_Network, nResult, "OTInetAddressToName()");
        return;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupMac::Notifier
//
//   Desc:: Utility function. Asynchronous provider notifier function for the
//          provider doing the asynchronous reverse lookup.
//
//  Input:: Read the OT docs if you want to know.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pascal void TCPRevLookupMac::Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie)
{
    TCPRevLookupMac*    nThis = (TCPRevLookupMac*) hContextPtr;
    
    //** Get Provider **//
    if(hCode == T_OPENCOMPLETE)
    {
        if(hResult != noErr || hCookie == NULL)
        {
            nThis->mState = sError;
            return;
        }
        
        nThis->mProvider    = (InetSvcRef) hCookie;
        nThis->mState       = sRevLookup;
        return;
    }
    

    //** Done Resolving **//
    else if(hCode == T_DNRADDRTONAMECOMPLETE)
    {
        if(hCookie == NULL)
        {
            nThis->mState = sError;
            return;
        }

        //Remove the trailing period
        LONG addrLen = strlen(nThis->mResultAddr);
        if(addrLen && nThis->mResultAddr[addrLen-1] == '.')
            nThis->mResultAddr[addrLen-1] = '\0';

        nThis->mState   = sIdle;
        nThis->mDone    = true;
        return;
    }


    //** Bad **//
    else
    {
        nThis->mState = sError;
        return;     
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRevLookupMac::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: The hostname associated with the ip.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPRevLookupMac::Results()
{
    ASSERT(!mGotResults);
    
    mGotResults = true;
    return mResultAddr;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


TCPOpenConnectMac::TCPOpenConnectMac()
{
    mState          = sError;
    mFwdLookup      = NULL;
    mEndpoint       = NULL;
    mBound          = false;
    mConnecting = false;
    mConnected      = false;
    mRefused        = false;
    mErrorBuf[0]    = '\0';
}



TCPOpenConnectMac::~TCPOpenConnectMac()
{
    //Release memory
    if(mEndpoint)
        OTCloseProvider(mEndpoint);
        
    if(mFwdLookup)
        delete mFwdLookup;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectMac::Cancel
//
//   Desc:: Cancels an asyncronous TCPOpenConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectMac::Cancel()
{
    if(mWasCanceled) return;
    
    if(mFwdLookup)
    {
        mFwdLookup->Cancel();
        while(!mFwdLookup->Done())
            mFwdLookup->Process();
            
        delete mFwdLookup;
        mFwdLookup = NULL;
    }
    
    //Connected endpoint that's not been retrieved
    if(mEndpoint && (mConnecting || mConnected))
        OTSndDisconnect(mEndpoint, NULL);

    //Endpoint still bound to localhost
    if(mEndpoint && mBound)
        OTUnbind(mEndpoint);
    
    //Release memory
    if(mEndpoint)
        OTCloseProvider(mEndpoint);

    mConnecting = false;
    mConnected      = false;
    mBound          = false;
    mEndpoint       = NULL;
    
    mDone           = true;
    mWasCanceled    = true;
    mState          = sIdle;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectMac::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectMac::Process()
{
    if(mState == sIdle) 
        return;
    if(mState == sError)
    {
        if(mErrorBuf[0] != '\0')
            ThrowTCPMsg(kProblemTCP_Network, string("Connection error. "+string(mErrorBuf)));
        else
            ThrowTCPMsg(kProblemTCP_Network, kFailedTaskOpenConnect);
    }


    //** Continue Processing Lookup **//
    if(mState == sLookup)
    {
        mFwdLookup->Process();
        if(mFwdLookup->Done())
        {
            mIPAddress = mFwdLookup->Results();
            mState = sGetEndpoint;
        }
        return;
    }


    //** Open Asynch Endpoint **//
    if(mState == sGetEndpoint)
    {
        OTConfigurationRef  nConf = OTCreateConfiguration(kTCPName);
        OSStatus            nResult;
                
        mState = sIdle;
        nResult = OTAsyncOpenEndpointInContext(nConf, 0, NULL, NewOTNotifyUPP(Notifier), this, NULL);
        if(nResult != noErr)
            ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTAsyncOpenEndpointInContext()");
        return;
    }
    
    
    //** Bind To Local Machine **//
    if(mState == sBind)
    {
        OSStatus    nResult;
        
        
        //Bind to local machine
        mState = sIdle;
        nResult = OTBind(mEndpoint, NULL, NULL);
        if(nResult != kOTNoError)
            ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTBind()");         
    }
    
    
    //** Connect To The Host **//
    if(mState == sConnect)
    {
        OSStatus            nResult;


        //Setup address
        mPeerAddr.fAddressType  = AF_INET;
        mPeerAddr.fHost         = TCPManagerMac::StuffIP(mIPAddress);
        mPeerAddr.fPort         = mPort;

        
        //Setup address in TCall structure
        OTMemzero(&mPeerRequest, sizeof(TCall));
        mPeerRequest.addr.buf   = (UCHAR*) &mPeerAddr;
        mPeerRequest.addr.len   = sizeof(InetAddress);
        

        //Start the async connection attempt
        mState = sIdle;
        nResult = OTConnect(mEndpoint, &mPeerRequest, NULL);
        if(nResult != kOTNoDataErr)
            ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTConnect(), Unexpected result.");
        mConnecting = true;
    }
    
    
    //** Finish Connecting **//
    if(mState == sRcvConnect)
    {
        OSStatus    nResult;
        
                
        nResult = OTRcvConnect(mEndpoint, NULL);
        if(nResult != kOTNoError)
            ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTRcvConnect()");
        
        mConnecting = false;
        mConnected  = true;
        mDone       = true;
        return;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectMac::Notifier
//
//   Desc:: Utility function. Asynchronous endpoint notifier function for the
//          endpoint doing the asynchronous connect.
//
//  Input:: Read the OT docs if you want to know.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pascal void TCPOpenConnectMac::Notifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie)
{
    TCPOpenConnectMac*  nThis = (TCPOpenConnectMac*) hContextPtr;
    

    //** Got Endpoint **//
    if(hCode == T_OPENCOMPLETE)
    {
        if(hResult != noErr || hCookie == NULL)
        {
            nThis->mState = sError;
            return;
        }
        
        nThis->mEndpoint    = (EndpointRef) hCookie;
        nThis->mState       = sBind;
        return;
    }
    

    //** Binding Complete **//
    else if(hCode == T_BINDCOMPLETE)
    {
        if(OTGetEndpointState(nThis->mEndpoint) != T_IDLE)
        {
            nThis->mState = sError;
            return;
        }
        
        nThis->mBound   = true;
        nThis->mState   = sConnect;
        return;
    }
    
    
    //** Got Connection Response **//
    else if(hCode == T_CONNECT)
    {
        nThis->mState = sRcvConnect;
        return;
    }

    else if(hCode == T_DISCONNECT)
    {
        nThis->mConnecting  = false;
        nThis->mRefused     = true;
        nThis->mState       = sIdle;
        nThis->mDone        = true;
        return;     
    }
    
    
    //** Bad **//
    else
    {
        nThis->mState = sError;
        sprintf(nThis->mErrorBuf, "Unknown Event Code %d", hCode);
        return;     
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectMac::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPOpenConnectMac::Results()
{
    ASSERT(!mGotResults);


    //** Gather Results **//
    OTRemoveNotifier(mEndpoint);
    

    //Host Refused Connection, Don't set the endpoint to NULL cause we want it
    //disposed of in destructor
    if(mRefused)    
    {
        mGotResults = true;
        return NULL;
    }

    //Host Accepted
    OSStatus nResult;
    
    if(!OTIsSynchronous(mEndpoint))
    if(noErr != (nResult = OTSetSynchronous(mEndpoint)))
        ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTSetSynchronous()");
    
    if(OTIsBlocking(mEndpoint))
    if(noErr != (nResult = OTSetNonBlocking(mEndpoint)))
        ThrowTCP(kProblemTCP_OpenConnect, nResult, "OTSetNonBlocking()");
    

    //** You can only get the results ONCE **//
    TCPLinkMac  *nMacLink = new TCPLinkMac(mEndpoint, mIPAddress, mPort);   
    TCPManager::Get()->Add(nMacLink);
    
    mEndpoint   = NULL; //NULLify so destructor doesn't dispose it
    mGotResults = true;
    return nMacLink;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPRecvConnectMac::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: NULL if no connections were waiting. Otherwise it returns a TCPLink
//          to the new connection and adds it to the TCPManager list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPRecvConnectMac::Results()
{
    ASSERT(!mGotResults);
    
    //Haha we just use the blocking one cause this is pointless but might need
    //to be around if for some reasons connections start to block a long time
    //before they finish

    //** Gather Results **//
    mGotResults = true;
    return TCPManager::Get()->RecieveConnection(mPort);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


//
// Original source for these routines:
// http://developer.apple.com/techpubs/mac/NetworkingOT/NetworkingWOT-39.html
//
// Used to implement the same socket option SO_REUSEADDR that gets used on
// unix a lot. This is so the OS allows you to re-listen on a port you may have
// just been using before a crash, force-quit, etc.
//




static OTResult OT_GetFourByteOption(EndpointRef ep,
                           OTXTILevel level,
                           OTXTIName  name,
                           UInt32   *value)
{
   OTResult err;
   TOption  option;
   TOptMgmt request;
   TOptMgmt result;
   
   /* Set up the option buffer */
   option.len  = kOTFourByteOptionSize;
   option.level= level;
   option.name = name;
   option.status = 0;
   option.value[0] = 0;// Ignored because we're getting the value.

   /* Set up the request parameter for OTOptionManagement to point
    to the option buffer we just filled out */

   request.opt.buf= (UInt8 *) &option;
   request.opt.len= sizeof(option);
   request.flags= T_CURRENT;

   /* Set up the reply parameter for OTOptionManagement. */
   result.opt.buf  = (UInt8 *) &option;
   result.opt.maxlen = sizeof(option);
   
   err = OTOptionManagement(ep, &request, &result);

   if (err == noErr) {
      switch (option.status) 
      {
         case T_SUCCESS:
         case T_READONLY:
            *value = option.value[0];
            break;
         default:
            err = option.status;
            break;
      }
   }
            
   return (err);
}



static OTResult OT_SetFourByteOption(EndpointRef ep,
                           OTXTILevel level,
                           OTXTIName  name,
                           UInt32   value)
{
   OTResult err;
   TOption  option;
   TOptMgmt request;
   TOptMgmt result;
   
   /* Set up the option buffer to specify the option and value to
         set. */
   option.len  = kOTFourByteOptionSize;
   option.level= level;
   option.name = name;
   option.status = 0;
   option.value[0] = value;

   /* Set up request parameter for OTOptionManagement */
   request.opt.buf= (UInt8 *) &option;
   request.opt.len= sizeof(option);
   request.flags  = T_NEGOTIATE;

   /* Set up reply parameter for OTOptionManagement. */
   result.opt.buf  = (UInt8 *) &option;
   result.opt.maxlen  = sizeof(option);

   
   err = OTOptionManagement(ep, &request, &result);

   if (err == noErr) {
      if (option.status != T_SUCCESS) 
         err = option.status;
   }
            
   return (err);
}



#endif //(TARGET_MACOS)
