/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: September, 2001                                       [Crossplatform]
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
#include "MemFileLink.h"




MemFileLink::MemFileLink(FileLink* hLink)
    : mMem(NULL)
    , mPos(0)
    , mLen(0)
{
    mLen = hLink->FileSize();
    mMem = new BYTE[mLen];
    hLink->SetPos(0);
    hLink->Read(mMem, mLen);
}





MemFileLink::~MemFileLink()
{
    if(!Closed())
        Close();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MemFileLink::Closed
//
//   Desc:: Tells you if the Link is closed.
//
//  Input:: True if it is closed, false if it is still open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MemFileLink::Closed()
{
    return mMem == NULL;
}




void MemFileLink::Close(bool hThrowOnErr)
{
    delete [] mMem;
    mMem = NULL;
    mPos = 0;
    mLen = 0;
}




LONG MemFileLink::Read(void* hBuff, LONG hBufSize)
{
    LONG nCanRead = mLen-mPos;
    nCanRead = nCanRead < hBufSize ? nCanRead : hBufSize;
    memcpy(hBuff, &mMem[mPos], nCanRead);
    mPos += nCanRead;
    return nCanRead;
}




bool MemFileLink::ReadReady()
{
    return mPos < mLen;
}




LONG MemFileLink::Write(const void* hData, LONG hAmount)
{   
    ASSERT(false);
    return 0;
}





bool MemFileLink::WriteReady()
{
    ASSERT(false);
    return 0;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



INT MemFileLink::GetC()
{
    return mPos < mLen ? mMem[mPos++] : EOF;
}



void MemFileLink::UnGetC(CHAR hC)
{
    mPos--;
}





LONG MemFileLink::GetPos()
{
    return mPos;
}


LONG MemFileLink::SetPos(LONG hPos, INT hSeek)
{
    switch(hSeek)
    {
        case SEEK_END:
            mPos = mLen+hPos;
            break;
        case SEEK_CUR:
            mPos = mPos+hPos;
            break;
        case SEEK_SET:
            mPos = 0+hPos;
            break;
    }
    
    return GetPos();
}



LONG MemFileLink::FileSize()
{
    LONG nLength, nSavePos = GetPos();
    
    SetPos(0, SEEK_END);
    nLength = GetPos();
    SetPos(nSavePos, SEEK_CUR);
    return nLength;
}







