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
#ifndef ENDIANUTILS_H
#define ENDIANUTILS_H 1


ULONG   HostLongToLE(ULONG hLong);
ULONG   HostLongToBE(ULONG hLong);

USHORT  HostShortToLE(USHORT hShort);
USHORT  HostShortToBE(USHORT hShort);
ULONG   LELongToHost(ULONG hLong);
ULONG   BELongToHost(ULONG hLong);
USHORT  LEShortToHost(USHORT hShort);
USHORT  BEShortToHost(USHORT hShort);


ULONG   BELongToLE(ULONG hLong);
ULONG   LELongToBE(ULONG hLong);
USHORT  BEShortToLE(USHORT hShort);
USHORT  LEShortToBE(USHORT hShort);



namespace EndianUtils
{
    const long gEndianTestSeq       = 0x12345678;
    const bool gSystemIsBigEndian   = (((CHAR*)&gEndianTestSeq)[0] == 0x12);
}

#define kBigEndian      (EndianUtils::gSystemIsBigEndian)
#define kLittleEndian   (!EndianUtils::gSystemIsBigEndian)




#endif //ifndef ENDIANUTILS_H
