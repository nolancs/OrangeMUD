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
#include "CommonTypesCW.h"
#include "FileManagerMac.h"
#include "StringUtils.h"


///## Local Functions ##//

OSErr GetDirectoryID(short vRefNum, long dirID, ConstStr255Param name, long *theDirID, Boolean *isDirectory);
OSErr FSpGetDirectoryID(const FSSpec *spec, long *theDirID, Boolean *isDirectory);
OSErr GetCatInfoNoName(short vRefNum, long dirID, ConstStr255Param name, CInfoPBPtr pb);



FileManager* FileManager::Get()
{
    static FileManagerMac* nManager = NULL;
    
    if(!nManager)
        nManager =  new FileManagerMac();
        
    return nManager;
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


bool FileManagerMac::Startup()
{
    return FileManager::Startup();
}



void FileManagerMac::Shutdown()
{
    FileManager::Shutdown();
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




STRING FileManagerMac::CvtToLocalPath(FilePath& hPath)
{
    STRING  nLocalPath;
    
    if(hPath.Size() > 1)
        nLocalPath += kOSDirSep;
    
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




bool FileManagerMac::Exists(FilePath& hPath)
{
    LONG        i, nVParentDirID = 0;
    FSSpec      nSpec;  
    OSErr       nErr;
    
    
    //Check Each Directory One At A Time
    for(i = 0; i < hPath.Size()-1; i++)
    {   
        FilePath    nTempDirPath((CHAR*) hPath[i].c_str());
        STRING      nStrPath = CvtToLocalPath(nTempDirPath);
        Boolean     nIsDir;
        
        nErr = FSMakeFSSpec(0, nVParentDirID, CToPStr(nStrPath.c_str()), &nSpec);
        if(nErr)
            return false;
        nErr = FSpGetDirectoryID(&nSpec, &nVParentDirID, &nIsDir);
        if(nErr)
            return false;
    }
    

    //Check the file or last item in the list
    FilePath    nLastItemPath((CHAR*) hPath[i].c_str());
    STRING      nLastItemPathStr(CvtToLocalPath(nLastItemPath));

    nErr = FSMakeFSSpec(0, nVParentDirID, CToPStr(nLastItemPathStr.c_str()), &nSpec);
    return nErr ? false : true;
}




bool FileManagerMac::Move(FilePath& hFrom, FilePath &hTo)
{
    return false;
}




bool FileManagerMac::MakeDir(FilePath& hPath)
{
    LONG        i, nCreatedDirID, nVParentDirID = 0;
    FSSpec      nSpec;  
    OSErr       nErr;
    
    
    //Check Each Directory One At A Time
    for(i = 0; i < hPath.Size()-1; i++)
    {   
        FilePath    nTempDirPath((CHAR*) hPath[i].c_str());
        STRING      nStrPath = CvtToLocalPath(nTempDirPath);
        Boolean     nIsDir;
        
        nErr = FSMakeFSSpec(0, nVParentDirID, CToPStr(nStrPath.c_str()), &nSpec);
        if(nErr)
            return false;
        nErr = FSpGetDirectoryID(&nSpec, &nVParentDirID, &nIsDir);
        if(nErr)
            return false;
    }
    

    //Check if the a file/dir exists already
    FilePath    nDirNamePath((CHAR*) hPath[i].c_str());
    STRING      nDirNamePathStr(CvtToLocalPath(nDirNamePath));

    nErr = FSMakeFSSpec(0, nVParentDirID, CToPStr(nDirNamePathStr), &nSpec);
    if(nErr != fnfErr) //If the result wasn't 'filenotfound'
        return false;
        
    //Make the directory off the spec
    nErr = FSpDirCreate(&nSpec, 0, &nCreatedDirID);
    return nErr ? false : true;
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#pragma mark ¥ÊMoreFiles Stuff ¥
#endif


//Stuff Cut Out of MoreFiles
OSErr GetDirectoryID(short vRefNum, long dirID, ConstStr255Param name, long *theDirID, Boolean *isDirectory)
{
    CInfoPBRec pb;
    OSErr error;

    error = GetCatInfoNoName(vRefNum, dirID, name, &pb);
    if ( error == noErr )
    {
        *isDirectory = (pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0;
        if ( *isDirectory )
        {
            *theDirID = pb.dirInfo.ioDrDirID;
        }
        else
        {
            *theDirID = pb.hFileInfo.ioFlParID;
        }
    }
    
    return ( error );
}

OSErr FSpGetDirectoryID(const FSSpec *spec, long *theDirID, Boolean *isDirectory)
{
    return ( GetDirectoryID(spec->vRefNum, spec->parID, spec->name,
             theDirID, isDirectory) );
}





OSErr GetCatInfoNoName(short vRefNum, long dirID, ConstStr255Param name, CInfoPBPtr pb)
{
    Str31 tempName;
    OSErr error;
    
    /* Protection against File Sharing problem */
    if ( (name == NULL) || (name[0] == 0) )
    {
        tempName[0] = 0;
        pb->dirInfo.ioNamePtr = tempName;
        pb->dirInfo.ioFDirIndex = -1;   /* use ioDirID */
    }
    else
    {
        pb->dirInfo.ioNamePtr = (StringPtr)name;
        pb->dirInfo.ioFDirIndex = 0;    /* use ioNamePtr and ioDirID */
    }
    pb->dirInfo.ioVRefNum = vRefNum;
    pb->dirInfo.ioDrDirID = dirID;
    error = PBGetCatInfoSync(pb);
    pb->dirInfo.ioNamePtr = NULL;
    return ( error );
}



#endif //(TARGET_MAC)
