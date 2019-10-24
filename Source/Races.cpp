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
#include "Races.h"


//## Globals ##//
Races gRaces;



class RaceHuman : public Race
{
    public: RaceHuman();
};
class RaceAvariel : public Race
{
    public: RaceAvariel();
};
class RaceDwarf : public Race
{
    public: RaceDwarf();
};
class RaceElf : public Race
{
    public: RaceElf();
};
class RaceFaery : public Race
{
    public: RaceFaery();
};
class RaceHobbit : public Race
{
    public: RaceHobbit();
};
class RaceKzinti : public Race
{
    public: RaceKzinti();
};
class RaceMorph : public Race
{
    public: RaceMorph();
};
class RaceOrc : public Race
{
    public: RaceOrc();
};
class RaceTroll : public Race
{
    public: RaceTroll();
};
class RaceVampire : public Race
{
    public: RaceVampire();
};
class RaceVyan : public Race
{
    public: RaceVyan();
};





Races::Races()
{
    Add(kRaceHuman  , new RaceHuman()   );
    Add(kRaceAvariel, new RaceAvariel() );
    Add(kRaceDwarf  , new RaceDwarf()   );
    Add(kRaceElf    , new RaceElf()     );
    Add(kRaceFaery  , new RaceFaery()   );
    Add(kRaceHobbit , new RaceHobbit()  );
    Add(kRaceKzinti , new RaceKzinti()  );
    Add(kRaceOrc    , new RaceOrc()     );
    Add(kRaceTroll  , new RaceTroll()   );
    Add(kRaceVampire, new RaceVampire() );
    Add(kRaceVyan   , new RaceVyan()    );
}


Races::~Races()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



RaceHuman::RaceHuman()
{
    mAbrv           = "Hum";
    mName           = "Human";
    mDescription    = "Average at everything, can thrive in most any situation.";
    mID             = kRaceHuman;
    mSize           = kSizeNormal;
}


RaceAvariel::RaceAvariel()
{
    mAbrv           = "Avr";
    mName           = "Avariel";
    mDescription    = "Winged elves. Strong, very quick, and bright. Very fragile.";
    mID             = kRaceAvariel;
    mSize           = kSizeNormal;
}


RaceDwarf::RaceDwarf()
{
    mAbrv           = "Dwf";
    mName           = "Dwarf";
    mDescription    = "Small, stout warriors of the mountains. Slow but strong.";
    mID             = kRaceDwarf;
    mSize           = kSizeSmall;
}


RaceElf::RaceElf()
{
    mAbrv           = "Elf";
    mName           = "Elf";
    mDescription    = "Agile forest dwellers. Smart and wise, but weak in body.";
    mID             = kRaceElf;
    mSize           = kSizeNormal;
}


RaceFaery::RaceFaery()
{
    mAbrv           = "Fae";
    mName           = "Faery";
    mDescription    = "Winged creatures of beauty, enchanted with the most powerful magic in the realm.";
    mID             = kRaceFaery;
    mSize           = kSizeInsect;
}


RaceHobbit::RaceHobbit()
{
    mAbrv           = "Hob";
    mName           = "Hobbit";
    mDescription    = "Fleet-footed people of the hills. Wise but not smart.";
    mID             = kRaceHobbit;
    mSize           = kSizeSmall;
}


RaceKzinti::RaceKzinti()
{
    mAbrv           = "Kzn";
    mName           = "Kzinti";
    mDescription    = "Ferociously clawed feline race. Tough, agile, dim-witted.";
    mID             = kRaceKzinti;
    mSize           = kSizeLarger;
}


RaceOrc::RaceOrc()
{
    mAbrv           = "Orc";
    mName           = "Orc";
    mDescription    = "Short, tough, green-skinned mercenaries. Strong and greedy.";
    mID             = kRaceOrc;
    mSize           = kSizeSmall;
}


RaceTroll::RaceTroll()
{
    mAbrv           = "Trl";
    mName           = "Troll";
    mDescription    = "Massive brutish warriors. Stronger, tougher, slow, and dense.";
    mID             = kRaceTroll;
    mSize           = kSizeLarger;
}


RaceVampire::RaceVampire()
{
    mAbrv           = "Vam";
    mName           = "Vampire";
    mDescription    = "Creatures of the night with a hunger for blood.";
    mID             = kRaceVampire;
    mSize           = kSizeNormal;
}


RaceVyan::RaceVyan()
{
    mAbrv           = "Vyn";
    mName           = "Vyan";
    mDescription    = "Dark elves from underground citadels. Fast, smart, wise, weak.";
    mID             = kRaceVyan;
    mSize           = kSizeSmall;
}



