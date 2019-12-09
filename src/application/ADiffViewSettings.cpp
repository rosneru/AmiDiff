#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/icon_protos.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>


#include "ADiffViewSettings.h"


/**
 * The default color set
 */
ADiffViewSettings::ColorArray ADiffViewSettings::m_sColors =
{
  0x00030004, // 0x0003 - Load 3 colors, starting from 0x004
  0xf3f3f3f3, 0xb5b5b5b5, 0xb9b9b9b9, // red
  0xfcfcfcfc, 0xffffffff, 0xbbbbbbbb, // yellow
  0xc1c1c1c1, 0xfefefefe, 0xbdbdbdbd, // green
  0x00000000  // Termination
};



ADiffViewSettings::ADiffViewSettings()
  : m_ColorArray(m_sColors)
{

}


ADiffViewSettings::~ADiffViewSettings()
{

}


bool ADiffViewSettings::Load()
{

}

ULONG* ADiffViewSettings::GetColorArray()
{
  return m_ColorArray.elem;
}
