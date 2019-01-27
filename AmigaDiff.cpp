/**
 * ADiffView
 *
 * A simple graphical viewer for Differences in ASCII text files.
 * 
 * Running on AMIGA computers with at least OS 3.0 / v39.
 *
 *
 * Development environment
 *   (1) DEVELOPING, TESTING, DEBUGGING, BUILD
 *       Amiga with OS3.9 on WinUAE
 *       NDK for AmigaOS 3.9
 *       StormC 4
 * 
 *   (2) CROSS DEVELOPING
 *       Linux / Debian Stretch in a VM
 *       Amiga OS toolchain from https://github.com/bebbo
 *       Visual Studio Code
 *
 *   (3) TESTING SOME SUBMODULES
 *       Linux / Debian Stretch in a VM
 *       UnitTests with QTCreator and the boost framework
 *
 * History
 *   xx.02.2019 - Release of version 1.0
 *   18.09.2018 - Project start
 *
 * Author: Uwe Rosner
 *    https://github.com/rosneru
 */

#define INTUI_V36_NAMES_ONLY

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/intuitionbase.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "Application.h"
#include "SimpleString.h"


/**
 * Extracts the parameters which could be provided optionally on 
 * program start via Workbench or CLI.
 * 
 * @param argc The argc variable from main()
 * @param argv The argv array from main.
 * 
 * @p_PubScreenName The name of the pubscreen to open the window on. 
 * Only set if the CLI argument or Workbench tooltype PUBSCREEN is set.
 * 
 * @p_bDoNotAsk A boolean switch which indicates if the diff starts
 * directly after program start without opening the OpenFilesWindow.
 * Only set to true if the CLI argument or Workbench tooltype DONOTASK
 * is set.
 * 
 * @p_LeftFilePath The file name of the left file if one was passed 
 * from Workbench or CLI.
 * 
 * @p_RightFilePath The file name of the right file if one was passed
 * from Workbench or CLI.
 */
void exctractArgs(int argc, char **argv,
  SimpleString& p_PubScreenName,
  bool& p_bDoNotAsk,
  SimpleString& p_LeftFilePath,
  SimpleString& p_RightFilePath);


/**
 * Displays a warn request with the given message
 */
void warnRequest(SimpleString p_Message);

extern struct IntuitionBase* IntuitionBase;

int main(int argc, char **argv)
{
  // Check if the OS version is at least v39 / OS 3.0; return otherwise
  if(IntuitionBase->LibNode.lib_Version < 39)
  {
    warnRequest("This program needs at least OS 3.0 / v39 to run.");
    return 20;
  }

  // Define some variables for values which optionally could be passed
  // as arguments to the app
  SimpleString pubScreenName;
  bool bDoNotAsk = false;
  SimpleString leftFilePath;
  SimpleString rightFilePath;

  // Fill the variables with values if appropriate arguments are passed
  // as CLI or Workbench startup arguments / tool types
  exctractArgs(argc, argv, pubScreenName, bDoNotAsk, leftFilePath, 
    rightFilePath);

  // Create a message port for shared use with all windows
  struct MsgPort* pMsgPortIDCMP = CreateMsgPort();
  if(pMsgPortIDCMP == NULL)
  {
    warnRequest("Error: Can't create idcmp message port.");
    return 30;
  }

  struct MsgPort* pMsgPortProgress = CreateMsgPort();
  if(pMsgPortProgress == NULL)
  {
    warnRequest("Error: Can't create progress message port.");
    DeleteMsgPort(pMsgPortIDCMP);
    return 30;
  }

  // Create the application dynamically, so we can destroy it later
  // before de Message port is destroyed.
  Application* pApp = new Application(pubScreenName, pMsgPortIDCMP, 
    pMsgPortProgress);

  pApp->SetLeftFilePath(leftFilePath);
  pApp->SetRightFilePath(rightFilePath);
  pApp->Run(bDoNotAsk);

  // Destroy app
  delete pApp;

  // Destroy message ports
  DeleteMsgPort(pMsgPortProgress);
  DeleteMsgPort(pMsgPortIDCMP);

  return 0;
}

void wbmain(struct WBStartup* wb)
{
  main(0, (char **) wb);
}

void exctractArgs(int argc, char **argv,
  SimpleString& p_PubScreenName,
  bool& p_bDoNotAsk,
  SimpleString& p_LeftFilePath,
  SimpleString& p_RightFilePath)
{
  if(argc == 0)
  {
    //
    // Started from Workbench
    //

    int bufLen = 2048;  // TODO How to get rid of this fixed maximum?
    STRPTR pBuf = (STRPTR) AllocVec(bufLen, 0L);
    if(pBuf != NULL)
    {
      struct WBStartup* pWbStartup = (struct WBStartup*) argv;
      struct WBArg* pWbArg = pWbStartup->sm_ArgList;
      for(int i=0; i < pWbStartup->sm_NumArgs; i++)
      {
        if((pWbArg[i].wa_Lock != NULL))
        {
          if(i == 0)
          {
            //
            // The first pWbArg is the application icon itself. Getting 
            // the PUBSCREEN tooltype  from it
            //

            // Make the directory of the icon to the current dir (cd)
            BPTR oldDir = CurrentDir(pWbArg[i].wa_Lock);

            struct DiskObject* pDiskObject = GetDiskObjectNew(
              (STRPTR) pWbArg[i].wa_Name);

            if(pDiskObject != NULL)
            {
              // Trying to read the value of the PUBSCREEN tooltype in
              // the application icon
              STRPTR pValue = (STRPTR) FindToolType(
                pDiskObject->do_ToolTypes, "PUBSCREEN");
              
              if(pValue != NULL)
              {
                // The tooltype exists and the value has been read. 
                // Now save the value in the provided variable.
                p_PubScreenName = pValue;
              }

              // Trying to read the value of the DONOTASK tooltype in
              // the application icon
              pValue = (STRPTR) FindToolType(
                pDiskObject->do_ToolTypes, "DONOTASK");

              if(pValue != NULL)
              {
                // The tooltype exists. The value is ignored here 
                // because its a boolean tooltype. Now set the provided
                // variable.
                p_bDoNotAsk = true;
              }

              FreeDiskObject(pDiskObject);
            }

            // Change back to the formerly current directory
            CurrentDir(oldDir);
          }
          else if(i < 3)
          {
            SimpleString fullPath;

            if(NameFromLock(pWbArg[i].wa_Lock, pBuf, bufLen) != 0)
            {
              if(AddPart(pBuf,(STRPTR) pWbArg[i].wa_Name, bufLen))
              {
                fullPath = pBuf;
              }
  
              if(i == 1)
              {
                p_LeftFilePath = fullPath;
              }
              else
              {
                p_RightFilePath = fullPath;
              }
            }
          }
          else
          {
            // We only need the filenames of the first 2 selected icons
            break;
          }
        }
      }

      FreeVec(pBuf);
    }
  }
  else
  {
    //
    // Started from CLI
    //

    // Reading the command line arguments
    SimpleString argTempl = "FILES/M,PUBSCREEN/K,DONOTASK/S";
    LONG args[3] = {0, 0, 0};

    struct RDArgs* pReadArgs = ReadArgs(argTempl.C_str(), args, NULL);
    if(pReadArgs == NULL)
    {
      return;
    }

    if(args[0] != NULL)
    {
      // args[0] contains a array of pointers to the passed FILEs.
      STRPTR* pFiles = (STRPTR*) args[0];
      
      // If there are passed some FILE arguments we only take the first
      // two of them, storing them as left and right file path.
      if(pFiles[0] != NULL)
      {
        p_LeftFilePath = pFiles[0];
      }

      if(pFiles[1] != NULL)
      {
        p_RightFilePath = pFiles[1];
      }
    }
    
    if(args[1] != NULL)
    {
      p_PubScreenName = (const char*) args[1];
    }

    if(args[2] != NULL)
    {
      p_bDoNotAsk = true;
    }

    FreeArgs(pReadArgs);
  }
}

void warnRequest(SimpleString p_Message)
{
  size_t iTextSize = sizeof(struct IntuiText);

  struct IntuiText* pBodyText = (struct IntuiText*) 
    AllocMem(iTextSize, MEMF_CLEAR);
  
  if(pBodyText == NULL)
  {
    return;
  }

  struct IntuiText* pButtonText = (struct IntuiText*) 
    AllocMem(iTextSize, MEMF_CLEAR);

  if(pButtonText == NULL)
  {
    FreeMem(pBodyText, iTextSize); 
    return;
  }

  SimpleString bodyStr = p_Message;
  pBodyText->FrontPen  = 1;
  pBodyText->BackPen   = 0;
  pBodyText->DrawMode  = JAM2;
  pBodyText->NextText  = NULL;
  pBodyText->TopEdge   = 10;
  pBodyText->LeftEdge  = 10;
  pBodyText->IText = (UBYTE*) bodyStr.C_str();
 
  SimpleString buttonStr = "Ok";
  pButtonText->FrontPen  = 1;
  pButtonText->BackPen   = 0;
  pButtonText->DrawMode  = JAM2;
  pButtonText->NextText  = NULL;
  pButtonText->TopEdge   = 10;
  pButtonText->LeftEdge  = 10;
  pButtonText->IText = (UBYTE*) buttonStr.C_str();

  AutoRequest(NULL, pBodyText, NULL, pButtonText, NULL, NULL, 180, 80);

  FreeMem(pBodyText, iTextSize); 
  FreeMem(pButtonText, iTextSize);
}
