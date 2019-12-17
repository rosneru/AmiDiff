/**
 * ADiffView
 *
 * A test-main for the pure DiffEngine as of 2019/12 under Amiga using
 * pool memory.
 *
 *
 * Author: Uwe Rosner
 *    https://github.com/rosneru
 */

#define INTUI_V36_NAMES_ONLY

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <clib/exec_protos.h>
#include <workbench/startup.h>

#include "DiffFileBase.h"
#include "DiffFileAmiga.h"
#include "DiffEngineAmiga.h"
#include "LinkedListAmiga.h"
#include "DiffLine.h"


extern struct IntuitionBase* IntuitionBase;

APTR m_pPoolHeader = NULL;

/**
 * CLI entry point
 */
int main(int argc, char **argv)
{
  m_pPoolHeader = CreatePool(MEMF_CLEAR, 50000, 25000);
  if(m_pPoolHeader == NULL)
  {
    printf("Failed to create memory pool.\n");
  }

  bool cancelRequested = false;
  bool diffOk = false;

  LinkedListAmiga list(m_pPoolHeader);

  DiffFileAmiga srcA(m_pPoolHeader, cancelRequested);
  srcA.PreProcess("/testfiles/testcase_23_RealLifeApp-left.cs");

  DiffFileAmiga srcB(m_pPoolHeader, cancelRequested);
  srcB.PreProcess("/testfiles/testcase_23_RealLifeApp-right.cs");

  DiffFileAmiga targetA(m_pPoolHeader, cancelRequested);
  DiffFileAmiga targetB(m_pPoolHeader, cancelRequested);

  DiffEngineAmiga diffEngine(srcA,
                             srcB,
                             targetA,
                             targetB,
                             m_pPoolHeader,
                             cancelRequested,
                             &list);

  diffOk = diffEngine.Diff();

  if(diffOk)
  {
    printf("Diff erfolgreich\n");
  }
  else
  {
    printf("Fehler beim Berechnen des Diff.\n");
  }

  DeletePool(m_pPoolHeader);
  return 0;
}


/**
 * Workbench entry point.
 */
void wbmain(struct WBStartup* wb)
{
  // Call the CLI entry point with argc=0
  main(0, (char **) wb);
}
