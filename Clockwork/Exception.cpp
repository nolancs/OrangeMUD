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



//** Main Exception Reference Table **//
/*
Exception::ExceptionInfo    Exception::cExceptionTable[xMax] =
{
    { kProblemTCP_NoErr             , "No error."                                                   },
    { kProblemTCP_Network           , "Operation failed."                                           },
    //-- Network --//
    { kProblemTCP_Network           , "Network resources are unavailable."                          },
    { kProblemTCP_Network           , "A networking error has ocurred."                             },
    { kProblemTCP_InvalidHostname   , "Invalid hostname supplied. A typo perhaps?"                  },
    { kProblemTCP_InvalidIP         , "Invalid IP address supplied."                                },
    { kProblemTCP_NoMachineIP       , "ACK! Couldn't figure out this machine's IP."                 },
    { kProblemTCP_NoMachineName     , "ACK! Couldn't figure out this machine's name."               },
    { kProblemTCP_Listen            , "An error occured while trying to listen."                    },
    { kProblemTCP_Poll              , "An error occured while polling the connection(s)."           },
    { kProblemTCP_AcceptConnect     , "An error occured while accepting a connection."              },
    { kProblemTCP_OpenConnect       , "An error occured while connecting."                          },
    { kProblemTCP_CloseConnect      , "An error occured while closing the connection."              },
    { kProblemTCP_Send              , "An error occured while sending data."                        },
    { kProblemTCP_Recv              , "An error occured while recieving data."                      },
    { kProblemTCP_Proxy             , "An error occured while negotiating with the proxy server."   }
    
};
*/



