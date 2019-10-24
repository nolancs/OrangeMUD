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



//## Prototypes ##//
pascal void AsyncWriteNotifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie);





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::TCPLinkMac
//
//   Desc:: Constructor.
//
//  Input:: hEP: the ptr to an Endpoint.
//          hIP: the IP address of the machine we're connected to.
//          hPort: the port the connection was established on.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkMac::TCPLinkMac(EndpointRef hEP, const STRING& hIP, USHORT hPort)
    : TCPLink(hIP, hPort)
    , mEndpoint(hEP)
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::~TCPLinkMac
//
//   Desc:: Destructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkMac::~TCPLinkMac()
{
    Close();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::Closed
//
//   Desc:: Is this TCPLink closed?
//
// Output:: True if this TCPLink is closed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::Closed()
{
    return mEndpoint == NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::Close
//
//   Desc:: Closes the connection and deallocates the memory allocated for it
//          by the OS.
//
//  Input:: hThrowOnErr: throw an exception if the close failed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::Close(bool hThrowOnErr)
{
    TCPManager::Get()->Remove(this);
    if(!mEndpoint)
        return;
    
    //** Disconnection Code **//
    TCall       nEmptyCall;
    OSStatus    nResult;
    OTResult    nState = OTGetEndpointState(mEndpoint);
    
    
    OTMemzero(&nEmptyCall, sizeof(TCall));
    nResult = OTSndDisconnect(mEndpoint, &nEmptyCall);

    OTCloseProvider(mEndpoint);
    mEndpoint = NULL;

    if(nResult != noErr && hThrowOnErr) 
        ThrowTCP(kProblemTCP_Network, nResult, "Failed to OTSndDisconnect()");
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::Read
//
//   Desc:: Reads at LEAST one byte of data off the connection. If there is
//          more waiting it will read all of it or up to the buffer size. This
//          call will block and wait for the one byte if no data is available.
//
//  Input:: hBuff: pointer to a buffer.
//          hBufSize: the size of the buffer in bytes.
//
// Output:: The the amount read. Zero indicates the connection was lost. If any
//          other error occurs it will throw an exception.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG TCPLinkMac::Read(void* hBuff, LONG hBufSize)
{
    OTFlags     nFlags;
    OSStatus    nResult;

    TCPManagerMac::Get()->SetAsynch(mEndpoint, NULL);

    do
    {   
    /////////////////////////////////////////////////////////////////////
        nFlags = 0;
        nResult = OTRcv(mEndpoint, hBuff, hBufSize, &nFlags);
        if(nResult == kOTNoDataErr)
        {
            TCPManagerMac::YieldingNotifier(NULL, kOTSyncIdleEvent, 0, NULL);
        }
        else
        if(nResult < 0 )
        {
            TCPManagerMac::Get()->SetSynch(mEndpoint);
            ThrowTCP(kProblemTCP_Network, nResult, "OTRcv()");
        }

        //if(!ConnectionAlive())
        //  break;
    /////////////////////////////////////////////////////////////////////
    }
    while(nResult == kOTNoDataErr);
    

    TCPManagerMac::Get()->SetSynch(mEndpoint);
    
    return nResult;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::ReadReady
//
//   Desc:: Returns true if there is data waiting on the connection.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::ReadReady()
{
    OTResult nResult = OTLook(mEndpoint); 
    
    return (nResult == T_DATA || nResult == T_EXDATA);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::Write
//
//   Desc:: Attempts to write an amount of data to the connection. This call is
//          blocking and is assumed not to return until all the data was
//          written. It will throw an kProblemTCP_Send exception if the write failed
//          completely, otherwise the amount wrote is returned.
//
//  Input:: hData: pointer to a data you want to write.
//          hAmount: the size of the data in bytes.
//
// Output:: The the amount wrote.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG TCPLinkMac::Write(const void *hData, LONG hAmount)
{
    OSStatus    nSent;
    
    nSent = OTSnd(mEndpoint, (void*) hData, hAmount, 0);
    if(nSent < 0)
        ThrowTCP(kProblemTCP_Send, nSent, "OTSend()");
        
    if(nSent != hAmount)
        cout << "OTSnd failed.\n";
        
    return nSent;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::WriteReady
//
//   Desc:: Returns true if the connection is able to be written to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::WriteReady()
{
    OTResult nResult;
    
    nResult = OTGetEndpointState(mEndpoint);
    
    return (nResult == T_DATAXFER || nResult == T_INREL);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::ConnectionAlive
//
//   Desc:: Returns true if the connection is alive and connected. Otherwise
//          it handles the fact that it has been disconnect from the host.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::ConnectionAlive()
{
    if(!mEndpoint) return false;
    
    OTResult nResult = OTLook(mEndpoint); 

    if(nResult == T_DISCONNECT)
    {
        TDiscon nDiscon;
        
        OTMemzero(&nDiscon, sizeof(TDiscon));
        OTRcvDisconnect(mEndpoint, &nDiscon);
        OTCloseProvider(mEndpoint);
        mEndpoint = NULL;
        return false;
    }
    else
    if(nResult == T_ORDREL)
    {
        OTRcvOrderlyDisconnect(mEndpoint);
        OTSndOrderlyDisconnect(mEndpoint);
        OTCloseProvider(mEndpoint);
        mEndpoint = NULL;
        return false;
    }
    else
    {
        return true;
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncWrite
//
//   Desc:: Attempts to asynchronously write an amount of data to the connection 
//          and returns immediately. You may only have one async read/write task
//          at any given time on any given link. You can query mWriteTask object
//          to get the status of the write and control it.
//
//  Input:: hData: pointer to a data you want to write.
//          hAmount: the size of the data in bytes.
//          hCopy: make an internal copy of the data for you.
//
// Output:: False if it failed because other async read/write tasks were
//          pending on the link. True on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::AsyncWrite(const void *hData, LONG hAmount, bool hCopy)
{
    if(mDoingAsyncRead || mDoingAsyncWrite) return false;

    
    mWriteDone      = false;
    mWriteCanceled  = false;
    mWriteTotal = hAmount;
    mWriteProgress  = 0;
    mWriteCopied    = hCopy;

    if(hCopy)
    {
        mWriteData = new BYTE[hAmount];
        memcpy(mWriteData, hData, hAmount);
    }else{
        mWriteData = (BYTE*) hData;
    }

    TCPManagerMac::Get()->SetAsynch(mEndpoint, AsyncWriteNotifier);
    mDoingAsyncWrite = true;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncWriteCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncWriteCleanup()
{
    //Dispose copied data
    if(mWriteCopied && mWriteData)
    {
        delete [] mWriteData;
        mWriteData = NULL;
    }

    mDoingAsyncWrite = false;
    TCPManagerMac::Get()->SetSynch(mEndpoint);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncWriteCancel
//
//   Desc:: Cancels an asyncronous writing task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncWriteCancel()
{
    if(mWriteCanceled || mWriteDone) return;
    
    mWriteDone      = true;
    mWriteCanceled  = true;
    AsyncWriteCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncWriteProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncWriteProcess()
{
    if(!WriteReady() || mWriteDone)
        return;


    //** Non-Blocking Data Transfer Code **//
    BYTE*   nData   = &mWriteData[mWriteProgress];
    OTFlags nFlags  = 0;
    LONG    nWrote, nSend;

    //Determine send size
    if((mWriteTotal - mWriteProgress) < kTCPWriteBlockSize)
    {
        nSend = (mWriteTotal - mWriteProgress);
    }else{
        nSend = kTCPWriteBlockSize;
        nFlags |= T_MORE;
    }
    
    nWrote = OTSnd(mEndpoint, nData, nSend, nFlags);
    if(nWrote < 0)
        ThrowTCP(kProblemTCP_Send, nWrote, "OTSnd()");
    
    mWriteProgress += nWrote;


    //** Are We Done? **//
    if(mWriteProgress == mWriteTotal)
    {
        AsyncWriteCleanup();
        mWriteDone = true;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: AsyncWriteNotifier
//
//   Desc::
//
//  Input:: Read the OT docs if you want to know.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pascal void AsyncWriteNotifier(void* hContextPtr, OTEventCode hCode, OTResult hResult, void* hCookie)
{
    switch(hCode) 
    {
        default:
            printf("got nofified\n");
            break;
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncRead
//
//   Desc:: Attempts to asynchronously read data from the connection and 
//          returns immediately. You may only have one async read/write task at
//          any given time on any given link.
//
//  Input:: hBuff: pointer to buffer to be read into.
//          hBufSize: much data to read into this buffer.
//
// Output:: False if it failed because other async read/write tasks were
//          pending on the link. True on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkMac::AsyncRead(void* hBuff, LONG hBufSize)
{
    if(mDoingAsyncRead || mDoingAsyncWrite) return false;

    
    mReadDone       = false;
    mReadCanceled   = false;
    mReadBufSize    = hBufSize;
    mReadBuf        = (BYTE*) hBuff;
    mReadProgress   = 0;


    TCPManagerMac::Get()->SetAsynch(mEndpoint, NULL);
    mDoingAsyncRead = true;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncReadCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncReadCleanup()
{
    mDoingAsyncRead = false;
    TCPManagerMac::Get()->SetSynch(mEndpoint);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncReadCancel
//
//   Desc:: Cancels an asyncronous reading task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncReadCancel()
{
    if(mReadCanceled || mReadDone) return;
    
    mReadDone       = true;
    mReadCanceled   = true;
    AsyncReadCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkMac::AsyncReadProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkMac::AsyncReadProcess()
{
    if(!ReadReady() || mReadDone)
        return;

    //** Non-Blocking Data Transfer Code **//
    void*       nBuf    = &mReadBuf[mReadProgress];
    size_t      nSize   = mReadBufSize - mReadProgress;
    OTResult    nRead;
    

    nRead = OTRcv(mEndpoint, nBuf, nSize, 0);
    if(nRead == kOTNoDataErr)
        return;
    if(nRead < 0)
        ThrowTCP(kProblemTCP_Recv, nRead, "OTRcv()");
        
    mReadProgress += nRead;
    

    //** Are We Done? **//
    if(mReadProgress == mReadBufSize)
    {
        AsyncReadCleanup();
        mReadDone = true;
    }
}











#endif //(TARGET_MAC)
