/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: December 2001                                         [Crossplatform]
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
#ifndef RACES_H
#define RACES_H

class Race
{
    public: Race() {}
    public: virtual ~Race() {}


    //** Public Member Functions **//
    public:
    STRINGCW&       GetAbrv() { return mAbrv; }
    STRINGCW&       GetName() { return mName; }
    STRINGCW&       GetDescription() { return mDescription; }
    BYTE            GetID() { return mID; }
    BYTE            GetSize() { return mSize; }



    //** Public Data Members **//
    public:
    STRINGCW        mAbrv;
    STRINGCW        mName;
    STRINGCW        mDescription;
    BYTE            mID;
    BYTE            mSize;
};




class Races
    : public Map<BYTE, Race*>
{
    public: Races();
    public: virtual ~Races();
};




//## Globals ##//
extern Races gRaces;





#endif //ifndef RACES_H
