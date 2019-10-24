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
#ifndef LOADOLD_H
#define LOADOLD_H 1

//## Defines ##//
#define SAVE_EOF            "(EOF)"

#define SECT_MAIN           "@_MAIN"
#define BEGIN_SECTION       "@@_SECT"
#define BEGIN_OBJECT        "$$_OBJ"
#define BEGIN_VNUM_OBJECT   "$$_VOBJ"
#define BEGIN_LLIST         "##_LL"
#define END_SECTION         "@@xSECT"
#define END_OBJECT          "$$xOBJ"
#define END_LLIST           "##xLL"

//** Each Object Type **//
#define IO_PLAYER           "PLAYER"
#define IO_MOBILE           "MOBILE"
#define IO_OBJECT           "OBJ"
#define IO_CURRENCY         "CUR"
#define IO_HELPFILE         "HFILE"
#define IO_SOCFILE          "SFILE"
#define IO_HELP             "HELP"
#define IO_SOCIAL           "SOCIAL"
#define IO_AREAFILE         "AREA"
#define IO_ROOMIDX          "ROOMIDX"
#define IO_OBJIDX           "OBJIDX"
#define IO_MOBIDX           "MOBIDX"
#define IO_EDESC            "EDESC"
#define IO_SCRIPT           "SCRIPT"
#define IO_VAR              "VAR"
#define IO_EXIT             "EXIT"
#define IO_PART             "PART"
#define IO_ATTACK           "ATTACK"
#define IO_SKIN             "SKIN"
#define IO_OREPOP           "OREPOP"
#define IO_MREPOP           "MREPOP"
#define IO_EREPOP           "EREPOP"
#define IO_OLR              "OLR"
#define IO_AMBIENCE         "AMB"
#define IO_SHOP             "SHOP"

//** Certain Special Loading Defs **//
#define IO_OLR_CONTENTS     "#OLR_CONT"
#define IO_AREA_REPOPS      "#AREA_REPOPS"
#define IO_AREA_ROOM_AMB    "#AREA_ROOM_AMB"
#define IO_AREA_MOB_AMB     "#AREA_MOB_AMB"
#define IO_REPOP_OLIST      "#REPOP_OLIST"
#define IO_REPOP_MOB_SHOPLIST "#REPOP_MOB_SHOPLIST"
#define IO_SCRIPT_VARS      "#SCRIPT_TEXT_VARS"

#define IO_ROOM_IDXS        "#ROOM_IDXS"
#define IO_ROOM_EXITS       "#R_EXITS"
#define IO_ROOM_EDESCS      "#R_EDESCS"
#define IO_ROOM_SPOTS       "#R_SPOTS"
#define IO_ROOM_SCRIPTS     "#R_SCRIPTS"

#define IO_OBJ_IDXS         "#OBJ_IDXS"
#define IO_OBJ_SCRIPTS      "#O_SCRIPTS"
#define IO_OBJ_SPOTS        "#O_SPOTS"

#define IO_MOB_IDXS         "#MOB_IDXS"
#define IO_MOB_SCRIPTS      "#M_SCRIPTS"
#define IO_MOB_ATTACKS      "#M_ATTACKS"
#define IO_MOB_PARTS        "#M_PARTS"
#define IO_MOB_SKINS        "#M_SKINS"
#define IO_MOB_SHOP         "#SHOP"
#define IO_MOB_SPOTS        "#M_SPOTS"

#define IO_PLAYER_PETS      "#PETS"
#define IO_OBJ_BANK         "#BANK"
#define IO_OBJ_INVENTORY    "#INV"
#define IO_OBJ_CONTENTS     "#CONTENTS"
#define IO_HFILE_HELPS      "#HELPS"
#define IO_SFILE_SOCIALS    "#SOCIALS"


//## Local Functions ##//
template <class Wants>
Wants*  LoadObject(MUDFile& hFp, Wants*& wNew);
template <class Wants>
Wants*  LoadObject(MUDFile& hFp, Wants*& wNew, CHAR* hWord);
template <class Wants>
bool    LinkedListLoop(MUDFile& hFp, Wants*& wNew);



//## Classes ##//

class LoadOld
{
    public: LoadOld() {}
    public: virtual ~LoadOld() {}

    //** Class Functions **//
    public:
    static bool     LoadAnyFile(MUDFile& hFp);
    static bool     LoadGenericObject(MUDFile& hFp, LoadOld*& hLoadOn);
    static bool     LoadGenericObject(MUDFile& hFp, LoadOld*& hLoadOn, CHAR* hWord);
    
    //** Members Functions **//
    bool            LoadSection     (MUDFile& hFp); //** Loading **//
    bool            LoadLinkedList  (MUDFile& hFp);
    bool            LoadSectMain    (MUDFile& hFp);
        
    //** For Loading As A Single Object **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName) = 0;
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName) = 0;
    virtual void    BeforeLoad();
    virtual void    OnLoad();
};



//** Extension To Add Linked List Functionality **//

class LoadOldLL : public LoadOld
{
    //** Saving The Object In A Linked List **//
    virtual void    SaveAsList(MUDFile& hFp, CHAR* hType) = 0;
    
    //** Your Own Homebrewed Loading of the List **//
    //Object* Object::LoadAsList(File &fp);
    //Object* Object::LoadInventory(File &fp, Person *ch);
};




#endif //ifndef LOADOLD_H
