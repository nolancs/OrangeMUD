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
/******************************************************************************
 Classes defined herein are:
    XMLAttributes :: representation of a list of xml attributes.
    XMLTag :: representation of an xml tag, may contain other tags.
    XMLFile :: representation of an xml file with a single root tag.
    XMLWriter :: writes xml out using a stream, XMLTag, and XMLFile.
    XMLReader :: parses xml from a stream.
 ******************************************************************************/
#ifndef XMLFILE_H
#define XMLFILE_H 1
#include "BufferedReader.h"
#include "FileLink.h"


/*********************************************/
/* Enable XML debugging and/or line counting */
/*********************************************/
#if(TARGET_UNIX)
//#define DEBUG_XML
#endif

#if(DEBUG_XML)
#ifndef XML_COUNT_LINES
#define XML_COUNT_LINES
#endif
#endif
/**********************************************/





#define kXML_Indent     "  "
class XMLTag;
typedef std::pair<STRING,STRING> XMLAttribute;
typedef std::list<XMLAttribute*>::iterator XMLAttributesIter;
typedef std::vector<XMLTag*> XMLTagArray;





/*****************************************/
/* STRING to other data type conversions */
/*****************************************/
inline ULONG  AsULong(STRING& inVal);
inline LONG   AsLong(STRING& inVal);
inline float  AsFloat(STRING& inVal);
inline double AsDouble(STRING& inVal);
inline bool   AsBool(STRING& inVal);

inline ULONG AsULong(STRING& inVal)
{   ULONG nValue;
    if(sscanf(inVal.c_str(), "%lu", &nValue) != 1) ASSERT(!"Not an unsigned long");
    return nValue;
}
inline LONG AsLong(STRING& inVal)
{   ULONG nValue;
    if(sscanf(inVal.c_str(), "%ld", &nValue) != 1) ASSERT(!"Not a long");
    return nValue;
}
inline float AsFloat(STRING& inVal)
{   float nValue;
    if(sscanf(inVal.c_str(), "%f", &nValue) != 1) ASSERT(!"Not a float");
    return nValue;
}
inline double AsDouble(STRING& inVal)
{   double nValue;
    if(sscanf(inVal.c_str(), "%lf", &nValue) != 1) ASSERT(!"Not a double");
    return nValue;
}
inline bool AsBool(STRING& inVal)
{   return (inVal == "true" || inVal == "1");
}






class XMLAttributes
    : public std::list<XMLAttribute*>
{
    public: XMLAttributes(){}
    public: XMLAttributes(XMLAttributes& copyThis);
    public: XMLAttributes& operator=(const XMLAttributes& copyThis);
    public: ~XMLAttributes()
        {for(XMLAttributesIter i = begin(); i != end(); ++i) delete *i;}


    //** Public Member Functions **//
    public:
    STRING& operator[](const CHAR* hKey);
    XMLAttribute* Find(const CHAR* hKey, bool hThrow = false);
    void AddString(const CHAR* hKey, const CHAR* hValue);
    void AddULong(const CHAR* hKey, const ULONG hValue);
    void AddLong(const CHAR* hKey, const LONG hValue);
    void AddFloat(const CHAR* hKey, const float& hValue);
    void AddDouble(const CHAR* hKey, const double& hValue);
    void AddBool(const CHAR* hKey, const bool hValue);
};





enum eXMLTagType
{
    eXML_UndefinedTagType,
    eXML_SuperTag,
    eXML_ValueTag,
    eXML_EmptyTag,
    eXML_CommentTag
};

class XMLTag
{
    public: XMLTag(XMLTag* hInTag = NULL);
    public: ~XMLTag();
    friend class XMLReader;
    

    //** Public Member Functions **//
    public:
    XMLTag*         InTag()             {return mInTag;}
    STRING&         GetName()           {return mName;}
    XMLAttributes&  GetAttributes()     {return mAttrs;}
    STRING&         GetValue()          {return mValue;}
    XMLTagArray&    GetSubTags()        {return mSubTags;}
    bool            IsValueTag();
    bool            IsSuperTag();
    bool            HasSubTags()        {return !mSubTags.empty();}
    bool            IsCommentTag()      {return mType==eXML_CommentTag;}
    bool            IsNil()             {XMLAttribute* nilAttr = GetAttributes().Find("xsi:nil", false); if(!nilAttr) return false; return (nilAttr->second == "true" || nilAttr->second == "1");}
    const CHAR*     AsCStr();
    const STRING&   AsString();
    ULONG           AsULong();
    LONG            AsLong();
    USHORT          AsUShort();
    SHORT           AsShort();
    BYTE            AsByte();
    CHAR            AsChar();
    UCHAR           AsUChar();
    float           AsFloat();
    double          AsDouble();
    bool            AsBool();




    //** Public Data Members **//
    public:
    STRING          mName;
    XMLAttributes   mAttrs;
    STRING          mValue;
    XMLTagArray     mSubTags;
    XMLTag*         mInTag;
    
    
    
    
    //** Protected Data Members **//
    protected:
    eXMLTagType     mType;
};






class XMLFile
{
    public:
    ~XMLFile() //destructor to delete tags within file
    {
        XMLTagArray::iterator i;
        for(i = mTags.begin(); i != mTags.end(); ++i)
            delete *i;
        mTags.clear();
    }

    //** Public Data Members **//
    public:
    XMLTagArray mTags;
};






class XMLWriter
{
    public: XMLWriter(FileLink* hStream);
    public: virtual ~XMLWriter();


    //** Public Member Functions **//
    public:
    virtual FileLink* GetLink(){return mStream;}
    virtual void SetCompact(bool hOn);
    virtual void SetLink(FileLink* hStream);
    virtual void BeginFile();
    virtual void EndFile();
    virtual void BlankLine();
    virtual void WriteXMLTag(XMLTag* hTag);
    virtual void WriteXMLFile(XMLFile* hFile);
    virtual void BeginSuperTag(const CHAR* hTagName, XMLAttributes* hAttrs = NULL);
    virtual void BeginValueTag(const CHAR* hTagName, XMLAttributes* hAttrs = NULL);
    virtual void EndTag();
    
    //Write a <tag>value</tag>:
    virtual void WriteCommentTag(const CHAR* hComment);
    virtual void WriteEmptyTag(const CHAR* hTagName, XMLAttributes* hAttrs = NULL);
    virtual void WriteHandle(const CHAR* hTagName, const BYTE* hValue, LONG hLength, XMLAttributes* hAttrs = NULL);
    virtual void WriteString(const CHAR* hTagName, const CHAR* hValue, LONG hLength = -1, XMLAttributes* hAttrs = NULL);
    virtual void WriteULong(const CHAR* hTagName, const ULONG hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteLong(const CHAR* hTagName, const LONG hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteUShort(const CHAR* hTagName, const USHORT hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteShort(const CHAR* hTagName, const SHORT hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteByte(const CHAR* hTagName, const BYTE hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteChar(const CHAR* hTagName, const CHAR hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteFloat(const CHAR* hTagName, const float& hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteDouble(const CHAR* hTagName, const double& hValue, XMLAttributes* hAttrs = NULL);
    virtual void WriteBool(const CHAR* hTagName, const bool hValue, XMLAttributes* hAttrs = NULL);
    
    //Write a value:
    virtual void WriteValueHandle(const BYTE* hValue, LONG hLength);
    virtual void WriteValueString(const CHAR* hValue, LONG hLength = -1);
    virtual void WriteValueULong(const ULONG hValue);
    virtual void WriteValueLong (const LONG hValue);
    virtual void WriteValueUShort(const USHORT hValue);
    virtual void WriteValueShort(const SHORT hValue);
    virtual void WriteValueByte(const BYTE hValue);
    virtual void WriteValueChar(const CHAR hValue);
    virtual void WriteValueFloat(const float& hValue);
    virtual void WriteValueDouble(const double& hValue);
    virtual void WriteValueBool(const bool hValue);
    
    
    
    
    //** Protected Member Functions **//
    protected:
    virtual void WriteToFile(const CHAR* hText, LONG hLen = -1);
    virtual void WriteValueToFile(const CHAR* hText, LONG hLen = -1);
    virtual void WriteAttributes(XMLAttributes* hAttrs);
    


    
    //** Protected Data Members **//
    protected:
    FileLink*           mStream;
    bool                mCompact;
    bool                mBegun;
    std::stack<STRING>  mTagNameStack;
    std::stack<eXMLTagType> mTagTypeStack;
    STRING              mIndent;
    STRING              mIdentIncrement;
    bool                mOnANewLine;
};





enum eXMLError
{
    eXML_NoErr = 0,
    //Parsing Errors
    eXML_PrematureEOF,
    eXML_InvalidCommentTag,
    eXML_UnfinishTagAttributes,
    eXML_UnmatchingEndBeginTags,
    eXML_BadAttributeQuotingChar,
    eXML_BadTagWhereEndTagExpected,
    eXML_BadTagWhereBeginTagExpected,
    eXML_UnknownAmpersandTranslationChar,
    //Reading Errors
    eXML_TagNotFound,
    eXML_AttributeNotFound,
    eXML_InvalidBase64Handle,
    eXML_InvalidUTF8Encoding
};

#define ThrowXML(e) throw e

class XMLReader
{
    public: XMLReader(FileLink* hStream);
    public: virtual ~XMLReader() {}
    
    
    //** Public Member Functions **//
    public:
    virtual FileLink* GetLink() {return &mStream;}
    virtual void SetLink(FileLink* hStream) {mStream = *hStream;}

    //Basics:
    virtual bool OpenBaseTag(APtr<XMLTag>& baseTag); // open base tag of the file
    virtual bool OpenTag(XMLTag*& nextTag); // open next tag in stream
    virtual void ReadTagContents(); // handle contents: read value/subtags
    virtual void SkipTagContents(); // handle contents: discard value/subtags
    virtual void CloseTag(bool freeSubTags = true); // close current tag, dispose subTags
    virtual XMLTag* CurrentTag()    {return mTagStack.top();}
    virtual bool CurrentTagIsDone() {return mEOTReachedStack.top();}

    //Helpful Additions:
    virtual bool ScanForTag(const CHAR* tagName, XMLTag** nextTag = NULL, bool throwOnFail = true);
    virtual eXMLError ReadFile(XMLFile* hFile);

    virtual bool ReadHandle(const CHAR* hTagName, BYTE*& hValue, LONG& hLength, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadString(const CHAR* hTagName, STRING& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadULong(const CHAR* hTagName, ULONG& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadLong(const CHAR* hTagName, LONG& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadUShort(const CHAR* hTagName, USHORT& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadShort(const CHAR* hTagName, SHORT& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadByte(const CHAR* hTagName, BYTE& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadChar(const CHAR* hTagName, CHAR& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadFloat(const CHAR* hTagName, float& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadDouble(const CHAR* hTagName, double& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    virtual bool ReadBool(const CHAR* hTagName, bool& hValue, XMLTag** hTag = NULL, bool throwOnFail = true);
    
    
    
    
    //** Protected Member Functions **//
    protected:
    virtual void ReadVersion();
    virtual bool ReadTillTag(STRING& hTag, STRING* hValue);
    virtual void ReadTillTagClose(XMLTag* hTag, bool hDiscardData);
    virtual void ParseTagString(STRING& hTagStr, APtr<XMLTag>& hTag);
    inline bool IsEndTag(STRING& hTagStr);
    inline bool IsBeginTag(STRING& hTagStr);
    inline bool IsEmptyTag(STRING& hTagStr);
    inline bool IsCommentTag(STRING& hTagStr);
    inline bool BeginTagMatchesEnd(STRING& hBegin, STRING& hEnd);
    inline bool ReadReady() {return mReadReady;}
    inline CHAR GetC()
    {
        INT c = mStream.GetC();
        mReadReady = c != EOF;
#if(XML_COUNT_LINES)
        if(c == '\n')
            ++mOnLine;
#endif
        return c;
    }
    
    
    
    
    //** Protected Data Members **//
    protected:
    FileLink&           mStream;
    bool                mReadReady;
    STRING              mReadingTag;
    std::stack<XMLTag*> mTagStack;
    std::stack<bool>    mEOTReachedStack;
    STRING              mTransBuf;
#if(XML_COUNT_LINES)
    LONG                mOnLine;
#endif
};






//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::IsEndTag
//
//   Desc:: Checks to see if the tag is an </EndTag>.
//
//  Input:: The contents of a tag.
//
// Output:: True if the tags is an end tag, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline bool XMLReader::IsEndTag(STRING& hTagStr)
{
    if(hTagStr.length() <= 1
    || hTagStr[0] != '/')
        return false;
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::IsBeginTag
//
//   Desc:: Checks to see if the tag is a <StartingTag> and not an </EndTag>.
//
//  Input:: The contents of a tag.
//
// Output:: True if the tags is a starting tag, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline bool XMLReader::IsBeginTag(STRING& hTagStr)
{
    if(hTagStr.empty())
        return false;
        
    CHAR nStartChar = hTagStr[0];

    if(nStartChar == '/'
    || nStartChar == '?'
    || nStartChar == '!'
    || hTagStr[hTagStr.length()-1] == '/')
        return false;
        
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::IsEmptyTag
//
//   Desc:: Checks to see if the tag is <Empty/>
//
//  Input:: The contents of a tag.
//
// Output:: True if the tag is empty, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline bool XMLReader::IsEmptyTag(STRING& hTagStr)
{
    if(hTagStr.length() <= 1)
        return false;
        
    if(hTagStr[hTagStr.length()-1] == '/')
        return true;
        
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: XMLReader::IsCommentTag
//
//   Desc:: Checks to see if the tag is a comment tag. 
//
//  Input:: The contents of a tag.
//
// Output:: True if the tags is a comment tag, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline bool XMLReader::IsCommentTag(STRING& hTagStr)
{
    if(hTagStr.empty()
    || hTagStr[0] != '!')
        return false;
        
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: BeginTagMatchesEnd
//
//   Desc:: Takes the stuff from inside <> in a tag. Takes both a starting tag
//          which can just be a name, or a name and attribute list and compares
//          that to an ending tag to see if they match up.
//
//  Input:: hBegin: The beginning tag.
//          hEnd: The ending tag.
//
// Output:: True if the tags match up, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline bool XMLReader::BeginTagMatchesEnd(STRING& hBegin, STRING& hEnd)
{
    if(hEnd.empty())
        return false;

    return !strncmp(hBegin.c_str(), hEnd.c_str()+1, hEnd.length()-1);
}




#endif //ifndef XMLFILE_H
