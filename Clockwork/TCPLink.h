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
#ifndef TCPLINK_H
#define TCPLINK_H 1
#include "Link.h"

#define kTCPWriteBlockSize  4096


class TCPLink : public Link
{
    //** De/Constructors **//
    public: TCPLink(const STRING& hIP, USHORT hPort);
    public: virtual ~TCPLink()  {};


    //** Abstract Link Functions  **//
    public:
    virtual bool    Closed()                                = 0;
    virtual void    Close(bool hThrowOnErr = false)         = 0;
    virtual LONG    Read(void* hBuff, LONG hBufSize)        = 0;
    virtual bool    ReadReady()                             = 0;
    virtual LONG    Write(const void *hData, LONG hAmount)  = 0;
    virtual bool    WriteReady()                            = 0;
    virtual bool    ConnectionAlive()                       = 0;
    
    virtual bool    AsyncWrite(const void *hData, LONG hAmount, bool hCopy = false) = 0;
    virtual bool    AsyncWriteDone()    = 0;
    virtual bool    AsyncWriteCanceled()= 0;
    virtual ULONG   AsyncWriteProgress()= 0;
    virtual ULONG   AsyncWriteTotal()   = 0;
    virtual void    AsyncWriteCleanup() = 0;
    virtual void    AsyncWriteCancel()  = 0;
    virtual void    AsyncWriteProcess() = 0;
    virtual bool    AsyncRead(void* hBuff, LONG hBufSize) = 0;
    virtual bool    AsyncReadDone()     = 0;
    virtual bool    AsyncReadCanceled() = 0;
    virtual ULONG   AsyncReadProgress() = 0;
    virtual void*   AsyncReadBuffer()   = 0;
    virtual void    AsyncReadCleanup()  = 0;
    virtual void    AsyncReadCancel()   = 0;
    virtual void    AsyncReadProcess()  = 0;


    //** Public Member Functions **//
    public:
    const CHAR*     GetPeerIP() const;
    const CHAR*     GetPeerName() const;
    USHORT          GetPeerPort() const;


    //** Private Data Members **//
    protected:
    STRING          mPeerIP;
    mutable STRING  mPeerName;
    USHORT          mPeerPort;
    bool            mDoingAsyncWrite;
    bool            mDoingAsyncRead;
};



#endif //ifndef TCPLINK_H
