#include "DiffFilePartition.h"
#include "DiffFilePartitionLinux.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "LinkedList.h"
#include "SimpleString.h"

#include <stdio.h>

int main()
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

  DiffFilePartition targetA(cancelRequested);
  DiffFilePartition targetB(cancelRequested);

  diffOk = diffEngine.Diff(srcA, srcB, targetA, targetB);
  
  if(!diffOk)
  {
    printf("Fehler beim Berechnen des Diff.\n");
    return -30;
  }

  printf("Diff erfolgreich.\n");
  return 0;
}

