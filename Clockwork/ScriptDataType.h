/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: April, 2001                                           [Crossplatform]
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
#ifndef SCRIPTDATATYPE_H
#define SCRIPTDATATYPE_H 1
#include "Script.h"

class ScriptDataType
{
    //** De/constructors **//
    public: ScriptDataType();
    public: virtual ~ScriptDataType();

    //** Public Member Functions **//
    public:
    virtual void OperatorEqual(void* hVoidVal);                 //Assigned From
    virtual void OperatorEqual(ScriptVariable* hVar) = 0;       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar) = 0;  //Equal To?
    

    //** Public Data Members **//
    public:
    void*   mValuePtr;
};


///////////////////////////////////////////////////////////////////////////////
// Basic Scripting Data Types Defines Below (Good Examples)
///////////////////////////////////////////////////////////////////////////////
enum    //kScriptVar_ Values 0 though 99 are reserved.
{
    kScriptDT_None = 0  ,
    kScriptDT_BOOL      ,
    kScriptDT_INT       , 
    kScriptDT_STRING
};
CreateDTFunc CreateDT_BOOL;
CreateDTFunc CreateDT_INT;
CreateDTFunc CreateDT_STRING;
extern ScriptDataTypeDef* gDTDef_None;
extern ScriptDataTypeDef* gDTDef_BOOL;
extern ScriptDataTypeDef* gDTDef_INT;
extern ScriptDataTypeDef* gDTDef_STRING;

class DataTypeBOOL : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeINT : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeSTRING : public ScriptDataType
{
    public:
    DataTypeSTRING();
    virtual void OperatorEqual(void* hVoidVal);             //Assigned From
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
    
    public:
    STRING  mString;
};

#endif //ifndef SCRIPTDATATYPE_H


