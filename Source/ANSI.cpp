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
#include "ANSI.h"
#include "StringUtils.h"


ANSITable           gANSITable;
ANSIPrefTable       gANSIPrefTable;


ANSITable::ANSITable()
{
    static ANSIType sANSIs[] =
    { 
    //Name           Char   ArrayPos  Static  Static Code   
    { "Room.Title"  , '@'   , 0     , false , NULL      },
    { "Exits"       , '#'   , 1     , false , NULL      },
    { "Room.Desc"   , '%'   , 2     , false , NULL      },
    { "Items"       , '&'   , 3     , false , NULL      },
    { "Mobiles"     , '*'   , 4     , false , NULL      },
    { "Players"     , '('   , 5     , false , NULL      },
    { "Say"         , ')'   , 6     , false , NULL      },
    { "SayTo"       , '_'   , 7     , false , NULL      },
    { "Whisper"     , '-'   , 8     , false , NULL      },
    { "Chat"        , '='   , 9     , false , NULL      },
    { "Gossip"      , '+'   , 10    , false , NULL      },
    { "Tell"        , '`'   , 11    , false , NULL      },
    { "Gtell"       , '{'   , 12    , false , NULL      },
    { "OOC"         , '}'   , 13    , false , NULL      },  
    { "Auction"     , '['   , 14    , false , NULL      },
    { "Info"        , ']'   , 15    , false , NULL      },
    { "To.Self"     , '|'   , 16    , false , NULL      },
    { "To.Group"    , ':'   , 17    , false , NULL      },
    { "By.Self"     , ';'   , 18    , false , NULL      },
    { "By.Group"    , '<'   , 19    , false , NULL      },
    { "ImmTalk"     , '>'   , 20    , false , NULL      },
    { "OLC"         , '/'   , 21    , false , NULL      },
    { "ClanTalk"    , ','   , 22    , false , NULL      },
    { "Scan"        , '.'   , 23    , false , NULL      },
    { "Titles"      , 'T'   , 24    , false , NULL      },
    { "Highlight"   , 'H'   , 25    , false , NULL      },
    { "Topics"      , 't'   , 26    , false , NULL      },
    //Name           Char   ArrayPos  Static  Static Code   
    { "Exponent"    , '^'   , 0     , true  , "^"       },
    { "Bold"        , '!'   , 0     , true  , "\x1B[1m" },
    { "Underline"   , 'U'   , 0     , true  , "\x1B[4m" },
    { "Flashing"    , 'F'   , 0     , true  , "\x1B[5m" },
    { "Inverse"     , 'I'   , 0     , true  , "\x1B[7m" },
    { "Normal"      , 'n'   , 0     , true  , "\x1B[30m"},
    { "*Normal"     , 'N'   , 0     , true  , "\x1B[40m"},
    { "Red"         , 'r'   , 0     , true  , "\x1B[31m"},
    { "*Red"        , 'R'   , 0     , true  , "\x1B[41m"},
    { "Green"       , 'g'   , 0     , true  , "\x1B[32m"},
    { "*Green"      , 'G'   , 0     , true  , "\x1B[42m"},
    { "Yellow"      , 'y'   , 0     , true  , "\x1B[33m"},
    { "*Yellow"     , 'Y'   , 0     , true  , "\x1B[43m"},
    { "Blue"        , 'b'   , 0     , true  , "\x1B[34m"},
    { "*Blue"       , 'B'   , 0     , true  , "\x1B[44m"},
    { "Magenta"     , 'm'   , 0     , true  , "\x1B[35m"},
    { "*Magenta"    , 'M'   , 0     , true  , "\x1B[45m"},
    { "Cyan"        , 'c'   , 0     , true  , "\x1B[36m"},
    { "*Cyan"       , 'C'   , 0     , true  , "\x1B[46m"},
    { "White"       , 'w'   , 0     , true  , "\x1B[37m"},
    { "*White"      , 'W'   , 0     , true  , "\x1B[47m"},
    { "None"        , 'x'   , 0     , true  , "\x1B[0m" },
    { NULL          , '0'   , 0     , false , NULL      }
    };
    
    for(LONG i = 0; sANSIs[i].mName; ++i)
        insert(value_type(sANSIs[i].mMarker, sANSIs[i]));
};




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



ANSIPrefTable::ANSIPrefTable()
{
    static ANSIPrefType sANSIPrefs[] =
    { 
    /* Name        PrefBit  ANSICode  LongName              */
    { "normal"      , (A)   , 0     , "normal"              },
    { "bold"        , (B)   , 1     , "bold"                },
    { "underline"   , (C)   , 4     , "underlined"          },
    { "blink"       , (D)   , 5     , "blinking"            },
    { "reverse"     , (E)   , 7     , "reversed"            },
    //Foreground    
    { "black"       , (F)   , 30    , "black"               },
    { "red"         , (G)   , 31    , "red"                 },
    { "green"       , (H)   , 32    , "green"               },
    { "yellow"      , (I)   , 33    , "yellow"              },
    { "blue"        , (J)   , 34    , "blue"                },
    { "magenta"     , (K)   , 35    , "magenta"             },
    { "cyan"        , (L)   , 36    , "cyan"                },
    { "white"       , (M)   , 37    , "white"               },
    //Background
    { "*black"      , (N)   , 40    , "black-backrounded"   },
    { "*red"        , (O)   , 41    , "red-backrounded"     },
    { "*green"      , (P)   , 42    , "green-backrounded"   },
    { "*yellow"     , (Q)   , 43    , "yellow-backrounded"  },
    { "*blue"       , (R)   , 44    , "blue-backrounded"    },
    { "*magenta"    , (S)   , 45    , "magenta-backrounded" },
    { "*cyan"       , (T)   , 46    , "cyan-backrounded"    },
    { "*white"      , (U)   , 47    , "white-backrounded"   },
    { NULL          , 0     , 0     , NULL                  }
    };

    for(LONG i = 0; sANSIPrefs[i].mName; ++i)
        Add(sANSIPrefs[i]);
}




STRINGCW ANSIPrefTable::BuildANSICode(LONG hBits)
{
    STRINGCW nANSICode;

    if(hBits == 0)
        return nANSICode;
    
    nANSICode += "\x1B[";
    
    for(USHORT i = 0; i < size(); ++i)
    if(IsSet(hBits, (*this)[i].mBit))
        nANSICode += QBuf("%d;", (*this)[i].mCode).c_str();
    
    if(nANSICode.Length() > 2)
        nANSICode.Resize(nANSICode.Length()-1);
    
    nANSICode += "m";
    return nANSICode;
}



SHORT ANSIPrefTable::FindNamed(const CHAR* hName)
{
    for(USHORT i = 0; i < size(); ++i)
    if(STRINGCW((*this)[i].mName).IsNamed(hName))
        return i;

    return -1;
}

