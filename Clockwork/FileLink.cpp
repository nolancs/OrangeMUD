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
#include "CommonTypesCW.h"
#include "FileLink.h"




FileLink::FileLink()
{
    mFilePtr = NULL;
}





FileLink::~FileLink()
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
//   Name:: FileLink::Closed
//
//   Desc:: Tells you if the Link is closed.
//
//  Input:: True if it is closed, false if it is still open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool FileLink::Closed()
{
    return mFilePtr == NULL;
}




void FileLink::Close(bool hThrowOnErr)
{
    INT nResult;
    
    nResult = fclose(mFilePtr);
    mFilePtr = NULL;
    
    if(nResult != 0 && hThrowOnErr)
        ; //Throw(kProblemTCP_Network, nResult, "Failed to close file.");
}




LONG FileLink::Read(void* hBuff, LONG hBufSize)
{
    return fread(hBuff, 1, hBufSize, mFilePtr);
}




bool FileLink::ReadReady()
{
    if(feof(mFilePtr))
        return false; else return true;
}




LONG FileLink::Write(const void* hData, LONG hAmount)
{   
    return fwrite(hData, 1, hAmount, mFilePtr);
}





bool FileLink::WriteReady()
{
    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



INT FileLink::GetC()
{
    return getc(mFilePtr);
}



void FileLink::UnGetC(CHAR hC)
{
    ungetc(hC, mFilePtr);
}





LONG FileLink::GetPos()
{
    return ftell(mFilePtr);
}


LONG FileLink::SetPos(LONG hPos, INT hSeek)
{
    fseek(mFilePtr, hPos, hSeek);
    return GetPos();
}



LONG FileLink::FileSize()
{
    LONG nLength, nSavePos = GetPos();
    
    SetPos(0, SEEK_END);
    nLength = GetPos();
    SetPos(nSavePos, SEEK_CUR);
    return nLength;
}







