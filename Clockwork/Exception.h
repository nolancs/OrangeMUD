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
#ifndef EXCEPTION_H
#define EXCEPTION_H 1

#define kProblem_NoErr      0
#define kProblem_Failed     1


class Exception 
    : public std::exception
{
    //** Public Member Functions **//
    public: 
    Exception(STRING hType, LONG hProblem, LONG hErr, STRING hFile, LONG hLine, STRING hMsg)
    {
        CHAR nBuf[1024];
        sprintf(nBuf             , "[[ Exception Thrown: %s\n", hType.c_str());
        sprintf(nBuf+strlen(nBuf), "[[     Failure Info: %s\n", hMsg.c_str());
        sprintf(nBuf+strlen(nBuf), "[[         Error No: %ld\n", hErr);
        sprintf(nBuf+strlen(nBuf), "[[             File: %s, Line %ld\n", hFile.c_str(), hLine);
        mWhat   = nBuf;
        mMsg    = hMsg;
        mFile   = hFile;
        mLine   = hLine;
    }
    ~Exception() throw(){}
    
    virtual const char* what() const throw()
    {
        return mWhat.c_str();
    }   
    

    //** Public Data Members **//
    public:
    STRING          mWhat;
    LONG            mProblem;
    LONG            mErrCode;
    STRING          mMsg;
    STRING          mFile;
    LONG            mLine;
};


#define Display_Exception(x)    cout << x.what() << endl

#if(TARGET_UNIX)
#include <errno.h>
#define ErrorStr(xMsg)  string(string(xMsg)+": "+strerror(errno))
#elif(TARGET_WIN)
#define ErrorStr(xMsg)  (xMsg)
#elif(TARGET_MAC)
#define ErrorStr(xMsg)  (xMsg)
#endif

    
#endif //ifndef EXCEPTION_H
