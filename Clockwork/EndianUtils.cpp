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
#include "CommonTypesCW.h"
#include "EndianUtils.h"





ULONG BELongToLE(ULONG hLong)
{
    ULONG nLELong = 0;
    
    ((CHAR*)&nLELong)[0] = ((CHAR*)&hLong)[3];
    ((CHAR*)&nLELong)[1] = ((CHAR*)&hLong)[2];
    ((CHAR*)&nLELong)[2] = ((CHAR*)&hLong)[1];
    ((CHAR*)&nLELong)[3] = ((CHAR*)&hLong)[0];
    
    return nLELong;
}



ULONG LELongToBE(ULONG hLong)
{
    ULONG nBELong = 0;
    
    ((CHAR*)&nBELong)[0] = ((CHAR*)&hLong)[3];
    ((CHAR*)&nBELong)[1] = ((CHAR*)&hLong)[2];
    ((CHAR*)&nBELong)[2] = ((CHAR*)&hLong)[1];
    ((CHAR*)&nBELong)[3] = ((CHAR*)&hLong)[0];
    
    return nBELong;
}



USHORT BEShortToLE(USHORT hShort)
{
    SHORT nLEShort = 0;
    
    ((CHAR*)&nLEShort)[0] = ((CHAR*)&hShort)[1];
    ((CHAR*)&nLEShort)[1] = ((CHAR*)&hShort)[0];
    
    return nLEShort;
}



USHORT LEShortToBE(USHORT hShort)
{
    SHORT nBEShort = 0;
    
    ((CHAR*)&nBEShort)[0] = ((CHAR*)&hShort)[1];
    ((CHAR*)&nBEShort)[1] = ((CHAR*)&hShort)[0];
    
    return nBEShort;
}




#if 0
#pragma mark -
#endif



ULONG HostLongToLE(ULONG hLong)
{
    return kBigEndian ? BELongToLE(hLong) : hLong;
}



ULONG HostLongToBE(ULONG hLong)
{
    return kBigEndian ? hLong : LELongToBE(hLong);
}



USHORT HostShortToLE(USHORT hShort)
{
    return kBigEndian ? BEShortToLE(hShort) : hShort;
}



USHORT HostShortToBE(USHORT hShort)
{
    return kBigEndian ? hShort : LEShortToBE(hShort);
}


#if 0
#pragma mark -
#endif

ULONG LELongToHost(ULONG hLong)
{
    return kBigEndian ? LELongToBE(hLong) : hLong;
}



ULONG BELongToHost(ULONG hLong)
{
    return kBigEndian ? hLong : BELongToLE(hLong);
}



USHORT LEShortToHost(USHORT hShort)
{
    return kBigEndian ? LEShortToBE(hShort) : hShort;
}



USHORT BEShortToHost(USHORT hShort)
{
    return kBigEndian ? hShort : BEShortToLE(hShort);
}


