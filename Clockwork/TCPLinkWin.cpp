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
//   Name:: TCPLinkWin::TCPLinkWin
//
//   Desc:: Constructor.
//
//  Input:: hSock: the SOCKET (INT) value.
//          hIP: the IP address of the machine we're connected to.
//          hPort: the port the connection was established on.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkWin::TCPLinkWin(SOCKET hSock, const STRING& hIP, USHORT hPort)
    : TCPLink(hIP, hPort)
{
    //Set the socket descriptor
    mSocket = hSock;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::~TCPLinkWin
//
//   Desc:: Destructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

TCPLinkWin::~TCPLinkWin()
{
    if(mSocket != -1)
        Close();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::Closed
//
//   Desc:: Is this TCPLink closed?
//
// Output:: True if this TCPLink is closed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkWin::Closed()
{
    return mSocket == -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::Close
//
//   Desc:: Closes the connection and deallocates the memory allocated for it
//          by the OS.
//
//  Input:: hThrowOnErr: throw an exception if the close failed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::Close(bool hThrowOnErr)
{
    TCPManager::Get()->Remove(this);
    
    INT nResult;
    
    nResult = closesocket(mSocket);
    if(nResult == SOCKET_ERROR && hThrowOnErr)
    {
        mSocket = -1;
        ThrowTCP(kProblemTCP_CloseConnect, WSAGetLastError(), "closesocket()");
    }
    mSocket = -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::Read
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

LONG TCPLinkWin::Read(void* hBuff, LONG hBufSize)
{
    INT nResult;
    
    nResult = recv(mSocket, (char*) hBuff, hBufSize, 0);
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Recv, WSAGetLastError(), "recv()");
    
    return nResult;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::ReadReady
//
//   Desc:: Returns true if there is data waiting on the connection.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkWin::ReadReady()
{
    //** Query Socket **//
    ULONG   nWaiting;
    INT     nResult;
    

    nResult = ioctlsocket(mSocket, FIONREAD, &nWaiting);
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Poll, WSAGetLastError(), "ioctlsocket()");

    return nWaiting ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::Write
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

LONG TCPLinkWin::Write(const void *hData, LONG hAmount)
{
    LONG nWrote;
    
    nWrote = send(mSocket, (const char*) hData, hAmount, 0);
    if(nWrote == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Send, WSAGetLastError(), "send()");
    
    if(nWrote != hAmount)
        perror("Send failed.");
        
    return nWrote;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::WriteReady
//
//   Desc:: Returns true if the connection is able to be written to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkWin::WriteReady()
{
    //** Query Socket **//
    static struct timeval   noWait;
    fd_set                  writeSet;
    INT                     nResult;
    
    
    FD_ZERO(&writeSet);
    FD_SET(mSocket, &writeSet);
    nResult = select(mSocket+1, NULL, &writeSet, NULL, &noWait);
    if(nResult == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Poll, WSAGetLastError(), ErrorStr("Select()"));
    
    return FD_ISSET(mSocket, &writeSet) ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::ConnectionAlive
//
//   Desc:: Returns true if the connection is alive and connected. Otherwise
//          it handles the fact that it has been disconnect from the host.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkWin::ConnectionAlive()
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
//   Name:: TCPLinkWin::AsyncWrite
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

bool TCPLinkWin::AsyncWrite(const void *hData, LONG hAmount, bool hCopy)
{
    if(mDoingAsyncRead || mDoingAsyncWrite) return false;

    
    mWriteDone      = false;
    mWriteCanceled  = false;
    mWriteTotal = hAmount;
    mWriteProgress  = 0;
    mWriteCopied    = hCopy;

    if(hCopy)
    {
        mWriteData = new char[hAmount];
        memcpy(mWriteData, hData, hAmount);
    }else{
        mWriteData = (void*) hData;
    }

    TCPManagerWin::Get()->SetNonBlocking(mSocket);
    mDoingAsyncWrite = true;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncWriteCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncWriteCleanup()
{
    //Dispose copied data
    if(mWriteCopied && mWriteData)
    {
        delete [] ((CHAR*) mWriteData);
        mWriteData = NULL;
    }

    mDoingAsyncWrite = false;
    TCPManagerWin::Get()->SetBlocking(mSocket);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncWriteCancel
//
//   Desc:: Cancels an asyncronous writing task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncWriteCancel()
{
    if(mWriteCanceled || mWriteDone) return;
    
    mWriteDone      = true;
    mWriteCanceled  = true;
    AsyncWriteCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncWriteProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncWriteProcess()
{
    if(!WriteReady() || mWriteDone)
        return;

    //** Non-Blocking Data Transfer Code **//
    void*   nData   = (void*) (((CHAR*) mWriteData) + mWriteProgress);
    LONG    nWrote, nSend;


    //How big of a chunk to send
    if((mWriteTotal - mWriteProgress) < kTCPWriteBlockSize)
        nSend = mWriteTotal - mWriteProgress;
    else
        nSend = kTCPWriteBlockSize;

    //Send it. Send returns with anywhere from 1 to nSend.
    nWrote = send(mSocket, (const char*) nData, nSend, 0);
    if(nWrote == SOCKET_ERROR)
        ThrowTCP(kProblemTCP_Send, WSAGetLastError(), "send()");

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
//   Name:: TCPLinkWin::AsyncRead
//
//   Desc:: Attempts to asynchronously read data from the connection and 
//          returns immediately. You may only have one async read/write task at
//          any given time on any given link. You can query mReadTask object
//          to get the status of the read and control it.
//
//  Input:: hData: pointer to a data you want to write.
//          hAmount: the size of the data in bytes.
//          hCopy: make an internal copy of the data for you.
//
// Output:: False if it failed because other async read/write tasks were
//          pending on the link. True on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TCPLinkWin::AsyncRead(void* hBuff, LONG hBufSize)
{
    if(mDoingAsyncRead || mDoingAsyncWrite) return false;

    
    mReadDone       = false;
    mReadCanceled   = false;
    mReadBuf        = (CHAR*) hBuff;
    mReadBufSize    = hBufSize;
    mReadProgress   = 0;

    TCPManagerWin::Get()->SetNonBlocking(mSocket);
    mDoingAsyncRead = true;
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncReadCleanup
//
//   Desc:: Cleans up any memory and flags associated with the task. Like
//          Cancel() but the task is assumed to have finished or been canceled,
//          either way you are safely disposing the resources.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncReadCleanup()
{
    mDoingAsyncRead = false;
    TCPManagerWin::Get()->SetBlocking(mSocket);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncReadCancel
//
//   Desc:: Cancels an asyncronous reading task. Cancel() is not assumed to
//          fullfill the request completely until Done() is true. So more
//          calls to WriteProcess() may be required to fully cancel the call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncReadCancel()
{
    if(mReadCanceled || mReadDone) return;
    
    mReadDone       = true;
    mReadCanceled   = true;
    AsyncReadCleanup();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TCPLinkWin::AsyncReadProcess
//
//   Desc:: Must be called every once and a while for the asynchronous task to
//          continue processing as it advances from one internal state to the
//          next. Tasks are never assumed to be preemptive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void TCPLinkWin::AsyncReadProcess()
{
    if(!ReadReady() || mReadDone)
        return;

    //** Non-Blocking Data Transfer Code **//
    void*   nBuf    = (void*) (((CHAR*) mReadBuf) + mReadProgress);
    INT     nSize   = mReadBufSize - mReadProgress;
    INT     nRead;
    

    nRead = recv(mSocket, (CHAR*) nBuf, nSize, 0);
    if(nRead == 0)
        return;
    if(nRead == -1)
        ThrowTCP(kProblemTCP_Recv, WSAGetLastError(), ErrorStr("recv()"));
    
    mReadProgress += nRead;


    if(mReadProgress == mReadBufSize)
    {
        AsyncReadCleanup();
        mReadDone = true;
    }
}




#endif //(TARGET_WIN)
