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
#include "CommonTypesCW.h"
#include "ScriptDataType.h"
#include "ScriptVariable.h"



ScriptDataType::ScriptDataType()
{
}


ScriptDataType::~ScriptDataType()
{
}


void ScriptDataType::OperatorEqual(void* hVoidVal)
{
    mValuePtr = hVoidVal;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ScriptDataType* CreateDT_BOOL()
{
    return new DataTypeBOOL();
}

void DataTypeBOOL::OperatorEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mType == kScriptDT_BOOL);
    ASSERT(hVar->mValue != NULL);
    mValuePtr = hVar->mValue->mValuePtr;
}

bool DataTypeBOOL::OperatorEqualEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mValue != NULL);
    return mValuePtr == hVar->mValue->mValuePtr;
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ScriptDataType* CreateDT_INT()
{
    return new DataTypeINT();
}

void DataTypeINT::OperatorEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mType == kScriptDT_INT);
    ASSERT(hVar->mValue != NULL);
    mValuePtr = hVar->mValue->mValuePtr;
}

bool DataTypeINT::OperatorEqualEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mValue != NULL);
    return mValuePtr == hVar->mValue->mValuePtr;
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ScriptDataType* CreateDT_STRING()
{
    return new DataTypeSTRING();
}

DataTypeSTRING::DataTypeSTRING()
{
    mValuePtr = &mString;
}

void DataTypeSTRING::OperatorEqual(void* hVoidVal)
{
    if(hVoidVal)
        mString = *((STRING*) hVoidVal); //hVoidVal can only be a STRING ptr
}


void DataTypeSTRING::OperatorEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mType == kScriptDT_STRING);
    ASSERT(hVar->mValue != NULL);
    mString = *((STRING*) hVar->mValue->mValuePtr);
}

bool DataTypeSTRING::OperatorEqualEqual(ScriptVariable* hVar)
{
    ASSERT(hVar->mType == kScriptDT_STRING);
    ASSERT(hVar->mValue != NULL);
    return mString == *((STRING*) hVar->mValue->mValuePtr);
}


