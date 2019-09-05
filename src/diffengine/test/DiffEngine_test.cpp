/* Unit test for Diff using BOOST framework                          */
/*-------------------------------------------------------------------*/
/* Main class for testing base behavior of the DiffEngine            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* 09/2018                                                           */
/*-------------------------------------------------------------------*/

#define BOOST_TEST_MAIN
#if !defined( WIN32 )
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include "DiffFilePartition.h"
#include "DiffFilePartitionLinux.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "DiffTrace.h"
#include "LinkedList.h"
#include "SimpleString.h"


/**
 *  The basic example which Eugene Myers used for the presentation of
 *  his algorithm.
 *
 *     A           |   B
 *   ------------------------------
 *     A           |   C
 *     B           |   B
 *     C           |   A
 *     A           |   B
 *     B           |   A
 *     B           |   C
 *     A           |
 */
BOOST_AUTO_TEST_CASE( testcase_00_myers )
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_00_myers_left.txt");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_00_myers_right.txt");

  DiffFilePartition targetA(cancelRequested);
  DiffFilePartition targetB(cancelRequested);

  diffOk = diffEngine.Diff(srcA, srcB, targetA, targetB);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(srcA.NumLines(), 7);
  BOOST_CHECK_EQUAL(srcB.NumLines(), 6);
  BOOST_CHECK_EQUAL(targetA.NumLines(), 9);
  BOOST_CHECK_EQUAL(targetB.NumLines(), 9);

  for(int i = 0; i < targetA.NumLines(); i++)
  {
    DiffLine::LineState state = targetA.GetDiffLineState(i);
    const char* lineText = targetA.GetDiffLineText(i).C_str();

    switch(state)
    {
    case DiffLine::Added:
      printf("ADD %s\n", lineText);
      break;

    case DiffLine::Deleted:
      printf("DEL %s\n", lineText);
      break;

    case DiffLine::Normal:
      printf("    %s\n", lineText);
      break;

    case DiffLine::Undefined:
      printf("??? %s\n", lineText);
      break;

    }
  }


  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(0).C_str(), "A");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(0), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(1).C_str(), "B");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(1), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(2).C_str(), "C");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(3).C_str(), "");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(4).C_str(), "A");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(5).C_str(), "B");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(6).C_str(), "B");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(6), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(7).C_str(), "A");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(8).C_str(), "");
  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(8), DiffLine::Normal);

  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(0).C_str(), "");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(1).C_str(), "");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(2).C_str(), "C");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(3).C_str(), "B");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(4).C_str(), "A");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(5).C_str(), "B");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(6).C_str(), "");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(7).C_str(), "A");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(8).C_str(), "C");
  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(8), DiffLine::Added);

}
