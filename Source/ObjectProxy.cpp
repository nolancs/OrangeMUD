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
#include "ObjectIndex.h"
#include "ObjectProxy.h"


ObjectProxy::ObjectProxy()
    : mObject(NULL)
{
}




ObjectProxy::~ObjectProxy()
{
}



bool ObjectProxy::IsCustom()
{
    return mCustomObject.IsNull() ? false : true;
}



void ObjectProxy::Customize()
{
    if(mCustomObject.Ptr())
        return;
    
    APtr<ObjectData> nCustom(new ObjectData(GetIndex()));
    
    mCustomObject   = nCustom.Detach();
    mObject         = mCustomObject;
}




void ObjectProxy::Revert()
{
    mCustomObject.Dispose();
    mObject = GetIndex()->GetFlyweight();
    ASSERT(mObject);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




const STRINGCW& ObjectProxy::Name() const
{
    return mObject->mName;
}




const STRINGCW& ObjectProxy::PName() const
{
    return mObject->mPName;
}




const STRINGCW& ObjectProxy::Keywords() const
{
    return mObject->mKeywords;
}




const STRINGCW& ObjectProxy::ShortDesc() const
{
    return mObject->mShortDesc;
}




const STRINGCW& ObjectProxy::PShortDesc() const
{
    return mObject->mPShortDesc;
}




const STRINGCW& ObjectProxy::Description() const
{
    return mObject->mDescription;
}



LONG ObjectProxy::Value(SHORT hIndex) const
{
    return mObject->mValue[hIndex];
}



LONG ObjectProxy::WearFlags() const
{
    return GetIndex()->mWearFlags;
}


SHORT ObjectProxy::ItemType() const
{
    return GetIndex()->mItemType;
}


float ObjectProxy::Weight() const
{
    return mObject->mWeight;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




STRINGCW& ObjectProxy::xName()
{
    Customize();
    return mObject->mName;
}




STRINGCW& ObjectProxy::xPName()
{
    Customize();
    return mObject->mPName;
}




STRINGCW& ObjectProxy::xKeywords()
{
    Customize();
    return mObject->mKeywords;
}




STRINGCW& ObjectProxy::xShortDesc()
{
    Customize();
    return mObject->mShortDesc;
}




STRINGCW& ObjectProxy::xPShortDesc()
{
    Customize();
    return mObject->mPShortDesc;
}




STRINGCW& ObjectProxy::xDescription()
{
    Customize();
    return mObject->mDescription;
}



LONG& ObjectProxy::xValue(SHORT hIndex)
{
    Customize();
    return mObject->mValue[hIndex];
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




ObjectData::ObjectData(ObjectIndex* hLikeObj)
{
    ASSERT(hLikeObj);
    
    mName           = hLikeObj->mName;
    mPName          = hLikeObj->mPName;
    mKeywords       = hLikeObj->mKeywords;
    mShortDesc      = hLikeObj->mShortDesc;
    mPShortDesc     = hLikeObj->mPShortDesc;
    mDescription    = hLikeObj->mDescription;

    mItemType       = hLikeObj->mItemType;
    mLevel          = hLikeObj->mLevel;
    mWeight         = hLikeObj->mWeight;
    mMaterials      = hLikeObj->mMaterials;
    mAntiFlags      = hLikeObj->mAntiFlags;
    mObjectFlags    = hLikeObj->mObjectFlags;
    mValue[0]       = hLikeObj->mValue[0];
    mValue[1]       = hLikeObj->mValue[1];
    mValue[2]       = hLikeObj->mValue[2];
    mValue[3]       = hLikeObj->mValue[3];
    mValue[4]       = hLikeObj->mValue[4];
    
    //Only exist in Customized Objects:
    mAge            = 0;
    mOwner          = "";
    mTimer          = 0;
    mEnchantment    = 0;
}



ObjectData::~ObjectData()
{
}


