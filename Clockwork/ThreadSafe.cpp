/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2001                                         [Crossplatform]
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
#include "StdAfx.h"
#include <iostream.h>
#include "ThreadSafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace PortLib;



CThreadSafeManager* CThreadSafeManager::Get()
{
    static CThreadSafeManager* nManager = NULL;
    
    //You cannot store the manager as a static object, it must be a ptr.
    //The reason is that if you use a static CThreadSafe<> object somewhere,
    //when the object is deleted by the system at app exit, it calls on
    //the CThreadSafeManager to remove its pointer from the global list,
    //but there's a chance CThreadSafeManager doesn't even exist anymore
    //at that point if it was created as a static object as well, and 
    //that's not good.
    
    if(!nManager)
        nManager = new CThreadSafeManager();
        
    return nManager;
}






void CThreadSafeManager::ThreadDying()
{
    std::vector<CThreadSafeBase*>::size_type i;
    
    for(i = 0; i < m_ThreadSafes.size(); i++)
        m_ThreadSafes[i]->OnThreadDie();
}




void CThreadSafeManager::Register(CThreadSafeBase* hThis)
{
    m_ThreadSafes.push_back(hThis);
}




void CThreadSafeManager::Unregister(CThreadSafeBase* hThis)
{
    std::vector<CThreadSafeBase*>::iterator nFound;
    
    nFound = std::find(m_ThreadSafes.begin(), m_ThreadSafes.end(), hThis);
    ASSERT(nFound != m_ThreadSafes.end());
    m_ThreadSafes.erase(nFound);
}




#if 0
#pragma mark -
//###########################################################################################
#endif




LONG CThreadSafeBase::GetThreadID()
{
    #if(TARGET_UNIX)
    pthread_t id = pthread_self();
    #elif(TARGET_WIN32)
    CWinThread* nInThread = AfxGetThread();
    LONG id = (LONG)(nInThread ? nInThread->m_nThreadID : 0);
    #elif(TARGET_MACOS)
    ULONG id = 0;
    #endif

    return (LONG) id;
}



