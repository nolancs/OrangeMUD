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
#ifndef OLC_H
#define OLC_H 1


class OLC
{
    //** De/constructors **//
    public: OLC(Immortal& hBuilder);
    public: virtual ~OLC();
    

    //** Static Member Functions **//
    static bool HasPermission(Person* hCh, Area* hArea);
    static void ModifyPermission(Person* hCh, Area* hArea);

    //** Public Member Functions **//
    public:
    bool                    CanExitOLC();
    void                    Msg(const CHAR* hMsg, ...);
    void                    AreaChanged();
    Area*                   GetOpenArea();
    MobileIndex*            GetOpenMobile();
    ObjectIndex*            GetOpenObject();
    RoomIndex*              GetOpenRoom();
    OrangeScript*&          GetOpenScript()             {return mCurScript;}
    OrangeScriptVariable*&  GetOpenScriptText()         {return mCurScriptText;}
    void                    WorkingWithRoomScripts()    {mCurScript=mRoomScript;mCurScriptText=mRoomScriptText;};
    void                    WorkingWithObjectScripts()  {mCurScript=mObjectScript;mCurScriptText=mObjectScriptText;};
    void                    WorkingWithMobileScripts()  {mCurScript=mMobileScript;mCurScriptText=mMobileScriptText;};

    //** Area Editing **//
    void    AEditOpen       (STRINGCW& hArg);
    void    AEditClose      (STRINGCW& hArg, bool hCloseNow);
    void    AEditSave       (STRINGCW& hArg);
    void    AEditNew        (STRINGCW& hArg);
    void    ASetName        (STRINGCW& hArg);
    void    ASetCredits     (STRINGCW& hArg);
    void    ASetLow         (STRINGCW& hArg);
    void    ASetHigh        (STRINGCW& hArg);
    void    ASetRespawn     (STRINGCW& hArg);

    //** Room Editing **//
    void    REditNew        (STRINGCW& hArg);
    void    REditDelete     (STRINGCW& hArg);
    void    RSetName        (STRINGCW& hArg);
    void    RSetDescription (STRINGCW& hArg);
    void    RSetFlags       (STRINGCW& hArg);
    void    RSetTerrain     (STRINGCW& hArg);
    void    RSetHit         (STRINGCW& hArg);
    void    RSetEnergy      (STRINGCW& hArg);
    void    RSetMove        (STRINGCW& hArg);
    void    RSetAmbience    (STRINGCW& hArg);
    void    RSetCapacity    (STRINGCW& hArg);
    //------RSetExit's-Functions------//
    void    RSetExit        (STRINGCW& hArg);
    void    RSetExitStat    (STRINGCW& hArg);
    void    RSetExitNew     (STRINGCW& hArg, SHORT hInDir);
    void    RSetExitDesc    (STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitLink    (STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitKey     (STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitName    (STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitKeywords(STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitDelete  (STRINGCW& hArg, SHORT hInDir, Exit* hExit);
    void    RSetExitFlags   (STRINGCW& hArg, SHORT hInDir, Exit* hExit);

    //** Mobile Editing **//
    void    MEditOpen       (STRINGCW& hArg);
    void    MEditClose      (STRINGCW& hArg);
    void    MEditNew        (STRINGCW& hArg);
    void    MEditDelete     (STRINGCW& hArg);
    void    MEditCopy       (STRINGCW& hArg);
    void    MSetStrings     (STRINGCW& hArg, SHORT hOpt);
    void    MSetPosition    (STRINGCW& hArg);
    void    MSetLevel       (STRINGCW& hArg);
    void    MSetExperience  (STRINGCW& hArg);
    void    MSetSize        (STRINGCW& hArg);
    void    MSetSex         (STRINGCW& hArg);
    void    MSetAPR         (STRINGCW& hArg);
    void    MSetFlags       (STRINGCW& hArg);
    void    MSetHpEnMv      (STRINGCW& hArg, SHORT hOpt);
    void    MSetStats       (STRINGCW& hArg, SHORT hOpt);
    void    MSetResist      (STRINGCW& hArg);
    void    MSetRidable     (STRINGCW& hArg);
    //------MSetAttack-Functions--------//
    void    MSetAttack      (STRINGCW& hArg);
    void    MSetAttackNew       (STRINGCW& hArg);
    void    MSetAttackList      (STRINGCW& hArg);
    void    MSetAttackDelete    (STRINGCW& hArg, Attack* hAttack);
    void    MSetAttackChance    (STRINGCW& hArg, Attack* hAttack);
    void    MSetAttackType      (STRINGCW& hArg, Attack* hAttack);
    void    MSetAttackFlags     (STRINGCW& hArg, Attack* hAttack);
    void    MSetAttackDamage    (STRINGCW& hArg, Attack* hAttack);
    void    MSetAttackHitDam    (STRINGCW& hArg, Attack* hAttack, SHORT hOpt);
    void    MSetAttackMessage   (STRINGCW& hArg, Attack* hAttack);
    //------MSetPart-Functions----------//
    void    MSetPart        (STRINGCW& hArg);
    void    MSetPartNew     (STRINGCW& hArg);
    void    MSetPartList    (STRINGCW& hArg);
    void    MSetPartMap     (STRINGCW& hArg);
    void    MSetPartDelete  (STRINGCW& hArg, Part* hPart);
    void    MSetPartAC      (STRINGCW& hArg, Part* hPart);
    void    MSetPartSize    (STRINGCW& hArg, Part* hPart);
    void    MSetPartName    (STRINGCW& hArg, Part* hPart);
    //------MSetSkin-Functions----------//
    void    MSetSkin        (STRINGCW& hArg);
    void    MSetSkinNew     (STRINGCW& hArg);
    void    MSetSkinList    (STRINGCW& hArg);
    void    MSetSkinDelete  (STRINGCW& hArg, Skin* hSkin);
    void    MSetSkinVRef    (STRINGCW& hArg, Skin* hSkin);
    void    MSetSkinChance  (STRINGCW& hArg, Skin* hSkin);

    //** Object Editing **//
    void    OEditOpen       (STRINGCW& hArg);
    void    OEditClose      (STRINGCW& hArg);
    void    OEditNew        (STRINGCW& hArg);
    void    OEditDelete     (STRINGCW& hArg);
    void    OEditCopy       (STRINGCW& hArg);
    void    OSetStrings     (STRINGCW& hArg, SHORT hOpt);
    void    OSetMaterials   (STRINGCW& hArg);
    void    OSetExtraFlags  (STRINGCW& hArg);
    void    OSetWear        (STRINGCW& hArg);
    void    OSetType        (STRINGCW& hArg);
    void    OSetLevel       (STRINGCW& hArg);
    void    OSetWeight      (STRINGCW& hArg);
    void    OSetCost        (STRINGCW& hArg);
    void    OSetDurability  (STRINGCW& hArg);
    void    OSetItemLight       (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemWeapon      (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemArmor       (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemFurniture   (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemContainer   (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemDrinkCon    (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemFood        (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemReagent     (STRINGCW& hArg, SHORT hOpt);
    void    OSetItemInstrument  (STRINGCW& hArg, SHORT hOpt);

    //** Zone Editing **//
/*  void    ZEditNew        (STRINGCW& hArg);
    void    ZEditRedo       (STRINGCW& hArg);
    void    ZEditOpen       (STRINGCW& hArg);
    void    ZEditClose      (STRINGCW& hArg);
    void    ZEditDelete     (STRINGCW& hArg);
    void    ZEditSort       (STRINGCW& hArg);
    void    ZSetMobMobile   (STRINGCW& hArg, RepopMob *onPop);
    void    ZSetMobRoom     (STRINGCW& hArg, RepopMob *onPop);
    void    ZSetMobPosition (STRINGCW& hArg, RepopMob *onPop);
    void    ZSetMobChance   (STRINGCW& hArg, RepopMob *onPop);
    void    ZSetObjRoom     (STRINGCW& hArg, RepopObj *onPop);
    void    ZSetExitFlags   (STRINGCW& hArg, RepopExit *onPop);
    void    ZSetROL         (STRINGCW& hArg);
    void    ZSetShopROL     (STRINGCW& hArg);
    void    ZSetROLChance   (STRINGCW& hArg, RepopObjList *onROL);
    void    ZSetROLAmount   (STRINGCW& hArg, RepopObjList *onROL);
*/  
    //** Help Editing **//
/*  void    HEditFOpen      (STRINGCW& hArg);
    void    HEditFClose     (STRINGCW& hArg, bool closeNow);
    void    HEditFNew       (STRINGCW& hArg);
    void    HEditOpen       (STRINGCW& hArg);
    void    HEditClose      (STRINGCW& hArg);
    void    HEditNew        (STRINGCW& hArg);
    void    HEditDelete     (STRINGCW& hArg);
    void    HEditSave       (STRINGCW& hArg);
    void    HSetHName       (STRINGCW& hArg);
    void    HSetLevel       (STRINGCW& hArg);
    void    HSetCategory    (STRINGCW& hArg);
    void    HSetTitle       (STRINGCW& hArg);
    void    HSetKeywords    (STRINGCW& hArg);
    void    HSetSyntax      (STRINGCW& hArg);
    void    HSetSummary     (STRINGCW& hArg);
    void    HSetText        (STRINGCW& hArg);
    void    HSetSeeAlso     (STRINGCW& hArg);
*/  
    //** Social Editing **//
/*  void    SEditFOpen      (STRINGCW& hArg);
    void    SEditFClose     (STRINGCW& hArg, bool hCloseNow);
    void    SEditFNew       (STRINGCW& hArg);
    void    SEditOpen       (STRINGCW& hArg);
    void    SEditClose      (STRINGCW& hArg);
    void    SEditNew        (STRINGCW& hArg);
    void    SEditDelete     (STRINGCW& hArg);
    void    SEditSave       (STRINGCW& hArg);
    void    SSetSName       (STRINGCW& hArg);
    void    SSetName        (STRINGCW& hArg);
    void    SSetStrings     (STRINGCW& hArg, SHORT hOpt);
*/
    //** Script Editing **//
    void    ScriptList      (STRINGCW& hArg, ScriptedThing* hList);
    void    ScriptOpen      (STRINGCW& hArg, ScriptedThing* hList);
    void    ScriptClose     (STRINGCW& hArg);
    void    ScriptNew       (STRINGCW& hArg, ScriptedThing* hList);
    void    ScriptDelete    (STRINGCW& hArg, ScriptedThing* hList);
    void    ScriptCode      (STRINGCW& hArg);
    void    ScriptCompile   (STRINGCW& hArg);
    void    ScriptTrigger   (STRINGCW& hArg);
    void    ScriptCommands  (STRINGCW& hArg);
    void    ScriptKeywords  (STRINGCW& hArg);
    void    ScriptText      (STRINGCW& hArg);
    void    ScriptTextNew   (STRINGCW& hArg);
    void    ScriptTextDelete(STRINGCW& hArg);
    void    ScriptTextOpen  (STRINGCW& hArg);
    void    ScriptTextClose (STRINGCW& hArg);
    void    ScriptTextList  (STRINGCW& hArg);
    void    ScriptTextEdit  (STRINGCW& hArg);

    //** Spot Editing **//
/*  void    SpotList        (STRINGCW& hArg, SpotArray& hList);
    void    SpotNew         (STRINGCW& hArg, SpotArray& hList, Spot*& hOnto);
    void    SpotDelete      (STRINGCW& hArg, SpotArray& hList, Spot*& hOpenSpot);
    void    SpotOpen        (STRINGCW& hArg, SpotArray& hList, Spot*& hOnto);
    void    SpotClose       (STRINGCW& hArg, Spot*& hSpot);
    void    SpotDesc        (STRINGCW& hArg, Spot*& hSpot);
    void    SpotName        (STRINGCW& hArg, Spot*& hSpot);
    void    SpotKeywords    (STRINGCW& hArg, Spot*& hSpot);
    void    SpotFlag        (STRINGCW& hArg, Spot*& hSpot);
*/  
    //** Shop Editing **//
/*  void    ShopStat        (STRINGCW& hArg);
    void    ShopCreate      (STRINGCW& hArg);
    void    ShopDestroy     (STRINGCW& hArg);
    void    ShopSetFlags    (STRINGCW& hArg);
    void    ShopSetItemMod  (STRINGCW& hArg, SHORT hOpt);
*/

    
    
    
    //** Runtime Data Members **//
    public:
    Immortal&               mBuilder;
    Area*                   mArea;
    MobileIndex*            mMobile;
    ObjectIndex*            mObject;
    OrangeScript*           mRoomScript;
    OrangeScriptVariable*   mRoomScriptText;
    OrangeScript*           mMobileScript;
    OrangeScriptVariable*   mMobileScriptText;
    OrangeScript*           mObjectScript;
    OrangeScriptVariable*   mObjectScriptText;
    OrangeScript*&          mCurScript;
    OrangeScriptVariable*&  mCurScriptText;
};


enum
{
    kOLC_Check_OLC,
    kOLC_Check_HelpFile,
    kOLC_Check_SocialFile,
    kOLC_Check_Help,
    kOLC_Check_Social,
    kOLC_Check_Area,
    kOLC_Check_RoomIndex,
    kOLC_Check_ObjectIndex,
    kOLC_Check_MobileIndex,
    kOLC_Check_Script,
    kOLC_Check_ScriptText,
    kOLC_Check_Spot,
    kOLC_Check_Population
};


#define ARGUMENT_REQUIRED(x) if(x.Empty()){mBuilder.Send("An argument is required.\n");return;}

void Do_Script(Immortal* hCh, STRINGCW& hArg, ScriptedThing* hOn, BYTE hCheck);



#endif //ifndef OLC_H
