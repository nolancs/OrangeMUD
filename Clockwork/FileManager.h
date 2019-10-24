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
#ifndef FILEMANAGER_H
#define FILEMANAGER_H 1



#define kDirOut         ""

#if(TARGET_MAC)
#define kOSDirSep       ":"
#define kOSDirOut       ""
#else
#define kOSDirSep       "/"
#define kOSDirOut       ".."
#endif





class FileManager
{
    //** Singleton Access & De/Constructors**//
    protected: FileManager() {}
    public: virtual ~FileManager()  {};
    public: static FileManager* Get();
    
    
    
    //** Inheireted Member Functions **//
    public:
    virtual bool    Startup();      //Must be called from subclasses' Startup()
    virtual void    Shutdown();     //Must be called from subclasses' Shutdown()
    
    
    //** Abstract Member Functions **//
    virtual bool        Exists(FilePath& hPath)                 = 0;
    virtual bool        Move(FilePath& hFrom, FilePath &hTo)    = 0;    
    virtual bool        MakeDir(FilePath& hPath)                = 0;    
    virtual STRING      CvtToLocalPath(FilePath& hPath)         = 0;
    virtual FileLink*   OpenRead(FilePath& hPath);
    virtual FileLink*   OpenWrite(FilePath& hPath);
};




class FilePath
{
    public: FilePath() {}
    public: FilePath(CHAR* hA    , CHAR* hB=NULL, CHAR* hC=NULL, CHAR* hD=NULL, 
                    CHAR* hE=NULL, CHAR* hF=NULL, CHAR* hG=NULL, CHAR* hH=NULL)
    {
        if(hA) mPathStrs.AddBack(hA);
        if(hB) mPathStrs.AddBack(hB);
        if(hC) mPathStrs.AddBack(hC);
        if(hD) mPathStrs.AddBack(hD);
        if(hE) mPathStrs.AddBack(hE);
        if(hF) mPathStrs.AddBack(hF);
        if(hG) mPathStrs.AddBack(hG);
        if(hH) mPathStrs.AddBack(hH);
    }
    
    
    //** Public Member Functions **//
    public:
    INT     Size()              { return mPathStrs.size(); }
    STRING& operator[](INT hN)  { return mPathStrs[hN]; }
    void    Add(STRING hStr)    { mPathStrs.AddBack(hStr); }


    //** Public Data Members **//
    public:
    Vector<STRING> mPathStrs;
};


#endif //ifndef FILEMANAGER_H
