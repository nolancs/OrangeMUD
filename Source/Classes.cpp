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
#include "CommonTypes.h"
#include "Classes.h"


//## Globals ##//
Classes gClasses;



class ClassWarrior : public Class
{
    public: ClassWarrior();
};
/*class ClassPaladin : public Class
{
    public: ClassPaladin();
};
class ClassDemonKnight : public Class
{
    public: ClassDemonKnight();
};
class ClassRanger : public Class
{
    public: ClassRanger();
};
class ClassArcher : public Class
{
    public: ClassArcher();
};
class ClassRogue : public Class
{
    public: ClassRogue();
};
class ClassNinja : public Class
{
    public: ClassNinja();
};
class ClassPriest : public Class
{
    public: ClassPriest();
};
class ClassShaman : public Class
{
    public: ClassShaman();
};
class ClassDruid : public Class
{
    public: ClassDruid();
};
class ClassMagician : public Class
{
    public: ClassMagician();
};
class ClassElementalist : public Class
{
    public: ClassElementalist();
};
class ClassNecromancer : public Class
{
    public: ClassNecromancer();
};
class ClassPsion : public Class
{
    public: ClassPsion();
};
class ClassBard : public Class
{
    public: ClassBard();
};*/





Classes::Classes()
{
    Add(kClassWarrior       , new ClassWarrior()        );
/*  Add(kClassPaladin       , new ClassPaladin()        );
    Add(kClassDemonknight   , new ClassDemonKnight()    );
    Add(kClassRanger        , new ClassRanger()         );
    Add(kClassArcher        , new ClassArcher()         );
    Add(kClassRogue         , new ClassRogue()          );
    Add(kClassNinja         , new ClassNinja()          );
    Add(kClassPriest        , new ClassPriest()         );
    Add(kClassShaman        , new ClassShaman()         );
    Add(kClassDruid         , new ClassDruid()          );
    Add(kClassMagician      , new ClassMagician()       );
    Add(kClassElementalist  , new ClassElementalist()   );
    Add(kClassNecromancer   , new ClassNecromancer()    );
    Add(kClassPsion         , new ClassPsion()          );
    Add(kClassBard          , new ClassBard()           );  */
}


Classes::~Classes()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



ClassWarrior::ClassWarrior()
{
    mAbrv           = "War";
    mName           = "Warrior";
    mDescription    = "Burly fighters with war training second to none.";
    mID             = kClassWarrior;
}

/*
ClassPaladin::ClassPaladin()
{
    mAbrv           = "Pal";
    mName           = "Paladin";
    mDescription    = "Holy warriors of the church with powers from both.";
    mID             = kClassPaladin;
}


ClassDemonKnight::ClassDemonKnight()
{
    mAbrv           = "Dem";
    mName           = "Demon Knight";
    mDescription    = "Unholy warriors with powers from hell like lesser necromancy.";
    mID             = kClassDemonknight;
}


ClassRanger::ClassRanger()
{
    mAbrv           = "Ran";
    mName           = "Ranger";
    mDescription    = "Warriors of the wilds with druidic power over nature.";
    mID             = kClassRanger;
}


ClassArcher::ClassArcher()
{
    mAbrv           = "Arc";
    mName           = "Archer";
    mDescription    = "Masterful warriors of ranged combat and weaponry.";
    mID             = kClassArcher;
}


ClassRogue::ClassRogue()
{
    mAbrv           = "Rog";
    mName           = "Rogue";
    mDescription    = "Assassins and thieves adept at those things unlawful.";
    mID             = kClassRogue;
}


ClassNinja::ClassNinja()
{
    mAbrv           = "Nin";
    mName           = "Ninja";
    mDescription    = "Jackie Chan on crack with two katanas and an attitude.";
    mID             = kClassNinja;
}


ClassPriest::ClassPriest()
{
    mAbrv           = "Prt";
    mName           = "Priest";
    mDescription    = "Divine acolytes imbued with healing and destructive power.";
    mID             = kClassPriest;
}


ClassShaman::ClassShaman()
{
    mAbrv           = "Sha";
    mName           = "Shaman";
    mDescription    = "Witch-doctors proficient in wicked primeval magicks.";
    mID             = kClassShaman;
}


ClassDruid::ClassDruid()
{
    mAbrv           = "Drd";
    mName           = "Druid";
    mDescription    = "Guardian priests of nature with sway over the weather.";
    mID             = kClassDruid;
}


ClassMagician::ClassMagician()
{
    mAbrv           = "Mag";
    mName           = "Magician";
    mDescription    = "Summoners of things demonic, elemental, and arcane.";
    mID             = kClassMagician;
}


ClassElementalist::ClassElementalist()
{
    mAbrv           = "Elm";
    mName           = "Elementalist";
    mDescription    = "Arcane magi fighters with unrivaled destructive magic.";
    mID             = kClassElementalist;
}


ClassNecromancer::ClassNecromancer()
{
    mAbrv           = "Nec";
    mName           = "Necromancer";
    mDescription    = "Dark mystics dealing in death and sacrificial voodoo.";
    mID             = kClassNecromancer;
}


ClassPsion::ClassPsion()
{
    mAbrv           = "Psi";
    mName           = "Psion";
    mDescription    = "Delvers of mind altering pursuits and psychokinesis.";
    mID             = kClassPsion;
}


ClassBard::ClassBard()
{
    mAbrv           = "Bar";
    mName           = "Bard";
    mDescription    = "Minstrels with subtle lyrical and musical magic.";
    mID             = kClassBard;
}
*/

