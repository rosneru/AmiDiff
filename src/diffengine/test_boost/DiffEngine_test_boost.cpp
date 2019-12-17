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

#include "DiffFileBase.h"
#include "DiffFileLinux.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "LinkedList.h"



/**
 * testcase_02
 *
 *   Left.txt     |   Right.txt
 *   ------------------------------
 *   AAAA         |   AAAA
 *   BBBB         |   BBBB
 *   CCCC         |   1111
 *   DDDD         |   2222
 *   EEEE         |   CCCC
 *   FFFF         |   DDDD
 *   GGGG         |   EEEE
 *                |   FFFF
 *                |   GGGG
 *
 */
BOOST_AUTO_TEST_CASE( testcase_02 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_02_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_02_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
  if(diffOk == false)
  {
    return;
  }

  BOOST_CHECK_EQUAL(diffA.NumLines(), 10);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "CCCC");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(5), "DDDD");
  BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(6), "EEEE");
  BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(7), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(8), "FFFF");
  BOOST_CHECK_EQUAL(diffA.GetLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(9), "GGGG");
  BOOST_CHECK_EQUAL(diffA.GetLineState(9), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 10);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "BBBB");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "1111");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "2222");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "CCCC");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(5), "DDDD");
  BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(6), "EEEE");
  BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(7), "3333");
  BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(8), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(9), "GGGG");
  BOOST_CHECK_EQUAL(diffB.GetLineState(9), DiffLine::Normal);


}


/**
 * testcase_03_FB101-02-Simple
 *
 *   Left.txt     |   Right.txt
 *   ------------------------------
 *   AAAA         |   AAAA
 *   BBBB         |   FFFF
 *   CCCC         |   CCCC
 *   DDDD         |   DDDD
 *   EEEE         |   EEEE
 *   FFFF         |   FFFF
 *   GGGG         |   GGGG
 *                |
 *
 *   >> Changed line 2
 */
BOOST_AUTO_TEST_CASE( testcase_03_simple )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_03_FB101-02-Simple_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_03_FB101-02-Simple_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
  if(diffOk == false)
  {
    return;
  }

  BOOST_CHECK_EQUAL(diffA.NumLines(), 7);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "CCCC");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "DDDD");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "EEEE");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(5), "FFFF");
  BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(6), "GGGG");
  BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);


  BOOST_CHECK_EQUAL(diffB.NumLines(), 7);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "CCCC");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "DDDD");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "EEEE");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(5), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(6), "GGGG");
  BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
}


BOOST_AUTO_TEST_CASE( testcase_03_var2 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  //
  // Test 2
  //
  // Left.txt     |   Right.txt
  // ------------------------------
  // AAAA         |   AAAA
  // BBBB         |   FFFF
  // CCCC         |   ffff
  // DDDD         |   CCCC
  // EEEE         |   DDDD
  // FFFF         |   EEEE
  // GGGG         |   FFFF
  //              |   GGGG
  //
  //
  // >> Changed line 2

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_03_var2_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_03_var2_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
  if(diffOk == false)
  {
    return;
  }

  BOOST_CHECK_EQUAL(diffA.NumLines(), 8);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "CCCC");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "DDDD");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(5), "EEEE");
  BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(6), "FFFF");
  BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(7), "GGGG");
  BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 8);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "ffff");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "CCCC");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "DDDD");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(5), "EEEE");
  BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(6), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(7), "GGGG");
  BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Normal);
}


/**
 * testcase_04
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *   Line 4         |   Line 4
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 */
BOOST_AUTO_TEST_CASE( testcase_04 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_04_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_04_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(diffA.NumLines(), 4);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 4);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
}



/**
 * testcase_05
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 3         |   Line 2
 *   Line 4         |   Line 3
 *                  |   Line 4
 *                  |
 *
 *   >> Inserted "Line 2" in right file
 *
 */
BOOST_AUTO_TEST_CASE( testcase_05 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_05_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_05_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(diffA.NumLines(), 4);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 4);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
}


/**
 * testcase_06
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 4         |   Line 2
 *   Line 5         |   Line 3
 *   Line 6         |   Line 4
 *   Line 7         |   Line 5
 *   Line 8         |   Line 8
 *   ab             |   Line 9
 *   Line 9         |   cd
 *   Line 10        |   Line 10
 *
 */
BOOST_AUTO_TEST_CASE( DiffTest_06_Mixed )
{
  bool cancelRequested = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_06_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_06_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  bool diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(diffA.NumLines(), 12);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "Line 5");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(5), "Line 6");
  BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(diffA.GetLineText(6), "Line 7");
  BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(diffA.GetLineText(7), "Line 8");
  BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(8), "ab");
  BOOST_CHECK_EQUAL(diffA.GetLineState(8), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(diffA.GetLineText(9), "Line 9");
  BOOST_CHECK_EQUAL(diffA.GetLineState(9), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(10), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(10), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(11), "Line 10");
  BOOST_CHECK_EQUAL(diffA.GetLineState(11), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 12);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "Line 5");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(5), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(6), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(7), "Line 8");
  BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(8), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(9), "Line 9");
  BOOST_CHECK_EQUAL(diffB.GetLineState(9), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(10), "cd");
  BOOST_CHECK_EQUAL(diffB.GetLineState(10), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(11), "Line 10");
  BOOST_CHECK_EQUAL(diffB.GetLineState(11), DiffLine::Normal);

  long numAdd = diffEngine.NumAdded();
  long numChn = diffEngine.NumChanged();
  long numDel = diffEngine.NumDeleted();

  long sum = numAdd + numChn + numDel;
  BOOST_CHECK_EQUAL(sum, 6);
}

/**
 * testcase_12_endless_loop
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *                  |
 *   Line 5         |   Line 5
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 *
 * Within this test case the first implementation of the old
 * (not Myers) algorithm from 2003 did hang in an edless loop.
 *
 */
BOOST_AUTO_TEST_CASE( testcase_12_endless_loop )
{
  bool cancelRequested = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_12_endless_loop_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_12_endless_loop_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  bool diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(diffA.NumLines(), 5);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "Line 5");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);


  BOOST_CHECK_EQUAL(diffB.NumLines(), 5);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "Line 5");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
}


/**
 * testcase_12a
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *                  |
 *   Line 5         |
 *                  |   Line 5
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 *   >> Inserted two empty lines in right file
 *
 *
 */
BOOST_AUTO_TEST_CASE( testcase_12a )
{
  bool cancelRequested = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_12a_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_12a_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  bool diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(diffA.NumLines(), 6);
  BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(4), "");
  BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetLineText(5), "Line 5");
  BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 6);
  BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(diffB.GetLineText(3), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetLineText(4), "");
  BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetLineText(5), "Line 5");
  BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
}



/**
 * testcase_13_6000_lines
 *
 * For these two big files not much is done here. Its only tested if
 * the diff result is ok.
 */
BOOST_AUTO_TEST_CASE( testcase_13_6000_lines )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
}


/**
 * testcase_24_1500_numbers
 *
 * 11 changes in two textfiles with about 1500 lines each.
 * Used to test the navigation between diffs.
 */
BOOST_AUTO_TEST_CASE( testcase_24_1500_numbers )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFileLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_24_1500_numbers_left.txt");

  DiffFileLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_24_1500_numbers_right.txt");

  DiffFileLinux diffA(cancelRequested);
  DiffFileLinux diffB(cancelRequested);

  LinkedList diffStartIdxsList;

  DiffEngine diffEngine(srcA,
                        srcB,
                        diffA,
                        diffB,
                        cancelRequested,
                        &diffStartIdxsList);

  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  long numAdd = diffEngine.NumAdded();
  long numChn = diffEngine.NumChanged();
  long numDel = diffEngine.NumDeleted();
  long numDiff = diffEngine.NumDifferences();

  BOOST_CHECK_EQUAL(diffStartIdxsList.Size(), 22);
}
