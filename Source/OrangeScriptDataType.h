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
#ifndef ORANGESCRIPTDATATYPE_H
#define ORANGESCRIPTDATATYPE_H 1
#include "ScriptDataType.h"


///////////////////////////////////////////////////////////////////////////////
// Basic Scripting Data Types Defines Below (Good Examples)
///////////////////////////////////////////////////////////////////////////////
enum    //kScriptVar_ Values 0 though 99 are reserved.
{
    kScriptDT_PERSON = 100  ,
    kScriptDT_OBJECT        ,
    kScriptDT_ROOM          ,
    kScriptDT_DIR           ,
    kScriptDT_AFFECT    
};
CreateDTFunc CreateDT_PERSON;
CreateDTFunc CreateDT_OBJECT;
CreateDTFunc CreateDT_ROOM;
CreateDTFunc CreateDT_DIR;
CreateDTFunc CreateDT_AFFECT;
extern ScriptDataTypeDef* gDTDef_PERSON;
extern ScriptDataTypeDef* gDTDef_OBJECT;
extern ScriptDataTypeDef* gDTDef_ROOM;
extern ScriptDataTypeDef* gDTDef_DIR;
extern ScriptDataTypeDef* gDTDef_AFFECT;

class DataTypePERSON : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeOBJECT : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeROOM : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeDIR : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

class DataTypeAFFECT : public ScriptDataType
{
    public:
    virtual void OperatorEqual(ScriptVariable* hVar);       //Assigned From
    virtual bool OperatorEqualEqual(ScriptVariable* hVar);  //Equal To? 
};

#endif //ifndef ORANGESCRIPTDATATYPE_H


