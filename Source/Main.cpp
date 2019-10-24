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
#include "StringUtils.h"
#include "MUD.h"
#include "FileManager.h"
#include "FileLink.h"
#include "Object.h"
#include "Player.h"
#include "EndianUtils.h"
#include "OrangeSession.h"
#include "OSUtils.h"
#include "StringUtils.h"

//## Prototypes ##//
void TestFiles();
void NamedTest();
void TestLoad();
void TestSOCKS();


int main()
{   
    try
    {
        //TestSOCKS();
        //NamedTest();
        MUD::Get()->Startup();
        
        //TestFiles();
        //TestLoad();

        while(MUD::Get()->mMUDAlive)
            MUD::Get()->Process();

        MUD::Get()->Shutdown();
        return 0;
    }
    catch(Exception* nMUDExcept)
    {
        printf(nMUDExcept->what());
    }
    catch(...)
    {
        cout << "Exception thrown, mud terminating." << endl;
    }
}





void TestFiles()
{
/*
    FilePath    nPath("Reference Code","test.txt");
    FileLink*   nFLink;
        
    cout << FileManager::Get()->CvtToLocalPath(nPath) << endl;

    if(FileManager::Get()->MakeDir(nPath))
        cout << "Made dir" << endl;
    else
        cout << "Couldn't create dir" << endl;
*/      
/*
    nFLink = FileManager::Get()->OpenWrite(nPath);
    //nFLink = FileManager::Get()->OpenRead(nPath);
    if(!nFLink)
    {
        cout << "Failed opening file.\n";
    }else{
        CHAR* nTestStr = "Hello\nThere\nWorld.";
        nFLink->Write(nTestStr, strlen(nTestStr));
        //while(nFLink->ReadReady())
        //  cout << nFLink->GetC();
        nFLink->Close();
    }
*/
}


#include "OrangeScriptCompiler.h"
#include "OrangeScript.h"
#include "ScriptExecutor.h"

STRING StripParenthesis(STRING hStr);
void NamedTest()
{
    const CHAR* nScriptSource =
    "INT    nTen, nHund;                        \n"
    "STRING nText;                              \n"
    "                                           \n"
    "nText = \"WooHoo $n\";                     \n"
    "nHund = (nTen=10)*10;                      \n"
    "if(nHund + 1 > 100)                        \n"
    "if(nHund+=3 < 1)                           \n"
    "   nHund = 0;                              \n"
    "                                           \n"
    "BOOL nNot;                                 \n"
    "nNot = !FALSE;                             \n"
    "if(!class(ch) && 4 == NECROMANCER)         \n"
    "   nHund =1;                               \n"
    "                                           \n"
    "INT i;                                     \n"
    "i = 1;                                     \n"
    "while(i < 5)                               \n"
    "   i+=1;                                   \n"
    "                                           \n"
    "if(!!class(ch))                            \n"
    "{                                          \n"
    "// act_to_all(\"$n dies.\", ch, obj, vict);\n"
    "   end;                                    \n"
    "}                                          \n";

    
    OrangeScriptCompiler    nCompiler;
    OrangeScript            nScript;
    CHAR                    c;
    
    nScript.mSource = nScriptSource;
    nCompiler.InitFunctionality();
    if(nCompiler.Compile(&nScript))
    {
        cout << "Compilation Successful!" << endl;
    }else{
        cout << "Failure:" << endl;
        cout << nScript.mErrorBuffer << endl;
        cin >> c;
        return;
    }
    
    cin >> c;
    ScriptExecutor nExec1(&nScript);
    ScriptExecutor nExec2(&nScript);
    ScriptExecutor nExec3(&nScript);
    
    nExec1.Execute();
    nExec2.Execute();
    nExec3.Execute();
    
    cout << "Executed (x3) !" << endl;
    cin >> c;
}




void TestLoad()
{
    return;
    STRINGCW    nName   = "volkaran";
    Player*     nPlayer = NULL;
    CHAR        nResult;
    
    nResult = MUD::Get()->LoadPlayerOld(nName, nPlayer);
    
    switch(nResult)
    {
        case kLoadPlayer_ErrWithFile:
            cout << "Error Loading File." << endl;
            break;
        case kLoadPlayer_NonExistant:
            cout << "No Player File." << endl;
            break;
        case kLoadPlayer_Success:
            cout << "Loaded Sucessfully." << endl;
            break;
    }
    
    if(nPlayer)
        delete nPlayer;
}



void TestSOCKS()
{
    STRING nBase64Text;
    STRING nDecodedText;
    STRING nText = "abcdefghijklmnop";
    
    Base64Encode(nText.data(), nText.length(), nBase64Text);
    printf("%s\n", nBase64Text.c_str());
    ASSERT(Base64Decode(nBase64Text.data(), nBase64Text.length(), nDecodedText));
    printf("%s\n", nDecodedText.c_str()); 
    //////////////////////////////////////////////////
    SOCKSv5Prefs nSocks5;
    nSocks5.mServer     = "proxy.ageofwonder.com";
    nSocks5.mPort       = 1080;
    nSocks5.mUsername   = "testuser1";
    nSocks5.mPassword   = "testpassword2474";
    //////////////////////////////////////////////////
    SOCKSv4Prefs nSocks4;   
    nSocks4.mServer     = "ns1.ageofwonder.com";
    nSocks4.mPort       = 1080;
    //nSocks4.mUsername = "fasagdfg";
    //////////////////////////////////////////////////
    HTTPProxyPrefs nHTTP;
    nHTTP.mServer       = "proxy.ageofwonder.com";
    nHTTP.mPort         = 8080;
    nHTTP.mUsername     = "testuser1";
    nHTTP.mPassword     = "testpassword2474";
    //////////////////////////////////////////////////
    STRING  nTestMachine    = "ageofwonder.com";
    SHORT   nTestPort       = 8888;
    TCPManager::Get()->Initialize();    
    TCPManager::Get()->SetProxyPrefs(nSocks5);
    
    printf("Attempting Proxied Connect.\n");
    TCPLink* nProxiedLink = NULL;
    try
    {
        TCPOpenConnect* nOpen = TCPManager::Get()->AsyncOpenConnection(nTestMachine, nTestPort);
        while(!nOpen->Done())
        {
            TimeV nSleep(0, 500000);        
            
            nOpen->Process();
            Sleep(nSleep);
        }
        
        
        nProxiedLink = nOpen->Results();
        delete nOpen, nOpen = NULL;
        if(!nProxiedLink)
        {
            printf("Proxied Connection Refused.\n");
            exit(0);
        }
        printf("Proxied Connection Established.\n");
        
        CHAR nBuf[2000];
        const CHAR* nMsg = "GET /index.html\r\n";
        nProxiedLink->Write(nMsg, strlen(nMsg));
        //nProxiedLink->AsyncRead(nBuf, 2000);
        while(nProxiedLink->ConnectionAlive())
        {
            LONG nRead = nProxiedLink->Read(nBuf, 2000);
            nBuf[nRead] = '\0';
            printf(nBuf);
        }
        
        //nBuf[nProxiedLink->AsyncReadProgress()] = '\0';
        printf(nBuf);
    }
    catch(Exception* hExc)
    {
        printf("Proxied Connect Failed.\n%s\n", hExc->what());
        if(nProxiedLink)
            delete nProxiedLink, nProxiedLink = NULL;
        exit(1);
    }

    printf("Proxied Connect Worked!\n");
    TCPManager::Get()->Shutdown();
    exit(0);
}
