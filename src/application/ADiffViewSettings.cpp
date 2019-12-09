#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/icon_protos.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>


#include "ADiffViewOptions.h"


ADiffViewOptions::ADiffViewOptions(int argc, char **argv)
  : m_ArgC(argc),
    m_pArgV(argv)
{
  if(argc == 0)
  {
    //
    // Started from Workbench
    //
    readWorkbenchArgs();

  }
  else
  {
    //
    // Started from CLI
    //
    readCommandLineArgs();
  }
}


ADiffViewOptions::~ADiffViewOptions()
{

}


SimpleString& ADiffViewOptions::PubScreenName()
{
  return m_PubScreenName;
}


SimpleString& ADiffViewOptions::LeftFile()
{
  return m_LeftFilePath;
}


SimpleString& ADiffViewOptions::RightFile()
{
  return m_RightFilePath;
}


bool ADiffViewOptions::DontAsk()
{
  return m_bDontAsk;
}


void ADiffViewOptions::readWorkbenchArgs()
{
  int bufLen = 2048;  // TODO How to get rid of this fixed maximum?
  STRPTR pBuf = (STRPTR) AllocVec(bufLen, MEMF_ANY);
  if(pBuf == NULL)
  {
    return;
  }

  struct WBStartup* pWbStartup = (struct WBStartup*) m_pArgV;
  struct WBArg* pWbArg = pWbStartup->sm_ArgList;
  for(int i=0; i < pWbStartup->sm_NumArgs; i++)
  {
    if((pWbArg[i].wa_Lock != NULL))
    {
      if(i == 0)
      {
        //
        // The first pWbArg is the application icon itself. Getting
        // the PUBSCREEN tooltype from it
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
            m_PubScreenName = pValue;
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
            m_bDontAsk = true;
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
            m_LeftFilePath = fullPath;
          }
          else
          {
            m_RightFilePath = fullPath;
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

void ADiffViewOptions::readCommandLineArgs()
{
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
        m_LeftFilePath = pFiles[0];
      }

      if(pFiles[1] != NULL)
      {
        m_RightFilePath = pFiles[1];
      }
    }

    if(args[1] != NULL)
    {
      m_PubScreenName = (const char*) args[1];
    }

    if(args[2] != NULL)
    {
      m_bDontAsk = true;
    }

    FreeArgs(pReadArgs);
}
