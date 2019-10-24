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
//   Name:: TCPLinkUnix::TCPLinkUnix
//
//   Desc:: Constructor.
//
//  Input:: hEP: the ptr to an Endpoint.
//          hIP: the IP address of the machine we're connected to.
//          hPort: the port the connection was established on.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkUnix::TCPLinkUnix(SOCKET hSock, const STRING& hIP, USHORT hPort)
    : TCPLink(hIP, hPort)
{
    //Set descriptor
    mSocket = hSock;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::~TCPLinkUnix
//
//   Desc:: Destructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkUnix::~TCPLinkUnix()
{
    if(mSocket != -1)
        Close();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::Closed
//
//   Desc:: Is this TCPLink closed?
//
// Output:: True if this TCPLink is closed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkUnix::Closed()
{
    return mSocket == -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::Close
//
//   Desc:: Closes the connection and deallocates the memory allocated for it
//          by the OS.
//
//  Input:: hThrowOnErr: throw an exception if the close failed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::Close(bool hThrowOnErr)
{
    TCPManager::Get()->Remove(this);
//  fsync(mSocket); //Make sure all pending data is written out
    if(close(mSocket) == -1 && hThrowOnErr)
    {
        mSocket = -1;
        ThrowTCP(kProblemTCP_CloseConnect, errno, ErrorStr("close()"));
    }

    mSocket = -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::Read
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

LONG TCPLinkUnix::Read(void* hBuff, LONG hBufSize)
{
    INT nAmount;
    
    nAmount = recv(mSocket, (char*) hBuff, hBufSize, 0);
    
    // Connection Interupted?
    if(nAmount == -1 && errno == EINTR)
    {
        ConnectionAlive();
        nAmount = 0;
    }

    if(nAmount == -1)
        ThrowTCP(kProblemTCP_Recv, errno, ErrorStr("recv()"));
    
    return (LONG) nAmount;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::ReadReady
//
//   Desc:: Returns true if there is data waiting on the connection.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkUnix::ReadReady()
{
    //** Query Socket **//
    static struct timeval   noWait;
    fd_set                  readSet;
    
    
    FD_ZERO(&readSet);
    FD_SET(mSocket, &readSet);
    if(select(mSocket+1, &readSet, NULL, NULL, &noWait) == -1)
        ThrowTCP(kProblemTCP_Poll, errno, ErrorStr("Select()"));
    
    return FD_ISSET(mSocket, &readSet) ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::Write
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

LONG TCPLinkUnix::Write(const void *hData, LONG hAmount)
{
    LONG nWrote;
    
    //nWrote = write(mSocket, hData, hAmount);
    nWrote = send(mSocket, hData, hAmount, 0);
    if(nWrote == -1)
        ThrowTCP(kProblemTCP_Send, errno, ErrorStr("write()"));
    
    if(nWrote != hAmount)
        perror("Error on write()");
        
    return nWrote;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::WriteReady
//
//   Desc:: Returns true if the connection is able to be written to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkUnix::WriteReady()
{
    //** Query Socket **//
    static struct timeval   noWait;
    fd_set                  writeSet;
    
    
    FD_ZERO(&writeSet);
    FD_SET(mSocket, &writeSet);
    if(select(mSocket+1, NULL, &writeSet, NULL, &noWait) == -1)
        ThrowTCP(kProblemTCP_Poll, errno, ErrorStr("Select()"));
    
    return FD_ISSET(mSocket, &writeSet) ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::ConnectionAlive
//
//   Desc:: Returns true if the connection is alive and connected. Otherwise
//          it handles the fact that it has been disconnected from the host.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkUnix::ConnectionAlive()
{
    if(mSocket == -1)
        return false;

    static struct timeval   nNoWait;
    fd_set                  nDeadSet;
    fd_set                  nReadSet;
    
    FD_ZERO(&nReadSet);
    FD_ZERO(&nDeadSet);
    FD_SET(mSocket, &nReadSet);
    FD_SET(mSocket, &nDeadSet);
    if(select(mSocket+1, &nReadSet, NULL, &nDeadSet, &nNoWait) == -1)
        ThrowTCP(kProblemTCP_Poll, errno, ErrorStr("Select()"));
    
    //This never seems to happen even though you'd expect it to. Oh well.
    if(FD_ISSET(mSocket, &nDeadSet))
        return false;
    
    //Sockets always seem to get set in the ReadSet with no data to be
    //read when the other end disconnects. Read with a return of 0 indicates
    //a "graceful" disconnect. So just check for read and peek at the data.
    //If it says there's data, yet there's no data, then the connection died.
    if(!FD_ISSET(mSocket, &nReadSet))
        return true;
        
    CHAR nTempC;
    if(recv(mSocket, &nTempC, 1, MSG_PEEK) <= 0)
        return false;
    
    return true;    
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncWrite
//
//   Desc:: Attempts to asynchronously write an amount of data to the connection 
//          and returns immediately. You may only have one async read/write task
//          at any given time on any given link.
//
//  Input:: hData: pointer to a data you want to write.
//          hAmount: the size of the data in bytes.
//          hCopy: make an internal copy of the data for you.
//
// Output:: False if it failed because other async read/write tasks were
//          pending on the link. True on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkUnix::AsyncWrite(const void* hData, LONG hAmount, bool hCopy)
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

    TCPManagerUnix::Get()->SetNonBlocking(mSocket);
    mDoingAsyncWrite = true;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncWriteCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncWriteCleanup()
{
    //Dispose copied data
    if(mWriteCopied && mWriteData)
    {
        delete [] ((CHAR*) mWriteData);
        mWriteData = NULL;
    }

    mDoingAsyncWrite = false;
    TCPManagerUnix::Get()->SetBlocking(mSocket);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncWriteCancel
//
//   Desc:: Cancels an asyncronous writing task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncWriteCancel()
{
    if(mWriteCanceled || mWriteDone) return;
    
    mWriteDone      = true;
    mWriteCanceled  = true;
    AsyncWriteCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncWriteProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncWriteProcess()
{
    if(!WriteReady() || mWriteDone)
        return;

    //** Non-Blocking Data Transfer Code **//
    BYTE*   nData = &mWriteData[mWriteProgress];
    LONG    nWrote, nSend;


    //How big of a chunk to send
    if((mWriteTotal - mWriteProgress) < kTCPWriteBlockSize)
        nSend = mWriteTotal - mWriteProgress;
    else
        nSend = kTCPWriteBlockSize;

    //Send it. Send returns with anywhere from 1 to nSend.
    //nWrote = write(mSocket, nData, nSend);
    nWrote = send(mSocket, nData, nSend, 0);
    if(nWrote == -1)
        ThrowTCP(kProblemTCP_Send, errno, "send()");

    mWriteProgress += nWrote;
    

    //** Are We Done? **//
    if(mWriteProgress == mWriteTotal)
    {
        AsyncWriteCleanup();
        mWriteDone = true;
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

bool TCPLinkUnix::AsyncRead(void* hBuff, LONG hBufSize)
{
    if(mDoingAsyncRead || mDoingAsyncWrite) return false;

    
    mReadDone           = false;
    mReadCanceled       = false;
    mReadBuf            = (BYTE*) hBuff;
    mReadBufSize        = hBufSize;
    mReadProgress       = 0;


    TCPManagerUnix::Get()->SetNonBlocking(mSocket);
    mDoingAsyncRead = true;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncReadCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncReadCleanup()
{
    mDoingAsyncRead = false;
    TCPManagerUnix::Get()->SetBlocking(mSocket);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncReadCancel
//
//   Desc:: Cancels an asyncronous reading task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncReadCancel()
{
    if(mReadCanceled || mReadDone) return;
    
    mReadDone       = true;
    mReadCanceled   = true;
    AsyncReadCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkUnix::AsyncReadProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkUnix::AsyncReadProcess()
{
    if(!ReadReady() || mReadDone)
        return;

    //** Non-Blocking Data Transfer Code **//
    void*   nBuf = &mReadBuf[mReadProgress];
    int     nSize = mReadBufSize - mReadProgress;
    int     nRead;
    

    //nRead = read(mSocket, nBuf, nSize);
    nRead = recv(mSocket, (char*) nBuf, nSize, 0);
    if(nRead == 0)
        return;
    if(nRead == -1)
        ThrowTCP(kProblemTCP_Recv, errno, ErrorStr("read()"));
    
    mReadProgress += nRead;

    if(mReadProgress == mReadBufSize)
    {
        AsyncReadCleanup();
        mReadDone = true;
    }
}







#endif //(TARGET_UNIX)
