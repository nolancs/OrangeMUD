/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: January 2001                                          [Crossplatform]
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
#ifndef EXIT_H
#define EXIT_H 1
#include "ActThing.h"
#include "LoadOld.h"
#include "SaveLoadable.h"

class Exit
    : public ActThing
    , public LoadOld
    , public SaveLoadable
{
    public: Exit(RoomIndex* hInRoom);
    public: virtual ~Exit();

    
    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag); 


    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    static void     LoadRoomExits(MUDFile& hFp, RoomIndex* hIntoRoom);


    //** ActThing Functions **//
    virtual STRINGCW    GetTextForCodeTo(Person* hTo, CHAR hCode);
    Exit*               GetOtherSideExit();


    //** Public Member Functions **//
    public:
    bool            IsClosed();
    bool            IsDoor();
    bool            IsLocked();
    bool            IsWater();
    bool            IsHidden();
    void            ShowTo(Person* hCh);
    void            StatTo(Person* hCh);
    void            Open(bool hBothSides);
    void            Close(bool hBothSides);




    //** Runtime Data Members **//
    public:
    RoomIndex*  mToRoom;
    RoomIndex*  mInRoom;
    
    //** Persistant Data Members **//
    public:
    VREF        mToVRef;
    LONG        mFlags;
    VREF        mKey;
    CHAR        mDirection;
    STRINGCW    mName;
    STRINGCW    mKeywords;
    STRINGCW    mDescription;
        
    VNUM        mToOldVnum;         //Outdated
    VNUM        mOldKey;            //Outdated
};





//## Exit Flags ##//
#define kExIsDoor       (A)
#define kExClosed       (B)
#define kExLocked       (C)
#define kExHidden       (D)
#define kExNoWander     (E)
#define kExUnused1      (F)
#define kExNoClose      (G) //May be a door, but not closable
#define kExLockBroken   (H) //Cannot be un/locked, even w/ key
#define kExNoSound      (I) //Sound doesn't penetrate. No eavesdropping.
#define kExNoPass       (J) //No passing through the door
#define kExNoPick       (K) //No picking the lock
#define kExEasy         (L) //Lock picking difficulties
#define kExHard         (M)
#define kExInfuriating  (N)
#define kExPlural       (Z) //Handles exit name as plural

#define kNoKey          MakeVRef(0,0)
#define kNoRoom         MakeVRef(USHRT_MAX, USHRT_MAX)



#endif //ifndef EXIT_H
