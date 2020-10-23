#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/icon_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/icon.h>
#endif

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>


#include "ADiffViewArgs.h"


ADiffViewArgs::ADiffViewArgs(int argc, char **argv)
  : m_ArgC(argc),
    m_pArgV(argv),
    m_bDontAsk(false),
    m_bNoAppIcon(false),
    m_bShowLineNumbers(true)
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


ADiffViewArgs::~ADiffViewArgs()
{

}


const std::string& ADiffViewArgs::PubScreenName() const
{
  return m_PubScreenName;
}


const std::string& ADiffViewArgs::LeftFile() const
{
  return m_LeftFilePath;
}


const std::string& ADiffViewArgs::RightFile() const
{
  return m_RightFilePath;
}


bool ADiffViewArgs::DontAsk() const
{
  return m_bDontAsk;
}

bool ADiffViewArgs::NoAppIcon()
{
  return m_bNoAppIcon;
}

bool ADiffViewArgs::ShowLineNumbers() const
{
  return m_bShowLineNumbers;
}


void ADiffViewArgs::readWorkbenchArgs()
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
    if((pWbArg[i].wa_Lock != 0))  // TODO check. Was 'NULL' before.
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
          const STRPTR* ppTooltypeArray = pDiskObject->do_ToolTypes;

          char* pValue = toolTypeValue(ppTooltypeArray, "PUBSCREEN");
          if(pValue != NULL)
          {
            m_PubScreenName = pValue;
          }

          if(toolTypeValue(ppTooltypeArray, "DONOTASK") != NULL)
          {
            m_bDontAsk = true;
          }

          if(toolTypeValue(ppTooltypeArray, "ASKONWORKBENCH") != NULL)
          {
            m_bNoAppIcon = true;
          }

          if(toolTypeValue(ppTooltypeArray, "NOLINENUMBERS") != NULL)
          {
             m_bShowLineNumbers = false;
          }

          FreeDiskObject(pDiskObject);
        }

        // Change back to the formerly current directory
        CurrentDir(oldDir);
      }
      else if(i < 3)
      {
        if(NameFromLock(pWbArg[i].wa_Lock, pBuf, bufLen) != 0)
        {
          if(AddPart(pBuf,(STRPTR) pWbArg[i].wa_Name, bufLen))
          {

            if(i == 1)
            {
              m_LeftFilePath = pBuf;
            }
            else
            {
              m_RightFilePath = pBuf;
            }
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

void ADiffViewArgs::readCommandLineArgs()
{
    // Reading the command line arguments
    const char argTempl[] = "FILES/M,PUBSCREEN/K,DONOTASK/S,NOAPPICON/S,NOLINENUMBERS/S";
    LONG args[] = {0, 0, 0, 0, 0};

    struct RDArgs* pReadArgs = ReadArgs(argTempl, args, NULL);
    if(pReadArgs == NULL)
    {
      return;
    }

    if(args[0] != 0)
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

    if(args[1] != 0)
    {
      m_PubScreenName = (const char*) args[1];
    }

    if(args[2] != 0)
    {
      m_bDontAsk = true;
    }

    if(args[3] != 0)
    {
      m_bNoAppIcon = true;
    }

    if(args[4] != 0)
    {
      m_bShowLineNumbers = false;
    }

    FreeArgs(pReadArgs);
}


char* ADiffViewArgs::toolTypeValue(const STRPTR* ppTooltypeArray, 
                                   const char* pTooltypeName)
{
  return (char*)FindToolType(ppTooltypeArray, (STRPTR)pTooltypeName);
}
