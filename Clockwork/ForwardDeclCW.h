/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                        [Crossplatform]
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
#ifndef FORCWARDECLCW_H
#define FORCWARDECLCW_H 1

typedef class Link              Link;
typedef class TCPLink           TCPLink;
typedef class TCPManager        TCPManager;
typedef class FileLink          FileLink;
typedef class FileManager       FileManager;
typedef class FilePath          FilePath;
typedef class Exception         Exception;
//TCP Tasks
typedef class TCPTask           TCPTask;
typedef class TCPFwdLookup      TCPFwdLookup;
typedef class TCPRevLookup      TCPRevLookup;
typedef class TCPOpenConnect    TCPOpenConnect;
typedef class TCPRecvConnect    TCPRecvConnect;
//OSUtils
typedef class TimeV             TimeV;
//Scripting
class Script;
class ScriptBinary;
class ScriptCodeLine;
class ScriptConditional;
class ScriptDataType;
class ScriptFunction;
class ScriptVariable;
struct ScriptFuncDef;
struct ScriptDataTypeDef;
typedef List<Script*>                   ScriptList;
typedef List<ScriptVariable*>           ScriptVariableList;
typedef List<ScriptCodeLine*>           ScriptCodeLineList;
typedef ScriptList::iterator            ScriptIter;
typedef ScriptVariableList::iterator    ScriptVariableIter;
typedef ScriptCodeLineList::iterator    ScriptCodeLineIter;

#endif //ifndef FORCWARDECLCW_H
