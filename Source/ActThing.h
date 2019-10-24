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
#ifndef ACTTHING_H
#define ACTTHING_H 1


//Specification:
//Every ActThing is required to identify itself with some sort of information
//when GetTextForCodeTo(...) passes a '?' as the hCode.


class ActThing
{
    //** De/constructors **//
    public: ActThing();
    public: virtual ~ActThing();
    
    //** Public Member Functions **//
    public:
    virtual STRINGCW GetTextForCodeTo(Person* hTo, CHAR hCode);


    //** Public Data Members **//
    public:
};



//Allows you to pass a string as an ActThing

class ActStr
    : public ActThing
{
    //** De/constructors **//
    public: ActStr(const CHAR* hStr)    { mStr = hStr; }
    public: virtual ~ActStr()           {}
    
    //Note:
    //You can pass this ActStr object off anywhere it expects a ActStr* using this
    //operator overload. We do this cause gcc "warns" about taking the address of a
    //temporary object in cases like this:
    // ActParseTo(hTo, nText, NULL, this, &ActStr("Foo")); which can now be written as
    // ActParseTo(hTo, nText, NULL, this, ActStr("Foo"));
    //
    public: operator ActStr*() { return this; }

    
    //** Public Member Functions **//
    public:
    virtual STRINGCW GetTextForCodeTo(Person* hTo, CHAR hCode) { return mStr; }

    //** Public Data Members **//
    public:
    STRINGCW mStr;
};




#endif //ifndef ACTTHING_H
