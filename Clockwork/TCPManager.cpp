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
#include "CommonTypesCW.h"
#include "TCPManager.h"
#include "TCPLink.h"
#include "EndianUtils.h"


TCPManager* TCPManager::sTCPManager = NULL;



void TCPManager::DeleteManager()
{
    if(sTCPManager != NULL)
    {
        TCPManager::Get()->Shutdown();
        delete sTCPManager;
        sTCPManager = NULL;
    }
}


TCPManager::TCPManager()
    : mProxyType(kProxyUseNone)
    , mSOCKSv5Prefs(NULL)
    , mSOCKSv4Prefs(NULL)
    , mHTTPProxyPrefs(NULL)
{
    NoProxyPrefs();
}



TCPManager::~TCPManager()
{
    NoProxyPrefs();
}


bool TCPManager::Initialize()
{
    return true;
}


void TCPManager::Shutdown()
{
    //** Close All Links (Sockets/Endpoints) **//
    {
        list<TCPLink*>::iterator nScan, nScanNext;
        for(nScan = mTCPLinks.begin(); nScan != mTCPLinks.end(); nScan = nScanNext)
        {
            TCPLink* nLink = *nScan;
            
            nScanNext = nScan;
            nScanNext++;
            
            nLink->Close();
            delete nLink;
        }
    }


    //** Close Forward Resolutions **//
    {
        list<TCPFwdLookup*>::iterator nScan, nScanNext;
        for(nScan = mForwardLookups.begin(); nScan != mForwardLookups.end(); nScan = nScanNext)
        {
            TCPFwdLookup* nLookup = *nScan;
            
            nScanNext = nScan;
            nScanNext++;
            
            nLookup->Cancel();
            delete nLookup;
        }
    }   


    //** Close Reserve Resolutions **//
    {
        list<TCPRevLookup*>::iterator nScan, nScanNext;
        for(nScan = mReverseLookups.begin(); nScan != mReverseLookups.end(); nScan = nScanNext)
        {
            TCPRevLookup* nLookup = *nScan;
            
            nScanNext = nScan;
            nScanNext++;
            
            nLookup->Cancel();
            delete nLookup;
        }
    }   


    //** Close Opening Connections **//
    {
        list<TCPOpenConnect*>::iterator nScan, nScanNext;
        for(nScan = mOpenConnections.begin(); nScan != mOpenConnections.end(); nScan = nScanNext)
        {
            TCPOpenConnect* nConnect = *nScan;
            
            nScanNext = nScan;
            nScanNext++;
            
            nConnect->Cancel();
            delete nConnect;
        }
    }
    

    //** Close Recieving Connections **//
    {
        list<TCPRecvConnect*>::iterator nScan, nScanNext;
        for(nScan = mRecieveConnections.begin(); nScan != mRecieveConnections.end(); nScan = nScanNext)
        {
            TCPRecvConnect* nConnect = *nScan;
            
            nScanNext = nScan;
            nScanNext++;
            
            nConnect->Cancel();
            delete nConnect;
        }
    }
    
    
    //** Stop Listening On All Ports **//
    StopListening();
}




void TCPManager::NoProxyPrefs()
{
    if(mSOCKSv5Prefs)
        delete mSOCKSv5Prefs, mSOCKSv5Prefs = NULL;
    if(mSOCKSv4Prefs)
        delete mSOCKSv4Prefs, mSOCKSv4Prefs = NULL;
    if(mHTTPProxyPrefs)
        delete mHTTPProxyPrefs, mHTTPProxyPrefs = NULL;

    mProxyType = kProxyUseNone;
}



void TCPManager::SetProxyPrefs(SOCKSv5Prefs& hPrefs)
{
    NoProxyPrefs(); //Clear old prefs
    mSOCKSv5Prefs   = new SOCKSv5Prefs(hPrefs);
    mProxyType      = kProxyUseSOCKSv5;
}



void TCPManager::SetProxyPrefs(SOCKSv4Prefs& hPrefs)
{
    NoProxyPrefs(); //Clear old prefs
    mSOCKSv4Prefs   = new SOCKSv4Prefs(hPrefs);
    mProxyType      = kProxyUseSOCKSv4;
}

void TCPManager::SetProxyPrefs(HTTPProxyPrefs& hPrefs)
{
    NoProxyPrefs(); //Clear old prefs
    mHTTPProxyPrefs = new HTTPProxyPrefs(hPrefs);
    mProxyType          = kProxyUseHTTP;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManager::StuffIP
//
//   Desc:: Stuff an IP into a LONG.
//
//  Input:: The IP as a STRING
//
// Output:: The IP as a ULONG
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
ULONG TCPManager::StuffIP(STRING hIPAddr)
{
    ULONG   a, b, c, d; 
    ULONG   nDotDotNum;
    
    //If you're really in the mood for more cryptic apple data structures
    //and inside mac docs you could look at OTInitDNSAddress which does
    //the same stuff
    
    
    if(sscanf(hIPAddr.c_str(), "%lu.%lu.%lu.%lu", &a, &b, &c, &d) != 4)
        ThrowTCPMsg(kProblemTCP_InvalidIP, hIPAddr);
    
    nDotDotNum =    (0xFF000000 & (a << 24)) |
                    (0x00FF0000 & (b << 16)) |
                    (0x0000FF00 & (c <<  8)) |
                    (0x000000FF & (d <<  0));

    return nDotDotNum;
}   




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManager::UnstuffIP
//
//   Desc:: Extract an IP from a LONG.
//
//  Input:: The IP as a ULONG
//
// Output:: The IP as a STRING
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING TCPManager::UnstuffIP(ULONG hDotDotNum)
{
    INT a = (BYTE) (0x000000FF & (hDotDotNum >> 24));
    INT b = (BYTE) (0x000000FF & (hDotDotNum >> 16));
    INT c = (BYTE) (0x000000FF & (hDotDotNum >> 8 ));
    INT d = (BYTE) (0x000000FF & (hDotDotNum >> 0 ));
    CHAR nDotDotBuf[200];
    
    sprintf(nDotDotBuf, "%d.%d.%d.%d", a, b, c, d);
    
    return string(nDotDotBuf);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPManager::IsInDottedIPForm
//
//   Desc:: Returns true if an address is in the dotted IP (192.14.1.4) form.
//
//  Input:: The address.
//
// Output:: True if in dotted IP form.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPManager::IsInDottedIPForm(STRING& hAddr)
{
    ULONG   a, b, c, d; 
    return sscanf(hAddr.c_str(), "%lu.%lu.%lu.%lu", &a, &b, &c, &d) == 4;
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Base64EncodeText
//
//   Desc:: Turns a normal healthy string into a Base64 encoded string. We need
//          this to authorize ourselves to HTTP web proxy servers. BAH!
//
//  Input:: hBase64: Holds resultant text.
//          hNormal: Holds normal string to be encoded.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Base64EncodeText(CHAR* hBase64, const CHAR* hNormal);
void Base64EncodeText(CHAR* hBase64, const CHAR* hNormal)
{
    static CHAR     sDTable[256];           //Encoding/Decoding table
    static bool     sInitedDTable = false;
    
    
    //** Setup Conversion Table **//
    if(!sInitedDTable)
    {
        SHORT i;
        for(i = 0; i < 26; i++)
        {
            sDTable[i]      = 'A'+i;
            sDTable[26+i]   = 'a'+i;
        }
        for(i = 0; i < 10; i++)
            sDTable[52+i] = '0'+i;

        sDTable[62] = '+';
        sDTable[63] = '/';
        sInitedDTable = true;
    }

    //** Encode The Text As Base 64 **//
    bool nDone  = false;
    LONG nIndex = 0, n64Index = 0;
    LONG nLen   = strlen(hBase64);
    
    while(!nDone)
    {
        CHAR nInGroup[3], nOutGroup[4], nChar;
        SHORT n;
        
        
        //Fill InGroup
        nInGroup[0] = nInGroup[1] = nInGroup[2] = 0;
        for(n = 0; n < 3; n++)
        {
            if(nIndex == nLen)
            {
                nDone = true;
                break;
            }
            nChar = hNormal[nIndex++];
            nInGroup[n] = nChar;
        }
    
        //Fill OutGroup
        if(n > 0) 
        {
            nOutGroup[0] = sDTable[nInGroup[0] >> 2];
            nOutGroup[1] = sDTable[((nInGroup[0] & 3) << 4) | (nInGroup[1] >> 4)];
            nOutGroup[2] = sDTable[((nInGroup[1] & 0xF) << 2) | (nInGroup[2] >> 6)];
            nOutGroup[3] = sDTable[nInGroup[2] & 0x3F];

            /* Replace characters in output stream with "=" pad
            characters if fewer than three characters were
            read from the end of the input stream. */
            if(n < 3) 
            {
                nOutGroup[3] = '=';
                if(n < 2)
                    nOutGroup[2] = '=';
            }
            
            for(SHORT i = 0; i < 4; i++)
                hBase64[n64Index++] = nOutGroup[i];
        }
    }

    //Finish off string
    hBase64[n64Index] = '\0';
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




TCPLink* TCPManager::ProxiedOpenConnection(const STRING& hAddr, SHORT hPort)
{
    //** Not Using A Proxy **//
    if(mProxyType == kProxyUseNone) 
        return OpenConnection(hAddr, hPort, false);
    
    
    //** Using Some Sort Of A Proxy **//
    TCPOpenConnect* nOpen = NULL;
    
    try
    {   //We use the AsyncOpen which will do proxy negotiation as well, this way you only
        //need to code the negotiation stuff once for the async way.
        if(!(nOpen = AsyncOpenConnection(hAddr, hPort)))
            return NULL;

        while(!nOpen->Done())
            nOpen->Process();
    }
    catch(...)
    {
        if(nOpen) delete nOpen;
        throw;
    }


    //Return Results
    TCPLink* nLink = nOpen->Results();
    delete nOpen;
    return nLink;
}





TCPOpenConnect* TCPManager::ProxiedAsyncOpenConnection(const STRING& hAddr, SHORT hPort)
{
    //** Not Using A Proxy **//
    if(mProxyType == kProxyUseNone) 
        return AsyncOpenConnection(hAddr, hPort, false);
    
    
    //** Using Some Sort Of A Proxy **//
    TCPOpenConnect* nProxiedOpen = NULL;
    
    try
    {
        switch(mProxyType)
        {
            case kProxyUseSOCKSv4:
                nProxiedOpen = new TCPOpenConnectSOCKS(mSOCKSv4Prefs, hAddr, hPort);
                break;
            case kProxyUseSOCKSv5:
                nProxiedOpen = new TCPOpenConnectSOCKS(mSOCKSv5Prefs, hAddr, hPort);
                break;
            case kProxyUseHTTP:
                nProxiedOpen = new TCPOpenConnectHTTP(mHTTPProxyPrefs, hAddr, hPort);
                break;
            default:
                ASSERT(!"Unhandled Proxy Type");
                break;
        }
    }
    catch(...)
    {
        if(nProxiedOpen) delete nProxiedOpen;
        throw;
    }
    
    return nProxiedOpen;
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectSOCKS::TCPOpenConnectSOCKS
//
//   Desc:: Constructor. Starts this asyncronous open connect using the SOCKS
//          version 5 protocol. Called internally when needed, users of this
//          code should not ever need to call this directly.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPOpenConnectSOCKS::TCPOpenConnectSOCKS(const SOCKSv5Prefs* hPrefs, const STRING& hAddr, SHORT hPort)
    : mDestPort(hPort)
    , mResolving(NULL)
    , mConnecting(NULL)
    , mLink(NULL)
    , mState(TCPOpenConnectSOCKS::kFindDest)
{
    ASSERT(hPrefs != NULL);
    mPrefs.mVers4       = false;
    mPrefs.mServer  = hPrefs->mServer;
    mPrefs.mPort        = hPrefs->mPort;
    mPrefs.mUsername    = hPrefs->mUsername;
    mPrefs.mPassword    = hPrefs->mPassword;
    mResolving = TCPManager::Get()->AsyncForwardLookup(hAddr);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectSOCKS::TCPOpenConnectSOCKS
//
//   Desc:: Constructor. Starts this asyncronous open connect using the SOCKS
//          version 4 protocol. Called internally when needed, users of this
//          code should not ever need to call this directly.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPOpenConnectSOCKS::TCPOpenConnectSOCKS(const SOCKSv4Prefs* hPrefs, const STRING& hAddr, SHORT hPort)
    : mDestPort(hPort)
    , mResolving(NULL)
    , mConnecting(NULL)
    , mLink(NULL)
    , mState(TCPOpenConnectSOCKS::kFindDest)
{
    ASSERT(hPrefs != NULL);
    mPrefs.mVers4       = true;
    mPrefs.mServer  = hPrefs->mServer;
    mPrefs.mPort        = hPrefs->mPort;
    mPrefs.mUsername    = hPrefs->mUsername;
    mResolving = TCPManager::Get()->AsyncForwardLookup(hAddr);
}




TCPOpenConnectSOCKS::~TCPOpenConnectSOCKS()
{
    Cancel();
    if(mLink)
        delete mLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectSOCKS::Cancel
//
//   Desc:: Cancels an asyncronous TCPOpenConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectSOCKS::Cancel()
{
    if(mWasCanceled)
        return;


    if(mResolving)
    {
        mResolving->Cancel();
        delete mResolving;
        mResolving = NULL;  
    }
    if(mConnecting) 
    {
        mConnecting->Cancel();
        delete mConnecting;
        mConnecting = NULL;
    }

    mDone           = true;
    mState          = TCPOpenConnectSOCKS::kIdle;
    mWasCanceled    = true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectSOCKS::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectSOCKS::Process()
{
    //** Lookup IP, Connect To SOCKS Server **//
    if(mState == TCPOpenConnectSOCKS::kFindDest)
    {
        ASSERT(mResolving != NULL);
        mResolving->Process();
        if(!mResolving->Done())
            return;
        
        STRING nIP = mResolving->Results();
        delete mResolving;
        mResolving = NULL;
        
        //Cram the IP into a 4 byte LONG
        mDestIP = (LONG) TCPManager::StuffIP(nIP);

        //Connect to SOCKS server now
        mState = TCPOpenConnectSOCKS::kConnecting;
        mConnecting = TCPManager::Get()->AsyncOpenConnection(mPrefs.mServer, mPrefs.mPort, false);
    }


    //** Finish Connecting, Send SOCKS Request (v4 & v5) **//
    else
    if(mState == TCPOpenConnectSOCKS::kConnecting)
    {
        ASSERT(mConnecting != NULL);
        mConnecting->Process();
        if(!mConnecting->Done())
            return;
            
        mLink = mConnecting->Results();
        delete mConnecting;
        mConnecting = NULL;
        
        if(!mLink)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Down, "Proxy server not responding");
        
        //Network byte order (Big Endian)
        mDestIP     = HostLongToBE(mDestIP);
        mDestPort   = HostShortToBE(mDestPort);
        
        if(mPrefs.mVers4)
            mState = TCPOpenConnectSOCKS::kSendV4Request;
        else
            mState = TCPOpenConnectSOCKS::kSendV5DetermineAuth;
        
        Process(); //Send Request Before We Stop Processing
        return;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    // SOCKS Version 4 Negotiation Code
    ////////////////////////////////////////////////////////////////////////////

    //** Send First Request v4 **//
    else
    if(mState == TCPOpenConnectSOCKS::kSendV4Request)
    {
        CHAR    nPacket[266];
        LONG    nPacketLength, nWrote;
        

        nPacketLength = 8+mPrefs.mUsername.length()+1;

        nPacket[0]          = '\x04';           /* SOCKS Version        */
        nPacket[1]          = '\x01';           /* 1 is CONNECT         */
        memcpy(&nPacket[2], &mDestPort, 2); /* Dest Port            */
        memcpy(&nPacket[4], &mDestIP, 4);       /* Dest IP Address      */
        memcpy(&nPacket[8], mPrefs.mUsername.c_str(), mPrefs.mUsername.length()+1);
        
        nWrote = mLink->Write(nPacket, nPacketLength);
        //if(nWrote != nPacketLength)

        mLink->AsyncRead(mResponse, 8);
        mState = TCPOpenConnectSOCKS::kWaitV4Response;
    }


    //** Read Response v4 Server, Handle Response **//
    else
    if(mState == TCPOpenConnectSOCKS::kWaitV4Response)
    {
        if(!mLink->ConnectionAlive())
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Server disconnected. May not be SOCKS v4");
        
        mLink->AsyncReadProcess();
        if(!mLink->AsyncReadDone())
            return;
            
        LONG nRead = mLink->AsyncReadProgress();
        mLink->AsyncReadCleanup();
        
        if(nRead != 8)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");

        
        switch(mResponse[1])
        {
            case 91: //Destination server refused connection
                mLink->Close();
                delete mLink, mLink = NULL;
                break;
            case 92: ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S4IdentFailed, "SOCKS4: Failed connection to this machine's identd");
                break;
            case 93: ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S4IdentMisMatch, "SOCKS4: Identd user different than supplied user");
                break;
        }
        
        //Successful SOCKS v4 Connection-By-Proxy
        mState  = TCPOpenConnectSOCKS::kIdle;
        mDone   = true;
    }


    ////////////////////////////////////////////////////////////////////////////
    // SOCKS Version 5 Negotiation Code
    ////////////////////////////////////////////////////////////////////////////

    //** Request Authorization Scheme v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kSendV5DetermineAuth)
    {
        static CHAR sAuthMethodHandshakeData[4] =
        {
            '\x05',     /* SOCKS Version                            */
            '\x02',     /* Number Of Auth Methods We Can Use        */
            '\x00',     /* Method1: No Authentication               */
            '\x02'      /* Method2: Username/Password               */
        };
        
        mLink->Write(sAuthMethodHandshakeData, 4);
        mLink->AsyncRead(mResponse, 2);
        mState = TCPOpenConnectSOCKS::kWaitV5AuthResponse;
    }
    
    
    //** Read/Respond To Authorization Response v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kWaitV5AuthResponse)
    {
        /* Reply Packet From SOCKS (2 bytes)    */
        /* [0]  Version                         */
        /* [1]  Reply Field                     */
        mLink->AsyncReadProcess();
        if(!mLink->AsyncReadDone())
            return;
            
        LONG nRead = mLink->AsyncReadProgress();
        mLink->AsyncReadCleanup();
        
        if(nRead != 2)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");
        if(mResponse[1] == '\xFF')
        {
            mLink->Close();
            delete mLink, mLink = NULL;
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5CannotAuth, "SOCKS Server Refused Auth Methods");
        }
        
        if(mResponse[1] == '\x02') //2 is User/Pass Auth Method
            mState = TCPOpenConnectSOCKS::kSendV5UserPassAuth;
        else
            mState = TCPOpenConnectSOCKS::kSendV5ConnectRequest;
        
        Process(); //Send Request Before We Stop Processing
        return;
    }
    
    
    //** Send Username/Password Authorization v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kSendV5UserPassAuth)
    {
        printf("Authenticating with User/Pass\n");
        fflush(stdout);
        /* Username/Password Auth Packet Structure      */
        /* [0]  Version Of Subnegiation (is 1)          */
        /* [1]  Username Length                         */
        /* [2]  Username Data (Above Length)            */
        /* [ ]  Password Length                         */
        /* [ ]  Password Data (Above Length)            */
        CHAR    nRawPacket[200];
        SHORT   nSpot = 0;
        
        nRawPacket[0] = '\x01';
        nRawPacket[1] = (CHAR) mPrefs.mUsername.length();
        memcpy(&nRawPacket[2], mPrefs.mUsername.c_str(), mPrefs.mUsername.length());
        nSpot = 2 + mPrefs.mUsername.length();
        nRawPacket[nSpot++] = (CHAR) mPrefs.mPassword.length();
        memcpy(&nRawPacket[nSpot], mPrefs.mPassword.c_str(), mPrefs.mPassword.length());
        nSpot += mPrefs.mPassword.length();
        
        mLink->Write(nRawPacket, nSpot);
        mLink->AsyncRead(mResponse, 2);
        mState = TCPOpenConnectSOCKS::kWaitV5UserPassResponse;
    }
    

    //** Read/Respond To User/Password Response v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kWaitV5UserPassResponse)
    {
        /* Reply Packet From SOCKS (2 bytes)        */
        /* [0]  Version                             */
        /* [1]  Status (0x00 indicates success      */
        
        //SOCKS v5 seems to disconnect us after more than one try of blank
        //username/password so we set it to failure if something f's up.        
        if(mLink->ConnectionAlive())
        {
            mLink->AsyncReadProcess();
            if(!mLink->AsyncReadDone())
                return;

            if(mLink->AsyncReadProgress() != 2)
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");
        }
        else
            mResponse[1] = 1;
        
        mLink->AsyncReadCleanup();
        if(mResponse[1] != '\x00')
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_AuthFailed, "User/Pass incorrect");
        
        mState = TCPOpenConnectSOCKS::kSendV5ConnectRequest;
        Process(); //Send Request Before We Stop Processing
        return;
    }
    
    
    //** Send Final Request For Connection v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kSendV5ConnectRequest)
    {
        printf("Connecting\n");
        fflush(stdout);
        CHAR nRawPacket[10];

        nRawPacket[0] = '\x05';     /* SOCKS Version                            */
        nRawPacket[1] = '\x01';     /* 1 is Connect (2=Bind, 3=UPD Associate)   */
        nRawPacket[2] = '\x00';     /* Reserved                                 */
        nRawPacket[3] = '\x01';     /* 1 is an IP v4 Address (IP-Long)          */
        memcpy(&nRawPacket[4], &mDestIP, 4);    /* 4 byte LONG IP               */
        memcpy(&nRawPacket[8], &mDestPort, 2);  /* 2 byte SHORT Port            */
        
        mLink->Write(nRawPacket, 10);
        mLink->AsyncRead(mResponse, 10);
        mState = TCPOpenConnectSOCKS::kWaitV5ConnectResponse;
    }
    
    
    //** Read/Respond To User/Password Response v5 **//
    else
    if(mState == TCPOpenConnectSOCKS::kWaitV5ConnectResponse)
    {
        /* Reply Packet From SOCKS (10 bytes)       */
        /* [0]  Version                             */
        /* [1]  Reply Field                         */
        /* [2]  Reserved                            */
        /* [3]  Address Type Of Following Addr      */
        /* [4]  ADDR - Associated IP (4 bytes)      */
        /* [8]  PORT - Associated Port (2 bytes)    */
        mLink->AsyncReadProcess();
        if(!mLink->AsyncReadDone())
            return;
            
        LONG nRead = mLink->AsyncReadProgress();
        mLink->AsyncReadCleanup();
        
        if(nRead != 10)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");
    
        switch(mResponse[1])
        {
            //Connection Worked
            case 0: break;

            //Connection Refused
            case 5: mLink->Close(); delete mLink, mLink = NULL; break;

            //Exceptional Errors
            case 1: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5General, "SOCKS5: General SOCKS server failure");
                break;
            case 2: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5Ruleset, "SOCKS5: Connection not allowed by ruleset");
                break;
            case 3: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5NetworkUR, "SOCKS5: Network unreachable");
                break;
            case 4: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5HostUR, "SOCKS5: Host unreachable");
                break;
            case 6: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5TTLExpired, "SOCKS5: TTL Expired");
                break;
            case 7: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5CmdNotSupp, "SOCKS5: Command not supported");
                break;
            case 8: 
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_S5AddrNotSupp, "SOCKS5: Address type not supported");
                break;
            default: 
                ThrowTCP(kProblemTCP_Proxy, mResponse[1], "SOCKS5: Unknown error");
                break;
        }
        
        //Successful SOCKS v5 Connection-By-Proxy
        mState  = TCPOpenConnectSOCKS::kIdle;
        mDone   = true;
    }
    
    else
    if(mState != TCPOpenConnectSOCKS::kIdle)
        ASSERT(!"Unhandled TCPOpenConnectSOCKS state"); 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectSOCKS::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPOpenConnectSOCKS::Results()
{
    ASSERT(mDone && !mGotResults);
    
    TCPLink* nTemp = mLink;
    mLink           = NULL;
    mGotResults = true;
    return nTemp;
}








#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectHTTP::TCPOpenConnectHTTP
//
//   Desc:: Constructor. Starts this asyncronous open connect using the HTTP
//          Web proxy protocol. Called internally when needed, users of this
//          code should not ever need to call this directly.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPOpenConnectHTTP::TCPOpenConnectHTTP(const HTTPProxyPrefs* hPrefs, const STRING& hAddr, SHORT hPort)
    : mDestPort(hPort)
    , mDestAddr(hAddr)
    , mResolving(NULL)
    , mConnecting(NULL)
    , mLink(NULL)
    , mState(TCPOpenConnectHTTP::kConnecting)
{
    ASSERT(hPrefs != NULL);
    mServer     = hPrefs->mServer;
    mPort           = hPrefs->mPort;
    mUsername       = hPrefs->mUsername;
    mPassword       = hPrefs->mPassword;
    mConnecting = TCPManager::Get()->AsyncOpenConnection(mServer, mPort, false);
}




TCPOpenConnectHTTP::~TCPOpenConnectHTTP()
{
    Cancel();
    if(mLink)
        delete mLink;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectHTTP::Cancel
//
//   Desc:: Cancels an asyncronous TCPOpenConnect task. Cancel() is not assumed
//          to fullfill the request completely until Done() is true. So more
//          calls to Process() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectHTTP::Cancel()
{
    if(mWasCanceled)
        return;

    if(mResolving)
    {
        mResolving->Cancel();
        delete mResolving;
        mResolving = NULL;  
    }
    if(mConnecting) 
    {
        mConnecting->Cancel();
        delete mConnecting;
        mConnecting = NULL;
    }

    mDone           = true;
    mState          = TCPOpenConnectHTTP::kIdle;
    mWasCanceled    = true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectHTTP::Process
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPOpenConnectHTTP::Process()
{
    //** Connect To the HTTP Web Proxy Server **//
    if(mState == TCPOpenConnectHTTP::kConnecting)
    {
        mConnecting->Process();
        if(!mConnecting->Done())
            return;
    
        mLink = mConnecting->Results();
        delete mConnecting, mConnecting = NULL;
        
        if(!mLink)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Down, "Proxy server not responding");
        
        mState = TCPOpenConnectHTTP::kSendConnect;
        Process();
        return;
    }
    
    
    //** Ask The Proxy Server To Connect To The Address **//
    else
    if(mState == TCPOpenConnectHTTP::kSendConnect)
    {
        //VALID HTTP TUNNELING PROXY REQUEST
        //-----------------------------------------------------------------
        //CONNECT server.host.com:2930 HTTP/1.1\n
        //Host: server.host.com:2930\n
        //Proxy-authorization: basic <Base64Encoded "user:pass">\n
        //\n
        //-----------------------------------------------------------------
        ASSERT(mLink != NULL);
        ASSERT(mDestAddr.length() < 1000); //Dont overflow buffers
        CHAR nBuf1[1024], nBuf2[1024];
        
        sprintf(nBuf1, "CONNECT %s:%d HTTP/1.1\n"   , mDestAddr.c_str(), mDestPort);
        sprintf(nBuf2, "Host: %s:%d\n"              , mDestAddr.c_str(), mDestPort);
        mLink->Write(nBuf1, strlen(nBuf1));
        mLink->Write(nBuf2, strlen(nBuf2));
        if(!mUsername.empty() || !mPassword.empty())
        {
            ASSERT(mUsername.length() < 100);
            ASSERT(mPassword.length() < 100);
            
            sprintf(nBuf1, "%s:%s", mUsername.c_str(), mPassword.c_str());
            Base64EncodeText(nBuf2, nBuf1); //Base64 encode the password
            sprintf(nBuf1, "Proxy-authorization: basic %s\n", nBuf2);
            mLink->Write(nBuf1, strlen(nBuf1));
        }
        
        //Final \n says we're done.
        mLink->Write("\n", 1);
        mResponse[0] = '\0';
        mState = TCPOpenConnectHTTP::kWaitResponse;
    }   
    
    
    //** Read In The First Line Of Their Response **//
    else
    if(mState == TCPOpenConnectHTTP::kWaitResponse)
    {
        //Read HTML header, look for "HTTP/1.0 403 Blah Blah Blah"
        //Anything in the 200's is success otherwise, freak out
        if(!mLink->ConnectionAlive())
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");

        //Read Till A CRLF
        bool    nHaveCR = false;
        CHAR    c;
        
        while(mLink->ReadReady())
        {
            if(mLink->Read(&c, 1) != 1)
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");

            strncat(mResponse, &c, 1);
            if(c == '\r')
            {
                nHaveCR = true;
                break;
            }
            
            //We should have an CR by 510 characters. If we don't stop and check
            //we could overwrite our mResponse buffer and corrupt memory.
            if(strlen(mResponse) > 510)
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Invalid HTTP proxy response");
        }
        if(!nHaveCR)
            return;
        
        //Parse The First Line Of The Response
        CHAR    nMessageBuf[512];
        float   nHTTPVers;
        LONG    nRespCode;
        
        if(sscanf(mResponse, "HTTP/%f %ld %[^\r]", &nHTTPVers, &nRespCode, nMessageBuf) != 3)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Invalid HTTP proxy response");
        
        
        //** HTTP Proxy Success! **//
        if(nRespCode >= 200 && nRespCode <= 299)
        {
            mState          = TCPOpenConnectHTTP::kFinishReadHeader;
            mReadForHeader  = 1;
            mLastFour[0] = '\0';
            mLastFour[1] = '\0';
            mLastFour[2] = '\0';
            mLastFour[3] = '\r';
            Process();
            return;
        }
        
        
        //** HTTP Proxy Failure! **//
        mState  = TCPOpenConnectHTTP::kIdle;
        mDone   = true;
        mLink->Close();
        delete mLink, mLink = NULL;
        
        if(nRespCode != 407)
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_HTTP4xx, mResponse);

        if(!mUsername.empty() && !mPassword.empty())
            ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_AuthFailed, "HTTP proxy authentication failed");

        ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_AuthRequired, "User/pass required for this HTTP proxy");
    }
    

    //** Finish Reading The Header (CRLFCRLF) **//
    else
    if(mState == TCPOpenConnectHTTP::kFinishReadHeader)
    {
        if(!mLink->ConnectionAlive())
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");

        //Read Till A CRLFCRLF
        bool    nHaveCRLFCRLF = false;
        CHAR    c;

        while(mLink->ReadReady())
        {
            if(mLink->Read(&c, 1) != 1)
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Incomplete Response");
            
            mReadForHeader++;
            mLastFour[0] = mLastFour[1];    //"1234" plus 5 becomes "2345"
            mLastFour[1] = mLastFour[2];
            mLastFour[2] = mLastFour[3];
            mLastFour[3] = c;

            if(!memcmp(mLastFour, "\r\n\r\n", 4))
            {
                nHaveCRLFCRLF = true;
                break;
            }

            //Header shouldn't be that long, something is messed if we don't get out
            //after 500 characters of reading.
            if(mReadForHeader > 500)
                ThrowTCP(kProblemTCP_Proxy, kErrTCP_Proxy_Negotiations, "Invalid HTTP proxy response");
        }
        if(!nHaveCRLFCRLF)
            return;
        
        //Done Reading HTML. Connection Is Now Clean For Normal I/O
        mState  = TCPOpenConnectHTTP::kIdle;
        mDone   = true;
    }

    else
    if(mState != TCPOpenConnectHTTP::kIdle)
        ASSERT(!"Unhandled TCPOpenConnectHTTP state");  
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPOpenConnectHTTP::Results
//
//   Desc:: Once Done() returns true, and you didn't Cancel() the task you may
//          use this method ONCE, and only once, to retrieve the results of the
//          task.
//
// Output:: It returns a TCPLink to the established connection. NULL if the
//          connection attempt was refused by the hAddr machine.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLink* TCPOpenConnectHTTP::Results()
{
    ASSERT(mDone && !mGotResults);
    
    TCPLink* nTemp  = mLink;
    mLink           = NULL;
    mGotResults     = true;
    return nTemp;
}






