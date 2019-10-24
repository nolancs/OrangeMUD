/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                                 [MacOS]
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
#ifndef FILEMANAGERMAC_H
#define FILEMANAGERMAC_H 1
#include "FileManager.h"



class FileManagerMac : public FileManager
{
    //** Singleton Access & De/Constructors**//
    friend class FileManager;
    protected: FileManagerMac() {}
    public: virtual ~FileManagerMac()   {}
    
    
    //** Inheireted Member Functions **//
    public:
    virtual bool    Startup();
    virtual void    Shutdown();
    
    
    //** Abstract Member Functions **//
    virtual STRING      CvtToLocalPath(FilePath& hPath);
    virtual bool        Exists(FilePath& hPath);
    virtual bool        Move(FilePath& hFrom, FilePath &hTo);
    virtual bool        MakeDir(FilePath& hPath);
};




#endif //ifndef FILEMANAGERMAC_H
#endif //(TARGET_MAC)
