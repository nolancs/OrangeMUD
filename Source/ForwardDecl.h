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
#ifndef FORWARDDECL_H
#define FORWARDDECL_H 1

//Clockwork Crossplatform Framework
#include "ForwardDeclCW.h"

//Clockwork MUD Classes
class ActThing;
class Ambience;
class Area;
class Attack;
class CombinedObjects;
class CombinedPeople;
class Exit;
class ExitPopulation;
class Fight;
class Help;
class HelpFile;
class Immortal;
class Mobile;
class MobileIndex;
class MobilePopulation;
class MUD;
class Object;
class ObjectData;
class ObjectHolder;
class ObjectIndex;
class ObjectPopulation;
class ObjectProxy;
class OLC;
class OrangeScript;
class OrangeScriptBinary;
class OrangeScriptCompiler;
class OrangeScriptVariable;
class OrangeSession;
class OrangeTCP;
class Person;
class PersonHolder;
class Player;
class Population;
class Part;
class RoomIndex;
class SaveLoadable;
class ScriptedThing;
class Skin;
class Social;
class SocialFile;
class Spot;
class SpottedThing;
class StringCW;
class TEditor;
class UIDHolder;
class VRefThing;
class XMLAttributes;
class XMLTag;
class XMLFile;
class XMLWriter;
class XMLReader;


//Lists
typedef List<Person*>               PersonList;
typedef List<OrangeSession*>        SessionList;
typedef List<Player*>               PlayerList;
typedef List<Mobile*>               MobileList;
typedef List<Object*>               ObjectList;
typedef List<Area*>                 AreaList;
typedef List<HelpFile*>             HelpFileList;
typedef List<Help*>                 HelpList;
typedef List<SocialFile*>           SocialFileList;
typedef List<Social*>               SocialList;
typedef List<Spot*>                 SpotList;
typedef List<Fight*>                FightList;
typedef List<UID>                   UIDList;
typedef List<OrangeScriptVariable*> OScriptVariableList;
//Arrays
typedef Vector<Part*>               PartArray;
typedef Vector<Attack*>             AttackArray;
typedef Vector<Skin*>               SkinArray;
typedef Vector<Ambience*>           AmbienceArray;
typedef Vector<Population*>         PopulationArray;
typedef Vector<ObjectPopulation*>   OPopulationArray;
//Containers
typedef Vector<PersonList*>         PersonHash;
typedef Map<VNUM,ObjectList*>       ObjectHash;
typedef Map<VNUM,Area*>             AreaMap;
typedef Map<VNUM,RoomIndex*>        IRoomMap;
typedef Map<VNUM,ObjectIndex*>      IObjectMap;
typedef Map<VNUM,MobileIndex*>      IMobileMap;
typedef Map<VNUM,OrangeScript*>     OScriptMap;
//Iterators
typedef PersonList::iterator                PersonIter;
typedef PlayerList::iterator                PlayerIter;
typedef MobileList::iterator                MobileIter;
typedef ObjectList::iterator                ObjectIter;
typedef SessionList::iterator               SessionIter;
typedef SpotList::iterator                  SpotIter;
typedef FightList::iterator                 FightIter;
typedef UIDList::iterator                   UIDIter;
typedef OScriptVariableList::iterator       OScriptVariableIter;
typedef IRoomMap::iterator                  IRoomIter;
typedef IObjectMap::iterator                IObjectIter;
typedef IMobileMap::iterator                IMobileIter;
typedef AreaMap::iterator                   AreaIter;
typedef OScriptMap::iterator                OScriptIter;
//Const Iterators
typedef PersonList::const_iterator          PersonCIter;
typedef PlayerList::const_iterator          PlayerCIter;
typedef MobileList::const_iterator          MobileCIter;
typedef ObjectList::const_iterator          ObjectCIter;
typedef SessionList::const_iterator         SessionCIter;
typedef SpotList::const_iterator            SpotCIter;
typedef FightList::const_iterator           FightCIter;
typedef UIDList::const_iterator             UIDCIter;
typedef OScriptVariableList::const_iterator OScriptVariableCIter;
typedef IRoomMap::const_iterator            IRoomCIter;
typedef IObjectMap::const_iterator          IObjectCIter;
typedef IMobileMap::const_iterator          IMobileCIter;
typedef AreaMap::const_iterator             AreaCIter;
typedef OScriptMap::const_iterator          OScriptCIter;
//Groups of Iters
typedef Vector<PersonIter>                  PersonIters;
typedef Vector<ObjectIter>                  ObjectIters;



//Old Load
class OldLoad;
class OldLoadLL;
class MUDFile;


#endif //ifndef FORWARDDECL_H
