/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                               [Windows]
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
#if(TARGET_WIN)
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CommonTypesCW.h"
#include "FileManagerWin.h"



//## Defines ##//
#define drwx______  S_IRUSR|S_IWUSR|S_IXUSR



FileManager* FileManager::Get()
{
    static FileManagerWin* nManager = NULL;
    
    if(!nManager)
        nManager =  new FileManagerWin();
        
    return nManager;
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


bool FileManagerWin::Startup()
{
    return FileManager::Startup();
}



void FileManagerWin::Shutdown()
{
    FileManager::Shutdown();
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif

STRING FileManagerWin::CvtToLocalPath(FilePath& hPath)
{
    STRING  nLocalPath;
    

    for(INT i = 0; i < hPath.Size(); i++)
    {
        if(hPath[i] == kDirOut)
            nLocalPath += kOSDirOut;
        else
            nLocalPath += hPath[i];
        
        if(i+1 < hPath.Size())
            nLocalPath += kOSDirSep;
    }
    
    return nLocalPath;
}



bool FileManagerWin::Exists(FilePath& hPath)
{
    STRING nPath = CvtToLocalPath(hPath);
    
    if(access(nPath.c_str(), F_OK))
        return false;
    else
        return true;
}


bool FileManagerWin::Move(FilePath& hFrom, FilePath &hTo)
{
    STRING nFromStr = CvtToLocalPath(hFrom);
    STRING nToStr   = CvtToLocalPath(hTo);
    
    if(rename(nFromStr.c_str(), nToStr.c_str()))
        return false;
    
    return true;
}



bool FileManagerWin::MakeDir(FilePath& hPath)
{
    STRING nPath = CvtToLocalPath(hPath);

    if(mkdir(nPath.c_str(), drwx______))
    {
        perror("mkdir");
        return false;
    }
    
    return true;
}





#endif //(TARGET_WIN)
