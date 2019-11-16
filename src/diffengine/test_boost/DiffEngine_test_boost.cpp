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




BOOST_AUTO_TEST_CASE( testcase_02 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  //
  // Test 2
  //
  // Left.txt     |   Right.txt
  // ------------------------------
  // AAAA         |   AAAA
  // BBBB         |   BBBB
  // CCCC         |   1111
  // DDDD         |   2222
  // EEEE         |   CCCC
  // FFFF         |   DDDD
  // GGGG         |   EEEE
  //              |   FFFF
  //              |   GGGG
  //              |
  //
  // >> Deleted "Line 3" in right file.

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_02_left.txt");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_02_right.txt");

  DiffFilePartitionLinux diffA(cancelRequested);
  DiffFilePartitionLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
  if(diffOk == false)
  {
    return;
  }

  BOOST_CHECK_EQUAL(diffA.NumLines(), 10);
  const char* pTxt = diffA.GetLineText(0);
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

  //
  // Test 4
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 2         |   Line 2
  // Line 3         |
  // Line 4         |   Line 4
  //                |
  //
  // >> Cleared "Line 3" (set to empty) in right file
BOOST_AUTO_TEST_CASE( testcase_04 )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFilePartitionLinux leftSrcPartition2(cancelRequested);
  leftSrcPartition2.PreProcess("../../../testfiles/testcase_04_left.txt");

  DiffFilePartitionLinux rightSrcPartition2(cancelRequested);
  rightSrcPartition2.PreProcess("../../../testfiles/testcase_04_right.txt");

  DiffFilePartitionLinux leftDiffPartition2(cancelRequested);
  DiffFilePartitionLinux rightDiffPartition2(cancelRequested);

  DiffEngine diffEngine(leftSrcPartition2,
                        rightSrcPartition2,
                        leftDiffPartition2,
                        rightDiffPartition2,
                        cancelRequested);

  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition2.NumLines(), 5);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineText(3), "");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineText(4), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetLineState(4), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition2.NumLines(), 5);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineText(2), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineText(3), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineText(4), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetLineState(4), DiffLine::Normal);
}


//  //
//  // Test 5
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 3         |   Line 2
//  // Line 4         |   Line 3
//  //                |   Line 4
//  //                |
//  //
//  // >> Deleted "Line 2" in left file
//BOOST_AUTO_TEST_CASE( testcase_05 )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  DiffFilePartitionLinux leftSrcPartition3(cancelRequested);
//  leftSrcPartition3.PreProcess("../../../testfiles/testcase_05_left.txt");

//  DiffFilePartitionLinux rightSrcPartition3(cancelRequested);
//  rightSrcPartition3.PreProcess("../../../testfiles/testcase_05_right.txt");

//  DiffFilePartition leftDiffPartition3(cancelRequested);
//  DiffFilePartition rightDiffPartition3(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition3,
//                           rightSrcPartition3,
//                           leftDiffPartition3,
//                           rightDiffPartition3);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(leftDiffPartition3.NumLines(), 4);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineText(0), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineText(1), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineText(2), "Line 3");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineText(3), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetLineState(3), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition3.NumLines(), 4);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineText(0), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineText(1), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineState(1), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineText(2), "Line 3");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineText(3), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetLineState(3), DiffLine::Normal);
//}


//BOOST_AUTO_TEST_CASE( DiffTest_06_Mixed )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  //
//  // Test 6: Mixed it up with 6 changes
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 4         |   Line 2
//  // Line 5         |   Line 3
//  // Line 6         |   Line 4
//  // Line 7         |   Line 5
//  // Line 8         |   Line 8
//  // ab             |   Line 9
//  // Line 9         |   cd
//  // Line 10        |   Line 10
//  //

//  DiffFilePartitionLinux leftSrcPartition(cancelRequested);
//  leftSrcPartition.PreProcess("../../../testfiles/testcase_06_left.txt");

//  DiffFilePartitionLinux rightSrcPartition(cancelRequested);
//  rightSrcPartition.PreProcess("../../../testfiles/testcase_06_right.txt");

//  DiffFilePartition leftDiffPartition(cancelRequested);
//  DiffFilePartition rightDiffPartition(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition,
//                           rightSrcPartition,
//                           leftDiffPartition,
//                           rightDiffPartition);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(leftDiffPartition.NumLines(), 12);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(0), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(1), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(2), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(3), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(4), "Line 5");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(5), "Line 6");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(5), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(6), "Line 7");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(6), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(7), "Line 8");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(8), "ab");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(8), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(9), "Line 9");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(9), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(10), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(10), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(11), "Line 10");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(11), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 12);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(0), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(1), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(1), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(2), "Line 3");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(2), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(3), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(4), "Line 5");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(5), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(5), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(6), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(6), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(7), "Line 8");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(8), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(8), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(9), "Line 9");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(9), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(10), "cd");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(10), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(11), "Line 10");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(11), DiffLine::Normal);

//  int leftAdded, leftChanged, leftDeleted, rightAdded, rightChanged, rightDeleted, sumChanges;

//  leftDiffPartition.NumChanges(leftAdded, leftChanged, leftDeleted);
//  rightDiffPartition.NumChanges(rightAdded, rightChanged, rightDeleted);

//  sumChanges = leftAdded + leftChanged + leftDeleted + rightAdded + rightChanged + rightDeleted;
//  BOOST_CHECK_EQUAL(sumChanges, 6);
//}

BOOST_AUTO_TEST_CASE( DiffTest_12_EndlessLoop )
{
  bool cancelRequested = false;
  bool diffOk = false;

  //
  // Test case: endless loop in version 1 from 2003
  // as spotlighted by an user comment on code
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 2         |   Line 2
  // Line 3         |
  //                |
  // Line 5         |   Line 5
  //                |
  //
  // >> Cleared "Line 3" (set to empty) in right file
  //

  DiffFilePartitionLinux leftSrcPartition(cancelRequested);
  leftSrcPartition.PreProcess("../../../testfiles/testcase_12_endless_loop_left.txt");

  DiffFilePartitionLinux rightSrcPartition(cancelRequested);
  rightSrcPartition.PreProcess("../../../testfiles/testcase_12_endless_loop_right.txt");

  DiffFilePartitionLinux leftDiffPartition(cancelRequested);
  DiffFilePartitionLinux rightDiffPartition(cancelRequested);

  DiffEngine diffEngine(leftSrcPartition,
                        rightSrcPartition,
                        leftDiffPartition,
                        rightDiffPartition,
                        cancelRequested);

  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(2), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(3), "");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(4), ""); // NOTE: This is a failure. Should be "Line 5" instead.
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineText(5), "Line 5");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetLineState(5), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(0), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(1), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(2), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(3), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(4), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineText(5), "Line 5");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetLineState(5), DiffLine::Normal);
}




BOOST_AUTO_TEST_CASE( testcase_13_6000_lines )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

  DiffFilePartitionLinux diffA(cancelRequested);
  DiffFilePartitionLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);
}



BOOST_AUTO_TEST_CASE( testcase_22_myers_ruby_linearSpace )
{
  bool cancelRequested = false;
  bool diffOk = false;

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_22_myersruby_left.c");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_22_myersruby_right.c");

  DiffFilePartitionLinux diffA(cancelRequested);
  DiffFilePartitionLinux diffB(cancelRequested);

  DiffEngine diffEngine(srcA, srcB, diffA, diffB, cancelRequested);
  diffOk = diffEngine.Diff();

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(srcA.NumLines(), 14);
  BOOST_CHECK_EQUAL(srcB.NumLines(), 14);
  BOOST_CHECK_EQUAL(diffA.NumLines(), 21);
  BOOST_CHECK_EQUAL(diffB.NumLines(), 21);

}
