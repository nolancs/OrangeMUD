/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: April, 2002                                           [Crossplatform]
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
#include "XMLFile.h"
#include "OSUtils.h"
#include <ctype.h>




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//  Copying
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLAttributes::XMLAttributes(XMLAttributes& copyThis)
{
    for(XMLAttributesIter i = copyThis.begin(); i != copyThis.end(); ++i)
        push_back(new XMLAttribute(**i));
}


XMLAttributes& XMLAttributes::operator=(const XMLAttributes& copyThis)
{
    for(XMLAttributes::const_iterator i = copyThis.begin(); i != copyThis.end(); ++i)
        push_back(new XMLAttribute(**i));
    return *this;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::operator[]
//
//   Desc:: Get the value of a key quickly using Attrs["Key"]; It will throw
//          an exception if the key isn't found. Find() does the same thing
//          but returns the XMLAttribute ptr and can optionally throw.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING& XMLAttributes::operator[](const CHAR* hKey)
{
    return Find(hKey, true)->second;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::Find
//
//   Desc:: Locates a XMLAttribute inside this object. Does the same thing as
//          operator[] but it doesn't throw by default and it returns the
//          XMLAttribute ptr instead of a reference to its value.
//
//  Input:: hKey: The name of the key you're looking for in the attributes.
//          hThrow = false: Throw an exception if you can't find the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLAttribute* XMLAttributes::Find(const CHAR* hKey, bool hThrow)
{
    for(XMLAttributesIter i = begin(); i != end(); ++i)
    {
        if((*i)->first == hKey)
            return *i;
    }

    if(hThrow)
    {
        #if(DEBUG_XML)
        {
            printf("XML: No \"%s\" attribute found, throwing\n", hKey);
            fflush(stdout);
        }
        #endif
        
        ThrowXML(eXML_AttributeNotFound);
    }
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddString
//
//   Desc:: Adds an STRING attribute to the end of this XMLAttributes
//          object.
//
//  Input:: hKey: The name of the key.
//          hValue: The STRING value paired to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddString(const CHAR* hKey, const CHAR* hValue)
{
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second = hValue;
    push_back(nAttr);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddULong
//
//   Desc:: Adds an unsigned long attribute to the end of this XMLAttributes
//          object.
//
//  Input:: hKey: The name of the key.
//          hValue: The unsigned long value to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddULong(const CHAR* hKey, const ULONG hValue)
{
    CHAR nBuf[11];
    LONG nLen = sprintf(nBuf, "%lu", hValue);
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second.append(nBuf, nLen);
    push_back(nAttr);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddLong
//
//   Desc:: Adds a long attribute to the end of this XMLAttributes object.
//
//  Input:: hKey: The name of the key.
//          hValue: The long value to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddLong(const CHAR* hKey, const LONG hValue)
{
    CHAR nBuf[12];
    LONG nLen = sprintf(nBuf, "%lu", hValue);
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second.append(nBuf, nLen);
    push_back(nAttr);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddFloat
//
//   Desc:: Adds a floating point attribute to the end of this XMLAttributes
//          object.
//
//  Input:: hKey: The name of the key.
//          hValue: The floating point value to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddFloat(const CHAR* hKey, const float& hValue)
{
    CHAR nBuf[256];
    LONG nLen = sprintf(nBuf, "%f", hValue);
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second.append(nBuf, nLen);
    push_back(nAttr);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddDouble
//
//   Desc:: Adds a double attribute to the end of this XMLAttributes object.
//
//  Input:: hKey: The name of the key.
//          hValue: The double value to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddDouble(const CHAR* hKey, const double& hValue)
{
    CHAR nBuf[256];
    LONG nLen = sprintf(nBuf, "%lf", hValue);
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second.append(nBuf, nLen);
    push_back(nAttr);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLAttributes::AddBool
//
//   Desc:: Adds a boolean attribute to the end of this XMLAttributes object.
//
//  Input:: hKey: The name of the key.
//          hValue: The boolean value to be paired with the key.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLAttributes::AddBool(const CHAR* hKey, const bool hValue)
{
    XMLAttribute* nAttr = new XMLAttribute;
    nAttr->first = hKey;
    nAttr->second = hValue ? "true" : "false";
    push_back(nAttr);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLTag::XMLTag
//
//   Desc:: Constructor.
//
//  Input:: A pointer to the XMLTag that will be housing this one. Only the
//          top most XMLTag (baseTag) will have mInTag == NULL. The default
//          is NULL.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLTag::XMLTag(XMLTag* hInTag)
    : mInTag(hInTag)
    , mType(eXML_UndefinedTagType)
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLTag::~XMLFile
//
//   Desc:: Destructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLTag::~XMLTag()
{
    for(ULONG i = 0; i < mSubTags.size(); ++i)
        delete mSubTags[i];
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLTag::IsValueTag
//
//   Desc:: Tells you if this XMLTag contains an actual value, is a data leaf
//          in the XML tree. Or if it is a SuperTag, which is just a tag that
//          contains other tags.
//
// Output:: True if it's a ValueTag, false if it's a SuperTag
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLTag::IsValueTag()
{
    return !HasSubTags();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLTag::IsSuperTag
//
//   Desc:: Tells you if this XMLTag contains an actual value, is a data leaf
//          in the XML tree. Or if it is a SuperTag, which is just a tag that
//          contains other tags.
//
// Output:: True if it's a SuperTag, false if it's a ValueTag
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLTag::IsSuperTag()
{
    return HasSubTags();
}



const CHAR* XMLTag::AsCStr()
{
    return mValue.c_str();
}


const STRING& XMLTag::AsString()
{
    return mValue;
}


ULONG XMLTag::AsULong()
{
    ULONG nValue;
    if(sscanf(mValue.c_str(), "%lu", &nValue) != 1)
        ASSERT(!"Tag is not an unsigned long");
    return nValue;
}


LONG XMLTag::AsLong()
{
    LONG nValue;
    if(sscanf(mValue.c_str(), "%ld", &nValue) != 1)
        ASSERT(!"Tag is not a long");
    return nValue;
}


USHORT XMLTag::AsUShort()
{
    USHORT nValue;
    if(sscanf(mValue.c_str(), "%hu", &nValue) != 1)
        ASSERT(!"Tag is not an unsigned short");
    return nValue;
}


SHORT XMLTag::AsShort()
{
    SHORT nValue;
    if(sscanf(mValue.c_str(), "%hd", &nValue) != 1)
        ASSERT(!"Tag is not a short");  
    return  nValue;
}


BYTE XMLTag::AsByte()
{
    INT nValue;
    if(sscanf(mValue.c_str(), "%u", &nValue) != 1)
        ASSERT(!"Tag is not a byte");
    return (BYTE) nValue;
}


CHAR XMLTag::AsChar()
{
    INT nValue;
    if(sscanf(mValue.c_str(), "%d", &nValue) == 1)
    {
        return (CHAR) nValue;
    }
    return (CHAR) mValue[0];
}

UCHAR XMLTag::AsUChar()
{
    INT nValue;
    if(sscanf(mValue.c_str(), "%u", &nValue) == 1)
    {
        return (UCHAR) nValue;
    }
    return (UCHAR) mValue[0];
}


float XMLTag::AsFloat()
{
    float nValue = 0;
    if(sscanf(mValue.c_str(), "%f", &nValue) != 1)
        ASSERT(!"Tag is not a float");
    
    return nValue;
}



double XMLTag::AsDouble()
{
    double nValue = 0;
    if(sscanf(mValue.c_str(), "%lf", &nValue) != 1)
        ASSERT(!"Tag is not a double");
    return nValue;
}



bool XMLTag::AsBool()
{
    return (mValue == "true" || mValue == "1");
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::XMLWriter
//
//   Desc:: Constructor.
//
//  Input:: A pointer to a valid FileLink. The XMLWriter object uses the
//          stream to write XML and does not close/delete it.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLWriter::XMLWriter(FileLink* hOurLink)
    : mBegun(false)
    , mStream(hOurLink)
    , mOnANewLine(true)
    , mCompact(false)
    , mIdentIncrement(kXML_Indent)
{
    ASSERT(mStream != NULL);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::~XMLWriter
//
//   Desc:: Destructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XMLWriter::~XMLWriter()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::SetCompact
//
//   Desc:: Control over the compact mode of writing XML. The compact mode
//          skips all the pretty whitespace indenting and newlines that are
//          added by default to help maintain a level of human readability in
//          the generated output.
//
//  Input:: On or Off.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::SetCompact(bool hOn)
{
    mCompact = hOn;
    
    if(mCompact)
    {
        mIndent = "";
        mIdentIncrement = "";
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::SetLink
//
//   Desc:: Changes the FileLink associated with the XMLWriter.
//
//  Input:: The FileLink you want to associate with the XMLWriter.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::SetLink(FileLink* hStream)
{
    mStream = hStream;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::BeginFile
//
//   Desc:: Called once before you start writing XML tags to a file. It writes
//          the XML version number declaration and any default comments you
//          want to specify in this function.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void XMLWriter::BeginFile()
{
    ASSERT(!mBegun);
    
    mBegun = true;
    WriteToFile("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    BlankLine();
    BlankLine();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::EndFile
//
//   Desc:: Called when you're done with a XMLWriter. It checks to make sure
//          you've closed all your XML tags, it will ASSERT if you haven't.
//          It wont close the FileLink for you.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::EndFile()
{
    ASSERT(mBegun);
    ASSERT(mTagNameStack.empty());
    
    
    //Reset To Pristine
    mBegun      = false;
    mIndent     = "";
    mOnANewLine = true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::BlankLine
//
//   Desc:: Simply writes a new line into the file and sets our little value
//          that keeps track of if we're on a blank line, for human readability
//          formatting. You can safely call this inside any SuperTags, but if
//          put one inside a ValueTag you'll skrew with the formatting if it
//          happens to be text.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::BlankLine()
{
    if(!mCompact)
        WriteToFile("\n", 1);
    mOnANewLine = true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteXMLTag
//
//   Desc:: Writes a XMLTag to the FileLink object. If there are any tags
//          inside hTag it writes them to, using recursion to accomplish the
//          full write.
//
//  Input:: The tag to be written.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteXMLTag(XMLTag* hTag)
{
    ASSERT(hTag);
    
    //Begin The Tag
    if(hTag->IsSuperTag())
        BeginSuperTag(hTag->GetName().c_str(), &hTag->GetAttributes());
    else if(hTag->IsValueTag())
        BeginValueTag(hTag->GetName().c_str(), &hTag->GetAttributes());
    else if(hTag->IsCommentTag())
    {
        WriteCommentTag(hTag->GetValue().c_str());
        return;
    }
    
    //Write Value/SubTags
    if(hTag->HasSubTags())
    {
        for(ULONG i = 0; i < hTag->GetSubTags().size(); ++i)
            WriteXMLTag(hTag->GetSubTags()[i]);
    }
    else
    {
        WriteValueToFile(hTag->GetValue().data(), hTag->GetValue().length());
    }
    
    
    //End Tag
    EndTag();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteXMLFile
//
//   Desc:: Writes a new XML file to the FileLink object using whatever data is
//          stored in the XMLFile object as its model. 
//
//  Input:: The XMLFile object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteXMLFile(XMLFile* hFile)
{
    ASSERT(hFile);
    
    BeginFile();
    for(ULONG i = 0; i < hFile->mTags.size(); ++i)
        WriteXMLTag(hFile->mTags[i]);
    EndFile();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::BeginSuperTag
//
//   Desc:: Begin writing a SuperTag to the XML file. A "SuperTag" is just a
//          name for a tag that only contains other tags, rather than a value.
//          This call must be matched with a call to EndTag().
//
//  Input:: hTagName: The name of the tag.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::BeginSuperTag(const CHAR* hTagName, XMLAttributes* hAttrs)
{
    mTagNameStack.push(STRING(hTagName));
    mTagTypeStack.push(eXML_SuperTag);


    if(!mOnANewLine)
        BlankLine();
        
    WriteToFile(mIndent.c_str(), mIndent.length());
    WriteToFile("<", 1);
    WriteToFile(hTagName);

    if(hAttrs)
        WriteAttributes(hAttrs);
    
    WriteToFile(">", 1);
    BlankLine();

    mIndent += mIdentIncrement;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::BeginValueTag
//
//   Desc:: Begin writing a ValueTag to the XML file. A "ValueTag" is a tag
//          that only contains a textual value and NO subtags. ValueTags take
//          up a single line (like <Value>100</Value>). This call must be
//          matched with a call to EndTag() once the value is written.
//
//  Input:: hTagName: The name of the tag.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::BeginValueTag(const CHAR* hTagName, XMLAttributes* hAttrs)
{
    mTagNameStack.push(STRING(hTagName));
    mTagTypeStack.push(eXML_ValueTag);

    if(!mOnANewLine)
        BlankLine();

    WriteToFile(mIndent.c_str(), mIndent.length());
    WriteToFile("<", 1);
    WriteToFile(hTagName);

    if(hAttrs)
        WriteAttributes(hAttrs);
    
    WriteToFile(">", 1);

    mOnANewLine = false;
    mIndent += mIdentIncrement;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::EndTag
//
//   Desc:: Every call to BeginSuperTag() or BeginValueTag() must be matched
//          with a call to this function. It writes the </FooTag> ending or
//          "closing" tag into the XML file. The XMLWriter already knows the
//          name of the tag from when you began it so you don't need to specify
//          that again.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::EndTag()
{
    ASSERT(mTagNameStack.size());

    STRING          nTagName = mTagNameStack.top();
    eXMLTagType     nTagType = mTagTypeStack.top();
    
    //Remove from stack
    mIndent.erase(0, mIdentIncrement.length());
    mTagNameStack.pop();
    mTagTypeStack.pop();
    
    if(nTagType == eXML_SuperTag && !mOnANewLine)
        BlankLine();
    
    if(nTagType == eXML_SuperTag)
        WriteToFile(mIndent.c_str(), mIndent.length());
    
    WriteToFile("</", 2);
    WriteToFile(nTagName.c_str(), nTagName.length());
    WriteToFile(">", 1);
    BlankLine();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteCommentTag
//
//   Desc:: Writes an XML comment tag. They <!-- appear like this --> inside
//          the file. The comment isn't checked for being valid 7-bit ascii.
//
//  Input:: The comment string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteCommentTag(const CHAR* hComment)
{
    if(!mOnANewLine)
        BlankLine();

    WriteToFile(mIndent.c_str(), mIndent.length());
    WriteToFile("<!-- ", 5);
    WriteToFile(hComment);
    WriteToFile(" -->", 4);
    mOnANewLine = false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteEmptyTag
//
//   Desc:: Writes an empty XML tag. Empty tags appear <LikeThis/> but are no
//          different than <LikeThis></LikeThis> other than saving some space.
//
//  Input:: hTagName: The name of the tag.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteEmptyTag(const CHAR* hTagName, XMLAttributes* hAttrs)
{
    if(!mOnANewLine)
        BlankLine();

    WriteToFile(mIndent.c_str(), mIndent.length());
    WriteToFile("<", 1);
    WriteToFile(hTagName);

    if(hAttrs)
        WriteAttributes(hAttrs);
    
    WriteToFile("/>", 2);
    mOnANewLine = false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteHandle
//
//   Desc:: Write a Tag-Value pair where the value is an arbitrary chunk of
//          memory, which may or may not contain NULL '\0' characters.
//
//  Input:: hTagName: The name of the tag.
//          hValue: A ptr to the chunk data.
//          hLength: The length of the data.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteHandle(const CHAR* hTagName, const BYTE* hValue, LONG hLength, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueHandle(hValue, hLength);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteString
//
//   Desc:: Write a Tag-Value pair where the value is an STRING.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The STRING value.
//          hLength: How much of the STRING to write, -1 (default) for all.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteString(const CHAR* hTagName, const CHAR* hValue, LONG hLength, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueString(hValue, hLength);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteULong
//
//   Desc:: Write a Tag-Value pair where the value is an unsigned long.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The unsigned long value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteULong(const CHAR* hTagName, const ULONG hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueULong(hValue);
    EndTag();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteLong
//
//   Desc:: Write a Tag-Value pair where the value is a long.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The long value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteLong(const CHAR* hTagName, const LONG hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueLong(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteUShort
//
//   Desc:: Write a Tag-Value pair where the value is an unsigned short.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The unsigned short value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteUShort(const CHAR* hTagName, const USHORT hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueUShort(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteShort
//
//   Desc:: Write a Tag-Value pair where the value is a short.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The short value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteShort(const CHAR* hTagName, const SHORT hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueShort(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteByte
//
//   Desc:: Write a Tag-Value pair where the value is a byte.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The byte value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteByte(const CHAR* hTagName, const BYTE hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueByte(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteChar
//
//   Desc:: Write a Tag-Value pair where the value is a char.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The char value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteChar(const CHAR* hTagName, const CHAR hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueChar(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteFloat
//
//   Desc:: Write a Tag-Value pair where the value is a float.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The float value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteFloat(const CHAR* hTagName, const float& hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueFloat(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteDouble
//
//   Desc:: Write a Tag-Value pair where the value is a double.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The double value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteDouble(const CHAR* hTagName, const double& hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueDouble(hValue);
    EndTag();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteBool
//
//   Desc:: Write a Tag-Value pair where the value is a bool.
//
//  Input:: hTagName: The name of the tag.
//          hValue: The bool value.
//          hAttrs: An optional list of name/value attributes for the tag.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteBool(const CHAR* hTagName, const bool hValue, XMLAttributes* hAttrs)
{
    BeginValueTag(hTagName, hAttrs);
    WriteValueBool(hValue);
    EndTag();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueHandle
//
//   Desc:: Write a chunk of data into the value field of the current tag. The
//          data is base64 encoded because it cannot be stored in XML
//          otherwise.
//
//  Input:: hValue: The handle data.
//          hLength: The length of the handle.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueHandle(const BYTE* hValue, LONG hLength)
{
    if(hLength > 0)
    {
        std::vector<unsigned char> nBase64Buf;
        //Base64Encode(hValue, hLength, nBase64Buf);
        ASSERT(!"BASE64");
        WriteToFile((const char*) (&nBase64Buf[0]), nBase64Buf.size());
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueString
//
//   Desc:: Write a string of text into the value field of the current tag.
//          Multiple calls will continue writing where the last one left off so
//          you don't need to accomplish the entire write at once.
//
//  Input:: hValue: The string data.
//          hLength: How much of the string to write, -1 (default) for all.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueString(const CHAR* hValue, LONG hLength)
{
    WriteValueToFile(hValue, hLength);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueULong
//
//   Desc:: Write an unsigned long into the value field of the current tag.
//
//  Input:: hValue: The unsigned long.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueULong(const ULONG hValue)
{
    CHAR nBuf[11];
    LONG nLen = sprintf(nBuf, "%lu", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueLong
//
//   Desc:: Write a signed long into the value field of the current tag.
//
//  Input:: hValue: The long.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueLong(const LONG hValue)
{
    CHAR nBuf[12];
    LONG nLen = sprintf(nBuf, "%ld", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueUShort
//
//   Desc:: Write an unsigned short into the value field of the current tag.
//
//  Input:: hValue: The unsigned short.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueUShort(const USHORT hValue)
{
    CHAR nBuf[6];
    LONG nLen = sprintf(nBuf, "%hu", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueShort
//
//   Desc:: Write a signed short into the value field of the current tag.
//
//  Input:: hValue: The short.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueShort(const SHORT hValue)
{
    CHAR nBuf[7];
    LONG nLen = sprintf(nBuf, "%hd", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueByte
//
//   Desc:: Write an unsigned 8-bit value into the field of the current tag.
//
//  Input:: hValue: The byte.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueByte(const BYTE hValue)
{
    CHAR nBuf[7];
    LONG nLen = sprintf(nBuf, "%u", (unsigned int) hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueChar
//
//   Desc:: Write a signed 8-bit value into the field of the current tag.
//
//  Input:: hValue: The character.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueChar(const CHAR hValue)
{
    CHAR nBuf[7];
    LONG nLen = sprintf(nBuf, "%d", (signed int) hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueFloat
//
//   Desc:: Write a floating point value into the value field of the current 
//          tag.
//
//  Input:: hValue: The floating point value.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueFloat(const float& hValue)
{
    CHAR nBuf[256];
    LONG nLen = sprintf(nBuf, "%f", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueDouble
//
//   Desc:: Write a double value into the value field of the current tag.
//
//  Input:: hValue: The double.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueDouble(const double& hValue)
{
    CHAR nBuf[256];
    LONG nLen = sprintf(nBuf, "%lf", hValue);
    WriteToFile(nBuf, nLen);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueBool
//
//   Desc:: Write a boolean value into the value field of the current tag.
//
//  Input:: hValue: The bool.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueBool(const bool hValue)
{
    if(hValue)
        WriteToFile("true", 4);
    else
        WriteToFile("false", 5);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteToFile
//
//   Desc:: Our private text writing function. Simply writes text to the file.
//          You can specify a length to write otherwise it writes the whole
//          thing.
//
//  Input:: hText: The text to write.
//          hLen: How much of the text to write. Default is -1 (all).
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteToFile(const CHAR* hText, LONG hLen)
{
    ASSERT(mStream != NULL);
    
    if(hLen == -1)
        hLen = strlen(hText);
    
    mStream->Write((void*)hText, hLen);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteValueToFile
//
//   Desc:: Our private text value writing function. Writes values to the file.
//          You can specify a length to write otherwise it writes the whole
//          thing. It also converts special characters like > into &gt; and
//          such so the file conforms to XML standards and can be read
//          correctly by any parser.
//
//  Input:: hText: The text to write.
//          hLen: How much of the text to write. Default is -1 (all).
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteValueToFile(const CHAR* hText, LONG hLen)
{
    if(hLen == -1)
        hLen = strlen(hText);
    
    register CHAR c;
    for(LONG i = 0; i < hLen; ++i)
    {
        c = hText[i];
        
        //Normal printable characters
        if(c >= 32 || c == 9 || c == 10 || c == 13)
        {
            switch(hText[i])
            {
                case '&':
                    WriteToFile("&amp;", 5);
                    break;
                case '<':
                    WriteToFile("&lt;", 4);
                    break;
                case '>':
                    WriteToFile("&gt;", 4);
                    break;
                default:
                    WriteToFile(&hText[i], 1);
                    break;
            }
        }
        else  //High bit ASCII characters
        if(c < 0) 
        {
            char nBuf[10];
            unsigned int nVal = ((unsigned char)c);
            LONG nLen = sprintf(nBuf, "&#%u;", nVal);
            WriteToFile(nBuf, nLen);        
        }
        else //Unprintable lowbit ASCII characters
        {
            char nBuf[10];
            unsigned int nVal = ((unsigned char)c);
            LONG nLen = sprintf(nBuf, "&#%u;", nVal);
            WriteToFile(nBuf, nLen);        

            //printf(">> Writing unprintable ASCII character to XML (%u)\n", (USHORT) c);
            //fflush(stdout);

            //A call to isprint(c) would return false here...
            //If you hit this assert it means you're trying write bytes into the value
            //of some tag that cannot ever conform to the XML specification no matter
            //what form they're written in (even &#21; form).
            //I suggest reconsidering what you're writing and if possible using write
            //handle which will base64 encode the data into acceptable values for
            //embedding in XML.
        }
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLWriter::WriteAttributes
//
//   Desc:: Our private attribute writing function.
//
//  Input:: The attributes. They may NOT be NULL.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLWriter::WriteAttributes(XMLAttributes* hAttrs)
{
    for(XMLAttributesIter nScan = hAttrs->begin(); nScan != hAttrs->end(); ++nScan)
    {
        const STRING& nAttrName = (*nScan)->first;
        const STRING& nAttrValue = (*nScan)->second;

        WriteToFile(" ", 1);
        WriteToFile(nAttrName.c_str(), nAttrName.length());
        WriteToFile("=\"", 2);
        WriteToFile(nAttrValue.c_str(), nAttrValue.length());
        WriteToFile("\"", 1);
    }   
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


#define PROFILE_XML 1
//To Be Added:
// [  ]-Escape characters in attributes? <Tag attr="qu\"ot\"e"></Tag>
// [XX]-Reads empty tags <Tag/>
// [  ]-Handles CDATA sections
// [  ]-Multi-content tags?? is that even possible
// [  ]-Very "loose" in parsing Tag-Attributes (not precise)




XMLReader::XMLReader(FileLink* hLink)
    : mStream(*hLink)
    , mReadReady(true)
#if(XML_COUNT_LINES)
    , mOnLine(1)
#endif
{
    mReadingTag.reserve(160);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::OpenBaseTag
//
//   Desc:: The first call you should make to a XMLReader if you plan on using
//          the tag-by-tag reading method instead of a single ReadFile() call.
//          This function scans through the file and opens the base tag.
//
//          VERY IMPORTANT...
//          A successful OpenBaseTag() call must be matched with a call to
//          CloseTag() once the contents have been dealt with. After you've
//          opened the base tag YOU OWN ITS MEMORY and are responsible for
//          deleting it, but only AFTER you've closed it or aborted the reading
//          altogether, otherwise the XMLReader will be left with a dangling
//          pointer in its tag stack that may cause a crash with any further
//          parsing.
//
//  Input:: baseTag: The file's base tag on success.
//
// Output:: True on successful open of the base tag, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::OpenBaseTag(APtr<XMLTag>& baseTag)
{
    XMLTag fakeBaseTag; //so we can use OpenTag()
    mTagStack.push(&fakeBaseTag);
    XMLTag* scanTagPtr = NULL;


    ReadVersion();
    while(true)
    {
        if(OpenTag(scanTagPtr) == false)
            return false;
        
        if(scanTagPtr->IsCommentTag())
        {
            CloseTag();
            continue;
        }
        
        baseTag = scanTagPtr;
        break;
    }
    
    
    mTagStack.pop(); //pop off scanTagPtr (the true base tag)
    mTagStack.pop(); //pop off fakeBaseTag
    mTagStack.push(scanTagPtr); //add the true base tag onto the stack
    
    scanTagPtr->mInTag = NULL;
    XMLTagArray& foundTags = fakeBaseTag.GetSubTags();
    XMLTagArray::iterator findBase = std::find(foundTags.begin(), foundTags.end(), scanTagPtr);
    ASSERT(findBase != foundTags.end());
    foundTags.erase(findBase); //The baseTag APtr owns the memory now so we erase the
    //pointer to the baseTag from fakeBaseTag's list of subtags, otherwise it would delete it.
    //Any comment tags that were found are destructed when fakeBaseTag goes out of scope.
    
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::OpenTag
//
//   Desc:: Begins reading the next tag off the stream. If it encounters the
//          end of the current "parent" tag it returns false, CloseTag() must
//          then be called to complete the parent read. This call may only be
//          used after the base tag has been found with OpenBaseTag().
//
//          VERY IMPORTANT...
//          Every successful call to OpenTag() must ALWAYS be matched with a
//          call to CloseTag() once the contents have been dealt with. After
//          you've opened a tag ITS PARENT TAG OWNS ITS MEMORY and will delete
//          it once CloseTag()/CloseTag(true) is called on the parent tag or it
//          is destructed.
//
//  Input:: nextTag: Points to the newly open tag on success.
//
// Output:: True on successful open of the next tag, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::OpenTag(XMLTag*& nextTag)
{
    ASSERT(mTagStack.empty() == false);
    XMLTag* insideThis = mTagStack.top();
    APtr<XMLTag> openedTag;
    
    mReadingTag.erase();
    if(!ReadTillTag(mReadingTag, NULL))
        ThrowXML(eXML_PrematureEOF);


    //Found begin <tag>
    if(IsBeginTag(mReadingTag))
    {
        ParseTagString(mReadingTag, openedTag);
        openedTag->mType = eXML_ValueTag;
        openedTag->mInTag = insideThis;
        nextTag = openedTag;
        insideThis->GetSubTags().push_back(openedTag);
        mTagStack.push(openedTag.Detach());
        mEOTReachedStack.push(false);
        return true;
    }

    //Found end </tag>
    if(IsEndTag(mReadingTag))
    {
        if(!BeginTagMatchesEnd(insideThis->GetName(), mReadingTag))
        {
            printf("eXML_UnmatchingEndBeginTags(%s, %s)\n", insideThis->GetName().c_str(), mReadingTag.c_str());
            fflush(stdout);
            ThrowXML(eXML_UnmatchingEndBeginTags);
        }
        ASSERT(mTagStack.top() == insideThis);
        mEOTReachedStack.top() = true;
        return false;
    }   

    //Found empty <tag/>
    if(IsEmptyTag(mReadingTag))
    {
        ParseTagString(mReadingTag, openedTag);
        openedTag->mType = eXML_EmptyTag;
        openedTag->mInTag = insideThis;
        nextTag = openedTag;
        insideThis->GetSubTags().push_back(openedTag);
        mTagStack.push(openedTag.Detach());
        mEOTReachedStack.push(true);
        return true;
    }

    //Found comment <!-- tag -->
    if(IsCommentTag(mReadingTag))
    {
        mReadingTag.erase(0, 4);
        mReadingTag.erase(mReadingTag.length()-3, 3);
        openedTag = new XMLTag(insideThis);
        openedTag->GetValue() = mReadingTag;
        openedTag->mType = eXML_CommentTag;
        nextTag = openedTag;
        insideThis->GetSubTags().push_back(openedTag);
        mTagStack.push(openedTag.Detach());
        mEOTReachedStack.push(true);
        return true;
    }
    
    //Some )!#@* up tag we can't figure out?
    ThrowXML(eXML_BadTagWhereBeginTagExpected);
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ScanForTag
//
//   Desc:: An extension of OpenTag() that reads until it encounters a tag of a
//          given name. If it encounters the end of the current "parent" tag it
//          returns false, CloseTag() must then be called to complete the
//          parent read.
//
//          Every successful call to ScanForTag() is just like a successful
//          call to OpenTag(), and so it must ALWAYS be matched with a call to
//          CloseTag() once the contents have been dealt with.
//
//  Input:: tagName: Search for a tag of this name.
//          nextTag: Points to the newly open tag on success.
//          throwOnFail: throw an exception instead of returning false.
//
// Output:: True if tag was found, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ScanForTag(const CHAR* tagName, XMLTag** nextTag, bool throwOnFail)
{
    XMLTag* searchTag;
    LONG skippedTags = 0;
    
    while(OpenTag(searchTag))
    {
        if(searchTag->IsCommentTag())
        {
            CloseTag();
            continue;
        }
        
        if(searchTag->GetName() != tagName)
        {
            #if(DEBUG_XML)
            {
                printf("XML: Skipped <%s> @ line %ld\n", searchTag->GetName().c_str(), mOnLine);
                fflush(stdout);
            }
            #endif
            ++skippedTags;
            SkipTagContents();
            CloseTag();
            continue;
        }
        
        if(nextTag)
            (*nextTag) = searchTag;
        
        #if(DEBUG_XML)
        {
            printf("XML: Matched <%s> @ line %ld\n", tagName, mOnLine);
            fflush(stdout);
        }
        #endif
        return true;
    }
    
    if(throwOnFail)
    {
        #if(DEBUG_XML)
        {
            printf("XML: Couldn't find <%s>, throwing @ line %ld\n", tagName, mOnLine);
            fflush(stdout);
        }
        #endif
        ThrowXML(eXML_TagNotFound);
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadTagContents
//
//   Desc:: Reads values/subtags into the current XMLTag until the end of tag
//          is encountered. Then CloseTag() will have to be called to complete
//          the read.
//
//   Note:: This can be called on every type of tag (for consistency), even if
//          they don't have any real contents because they are <EmptyTags/> or 
//          <!-- Comment Tags -->.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLReader::ReadTagContents()
{
    bool& endOfCurrentTagReached = mEOTReachedStack.top();
    
    if(endOfCurrentTagReached == false)
    {
        ReadTillTagClose(mTagStack.top(), false);
        endOfCurrentTagReached = true;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::SkipTagContents
//
//   Desc:: Exactly like ReadTagContents() but used to read through and discard
//          the values/subtags of the current XMLTag because you do not intend
//          to use them.
//
//   Note:: This can be called on every type of tag (for consistency), even if
//          they don't have any real contents because they are <EmptyTags/> or 
//          <!-- Comment Tags -->.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLReader::SkipTagContents()
{
    bool& endOfCurrentTagReached = mEOTReachedStack.top();
    
    if(endOfCurrentTagReached == false)
    {
        ReadTillTagClose(mTagStack.top(), true);
        endOfCurrentTagReached = true;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::CloseTag
//
//   Desc:: Once the CurrentTagIsDone() you must close it with this call. By
//          default the sub tags are deleted, otherwise the entire XML tree
//          remains in memory as the file is read. If you pass in false the sub
//          tags stick around as long as this tag does.
//
//  Input:: freeSubTags = true: Delete the sub tags as it's closed?
//
//   Note:: This MUST be called on every type of tag (for consistency), even if
//          they don't have a </ClosingTag> because they are <EmptyTags/> or 
//          <!-- Comment Tags -->.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void XMLReader::CloseTag(bool freeSubTags)
{
    ASSERT(!mEOTReachedStack.empty());
    ASSERT(mEOTReachedStack.top() == true); //if this assert fires it means
    //you haven't read up till the closing of the current XMLTag and closing
    //it now would just )!@#* everything up.
    
    if(freeSubTags)
    {
        XMLTagArray& freeTags = mTagStack.top()->GetSubTags();
        for(ULONG i = 0; i < freeTags.size(); ++i)
            delete freeTags[i];
        freeTags.clear();
    }
    
    mEOTReachedStack.pop();
    mTagStack.pop();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadFile
//
//   Desc:: Reads an XML file from the FileLink object. The XML data heirarchy
//          is stored in the XMLFile object you pass in, so the XMLFile
//          should be untouched since construction.
//
//  Input:: An XML file object to build the parsed XML into.
//
// Output:: eXML_NoErr on success, an error otherwise.
//
//   Note:: Any commenting structure before the base tag gets discarded.
//          Comments inside the base tag will be read in as comment tags.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

eXMLError XMLReader::ReadFile(XMLFile* hXMLFile)
{
    ASSERT(hXMLFile);
    APtr<XMLTag> baseTag;

    try
    {
        if(OpenBaseTag(baseTag) == false)
            ThrowXML(eXML_PrematureEOF);
            
        ReadTagContents();
        CloseTag(false);        
        hXMLFile->mTags.push_back(baseTag.Detach());
    }
    catch(eXMLError hExceptionErr)
    {
        printf("%d exception XML thrown on read.\n", hExceptionErr);
        return hExceptionErr;
    }

    return eXML_NoErr;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadHandle
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a base 64 encoded handle.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: Points to the newly allocated handle memory on success.
//          hLength: Length of the newly allocated handle memory.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadHandle(const CHAR* hTagName, BYTE*& hValue, LONG& hLength, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        if(currTag->GetValue().length())
        {
            std::vector<unsigned char> base64Buf;
            ASSERT(!"BASE64");
//          if(!Base64Decode((const unsigned char*) currTag->GetValue().c_str(), currTag->GetValue().length(), base64Buf)
//          {
//              ThrowXML(eXML_InvalidBase64Handle);
//          }
            hLength = base64Buf.size();
            hValue = new BYTE[hLength];
            memcpy(hValue, &base64Buf[0], hLength);
        }
        else
        {
            hLength = 0;
            hValue = NULL;
        }
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadString
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as an STRING.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The STRING is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadString(const CHAR* hTagName, STRING& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->GetValue();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadULong
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as an unsigned long.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The unsigned long is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadULong(const CHAR* hTagName, ULONG& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsULong();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadLong
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a long.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The long is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadLong(const CHAR* hTagName, LONG& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsLong();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadUShort
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as an unsigned short.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The unsigned short is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadUShort(const CHAR* hTagName, USHORT& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsUShort();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadShort
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a short.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The short is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadShort(const CHAR* hTagName, SHORT& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsShort();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadByte
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a byte (unsigned char).
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The byte is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadByte(const CHAR* hTagName, BYTE& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsByte();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadChar
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a char.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The char is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadChar(const CHAR* hTagName, CHAR& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsChar();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadFloat
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a float.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The float is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadFloat(const CHAR* hTagName, float& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsFloat();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadDouble
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a double.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The double is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadDouble(const CHAR* hTagName, double& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsDouble();
        CloseTag();
        return true;
    }
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadBool
//
//   Desc:: An extension of ScanForTag() that reads until it encounters a tag
//          of a given name and then reads it in as a bool.
//
//  Input:: hTagName: Search for a tag of this name.
//          hValue: The bool is read into this.
//          hTag = NULL: Optionally points to the tag on success.
//          throwOnFail = true: Throw an exception on failure.
//
// Output:: True on success, otherwise end of parent tag was reached.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadBool(const CHAR* hTagName, bool& hValue, XMLTag** hTag, bool throwOnFail)
{
    if(ScanForTag(hTagName, hTag, throwOnFail))
    {
        XMLTag* currTag = CurrentTag();
        ReadTagContents();
        hValue = currTag->AsBool();
        CloseTag();
        return true;
    }
    return false;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadVersion
//
//   Desc:: Reads the XML version information off the stream. This assumes the
//          stream's position is at the beginning of the file because the
//          version information is always placed there.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLReader::ReadVersion()
{   
    ReadTillTag(mReadingTag, NULL);

    if(mReadingTag[0] != '?')
        ThrowXML(eXML_InvalidCommentTag);
    if(mReadingTag[mReadingTag.length()-1] != '?')
        ThrowXML(eXML_InvalidCommentTag);
        
        
    //ToDo:
    //Grab the document encoding here, just to be on the safe side. Must be
    //an ASCII encoding or UTF-8 unicode encoding because that is all this
    //class is setup to read.
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadTillTag
//
//   Desc:: A lower level way of reading until the next tag. It reads into
//          hValue (if its non-NULL) until it encounters a tag of any type.
//          Then it fills hTag with the text inside the newly encountered tag.
//          It is then up to you to determine what type of tag hTag represents
//          and then parse and handle it accordingly.
//
//  Input:: hTag: Read the contents of the next tag into this.
//          hValue: Read whatever is encountered until the next tag into this.
//
// Output:: True found a new tag, false for end of file.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool XMLReader::ReadTillTag(STRING& hTag, STRING* hValue)
{
    CHAR c;

    if(hValue)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Inline & translation
        /////////////////////////////////////////////////////////////////////////////////////
        while((c = GetC()) != '<')
        {
            if(!ReadReady())
                ThrowXML(eXML_PrematureEOF);

            if(c == '&')
            {
                mTransBuf.erase();
                while((c = GetC()) != ';')
                {
                    if(!ReadReady())
                        ThrowXML(eXML_PrematureEOF);
                    mTransBuf.operator+=(c);
                }
                
                if(mTransBuf == "gt")
                {
                    c = '>';
                }
                else 
                if(mTransBuf == "lt")
                {
                    c = '<';
                }
                else 
                if(mTransBuf == "amp")
                {
                    c = '&';
                }
                else
                if(mTransBuf[0] == '#') //handles &#109; conversions (high bit ascii)
                {
                    mTransBuf.erase(0, 1);
                    LONG nCharValue = strtoul(mTransBuf.c_str(), NULL, 0);
                    if(nCharValue > 255)
                        nCharValue = '?'; //Shame on you! Don't store unicode like this, use UTF-8
                    c = nCharValue;
                }
                else
                    ThrowXML(eXML_UnknownAmpersandTranslationChar);
            }

            hValue->operator+=(c);
        }
    }
    else
    {
        while((c = GetC()) != '<')
        {
            if(!ReadReady())
                ThrowXML(eXML_PrematureEOF);
        }
    }
    
    
    //Read <TagName> Into hStr
    while((c = GetC()) != '>')
    {
        if(!ReadReady())
            ThrowXML(eXML_PrematureEOF);
        else
            hTag.operator+=(c);
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ReadTillTagClose
//
//   Desc:: 
//
//  Input:: 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLReader::ReadTillTagClose(XMLTag* hTag, bool hDiscardData)
{
    STRING* readInto = hDiscardData ? NULL : &hTag->GetValue();
    bool nWeAreDataLeaf = true;
    APtr<XMLTag> nNewTag;
    

    while(true)
    {
        if(!ReadReady())
            ThrowXML(eXML_PrematureEOF);
        
        //Read until we find a tag
        mReadingTag.erase();
        if(!ReadTillTag(mReadingTag, nWeAreDataLeaf ? readInto : NULL))
            break;
        
        //Found end </tag>, check & exit the loop
        if(IsEndTag(mReadingTag))
        {
            if(!BeginTagMatchesEnd(hTag->GetName(), mReadingTag))
            {
                printf("eXML_UnmatchingEndBeginTags(%s, %s)\n", hTag->GetName().c_str(), mReadingTag.c_str());
                fflush(stdout);
                ThrowXML(eXML_UnmatchingEndBeginTags);
            }
            break;
        }

        //Found sub tag
        if(IsBeginTag(mReadingTag))
        {       
            nWeAreDataLeaf = false;
            ParseTagString(mReadingTag, nNewTag);
            ReadTillTagClose(nNewTag, hDiscardData);
            nNewTag->mInTag = hTag;
            hTag->mSubTags.push_back(nNewTag.Detach());
            continue;
        }

        //Found empty <tag/>
        if(IsEmptyTag(mReadingTag))
        {
            nWeAreDataLeaf = false;
            if(hDiscardData == false)
            {
                ParseTagString(mReadingTag, nNewTag);
                nNewTag->mInTag = hTag;
                hTag->mSubTags.push_back(nNewTag.Detach());
            }
            continue;
        }
                
        //Found comment <!-- tag -->
        if(IsCommentTag(mReadingTag))
        {
            if(hDiscardData == false)
            {
                mReadingTag.erase(0, 4);
                mReadingTag.erase(mReadingTag.length()-3, 3);
                nNewTag.Dispose();
                nNewTag = new XMLTag(hTag);
                nNewTag->GetValue() = mReadingTag;
                nNewTag->mType = eXML_CommentTag;
                hTag->mSubTags.push_back(nNewTag.Detach());
            }
            continue;
        }
        
        ThrowXML(eXML_BadTagWhereEndTagExpected);
    }


    if(!nWeAreDataLeaf || hDiscardData)
        return;


    hTag->mType = eXML_ValueTag;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::ParseTagString
//
//   Desc:: Takes the text from inside <> in <TagName TagAttr="AttrValue"> and
//          extracts the name of the tag and any attribute-value pairs. The
//          info is read into hTag. hTag is expected to be NULL so it may be
//          created as needed.
//
//  Input:: hStr: The tag contents.
//          hTag: Contains the parsed XMLTag on return.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void XMLReader::ParseTagString(STRING& hStr, APtr<XMLTag>& hTag)
{
    ULONG i, nFrom, nLength = hStr.length();
    CHAR theChar;


    //Extract Tag Name
    hTag.Dispose();
    hTag = new XMLTag();
    STRING& nReadingTagName = hTag->GetName();
    for(i = 0; i < nLength; ++i)
    {
        theChar = hStr[i];
        if(isspace(theChar) || theChar == '/')
            break;
    }
    nReadingTagName.append(hStr, 0, i);

    
    
    //Extract All Tag Attributes
    APtr<XMLAttribute> nNewAttribute;
    while(true)
    {
        if(i >= nLength)
            break;
        
        //Skip whitespace and empty tag marker
        theChar = hStr[i];
        if(isspace(theChar) || (i+1 == nLength && theChar == '/'))
        {
            ++i;
            continue;
        }
        
        nNewAttribute = new XMLAttribute();
        STRING& nReadingAttrKey = nNewAttribute->first;
        STRING& nReadingAttrValue = nNewAttribute->second;
        
        //Read in key
        nFrom = i;
        while(i < nLength)
        {
            if(hStr[i] == '=')
                break;
            ++i;
        }
        nReadingAttrKey.append(hStr, nFrom, i-nFrom);
        ++i;

        //Find quoting
        if(i >= nLength)
            ThrowXML(eXML_UnfinishTagAttributes);
            
        CHAR nQuoteChar = hStr[i];
        if(nQuoteChar != '\'' && nQuoteChar != '"')
            ThrowXML(eXML_BadAttributeQuotingChar); 
        
        //Read in value
        nFrom = ++i;
        while(i < nLength)
        {
            if(hStr[i] == nQuoteChar)
                break;
            ++i;
        }
        nReadingAttrValue.append(hStr, nFrom, i-nFrom);
        ++i;
        
        //Add finished attribute
        hTag->GetAttributes().push_back(nNewAttribute.Detach());
    }
} 




