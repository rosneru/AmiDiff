#include "DiffFilePartition.h"
#include "DiffFilePartitionLinux.h"
#include "DiffEngine.h"
#include "DiffLine.h"

#include <stdio.h>

int main()
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

  DiffFilePartition targetA(cancelRequested);
  DiffFilePartition targetB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, targetA, targetB, cancelRequested);
  diffOk = diffEngine.Diff();
  
  if(!diffOk)
  {
    printf("Fehler beim Berechnen des Diff.\n");
    return -30;
  }

  printf("Diff erfolgreich.\n");
  return 0;
}
