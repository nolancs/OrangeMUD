/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                         [Crossplatform]
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
#ifndef MEMFILELINK_H
#define MEMFILELINK_H 1
#include "FileLink.h"



class MemFileLink
    : public FileLink
{
    //** De/Constructors**//
    public: MemFileLink(FileLink*);
    public: virtual ~MemFileLink();
    

    //** Abstract Link Functions  **//
    public:
    virtual bool    Closed();
    virtual void    Close(bool hThrowOnErr = false);
    virtual LONG    Read(void* hBuff, LONG hBufSize);
    virtual bool    ReadReady();
    virtual LONG    Write(const void* hData, LONG hAmount);
    virtual bool    WriteReady();
    
    //** Public Member Functions **//
    virtual INT     GetC();
    virtual void    UnGetC(CHAR hC);
    virtual LONG    GetPos();
    virtual LONG    SetPos(LONG hPos, INT hSeek = SEEK_SET);
    virtual LONG    FileSize();
    

    //** Public Data Members **//
    public:
    BYTE*           mMem;
    LONG            mPos;
    LONG            mLen;
};


#endif //ifndef MEMFILELINK_H
