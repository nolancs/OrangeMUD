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
#ifndef TCPLINKMAC_H
#define TCPLINKMAC_H 1
#include <OpenTransport.h>
#include <OpenTransportProviders.h>
#include "TCPLink.h"




class TCPLinkMac : public TCPLink
{
    friend class TCPManagerMac;
    friend class TCPOpenConnectMac;
    friend class TCPRecvConnectMac;
    
    
    //** De/Constructors **//
    protected:  TCPLinkMac(EndpointRef hEP, const STRING& hIP, USHORT hPort);
    public:     ~TCPLinkMac();
    
    
    //** Abstract Member Functions **//
    public:
    virtual bool    Closed();
    virtual void    Close(bool hThrowOnErr = false);
    virtual LONG    Read(void* hBuff, LONG hBufSize);
    virtual bool    ReadReady();
    virtual LONG    Write(const void *hData, LONG hAmount);
    virtual bool    WriteReady();
    virtual bool    ConnectionAlive();
    //Async Write
    virtual bool    AsyncWrite(const void *hData, LONG hAmount, bool hCopy = false);
    virtual bool    AsyncWriteDone()        { return mWriteDone;        }
    virtual bool    AsyncWriteCanceled()    { return mWriteCanceled;    }
    virtual ULONG   AsyncWriteProgress()    { return mWriteProgress;    }
    virtual ULONG   AsyncWriteTotal()       { return mWriteTotal;       }
    virtual void    AsyncWriteCleanup();
    virtual void    AsyncWriteCancel();
    virtual void    AsyncWriteProcess();
    //Async Read
    virtual bool    AsyncRead(void* hBuff, LONG hBufSize);
    virtual bool    AsyncReadDone()         { return mReadDone;     }
    virtual bool    AsyncReadCanceled()     { return mReadCanceled; }
    virtual ULONG   AsyncReadProgress()     { return mReadProgress; }
    virtual void*   AsyncReadBuffer()       { return (void*) mReadBuf;  }
    virtual void    AsyncReadCleanup();
    virtual void    AsyncReadCancel();
    virtual void    AsyncReadProcess();
    
    
    //** Private Data Members **//
    protected:
    EndpointRef     mEndpoint;
    //Async Write Data Members
    bool            mWriteDone;     //Job finished?
    bool            mWriteCanceled; //Job canceled?
    ULONG           mWriteTotal;        //Total to write
    ULONG           mWriteProgress; //Amount wrote
    BYTE*           mWriteData;     //Data to send
    bool            mWriteCopied;       //Did we make a copy?
    //Async Read Data Members
    bool            mReadDone;
    bool            mReadCanceled;
    ULONG           mReadProgress;
    ULONG           mReadBufSize;
    BYTE*           mReadBuf;
};




#endif //ifndef TCPLINKMAC_H
#endif //(TARGET_MAC)
