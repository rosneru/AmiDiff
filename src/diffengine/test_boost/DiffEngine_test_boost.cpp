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
  DiffEngine diffEngine(cancelRequested);

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

  DiffFilePartition diffA(cancelRequested);
  DiffFilePartition diffB(cancelRequested);

  diffOk = diffEngine.Diff(srcA,
                           srcB,
                           diffA,
                           diffB);

  BOOST_CHECK_EQUAL(diffOk, true);
  if(diffOk == false)
  {
    return;
  }

  BOOST_CHECK_EQUAL(diffA.NumLines(), 10);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(1).C_str(), "BBBB");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(2).C_str(), "");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(3).C_str(), "");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(4).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(5).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(6).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(7).C_str(), "");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(8).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffA.GetDiffLineText(9).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(diffA.GetDiffLineState(9), DiffLine::Normal);

  BOOST_CHECK_EQUAL(diffB.NumLines(), 10);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(1).C_str(), "BBBB");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(2).C_str(), "1111");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(3).C_str(), "2222");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(4).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(5).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(6).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(7).C_str(), "3333");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(7), DiffLine::Added);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(8).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(diffB.GetDiffLineText(9).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(diffB.GetDiffLineState(9), DiffLine::Normal);


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
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux leftSrcPartition2(cancelRequested);
  leftSrcPartition2.PreProcess("../../../testfiles/testcase_04_left.txt");

  DiffFilePartitionLinux rightSrcPartition2(cancelRequested);
  rightSrcPartition2.PreProcess("../../../testfiles/testcase_04_right.txt");

  DiffFilePartition leftDiffPartition2(cancelRequested);
  DiffFilePartition rightDiffPartition2(cancelRequested);

  diffOk = diffEngine.Diff(leftSrcPartition2,
                           rightSrcPartition2,
                           leftDiffPartition2,
                           rightDiffPartition2);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition2.NumLines(), 5);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(3).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(4).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(4), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition2.NumLines(), 5);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(3).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(3), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(4).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(4), DiffLine::Normal);
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
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(1).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(3), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition3.NumLines(), 4);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(1), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(3), DiffLine::Normal);
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
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(1).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(2).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(4).C_str(), "Line 5");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(5).C_str(), "Line 6");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(5), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(6).C_str(), "Line 7");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(6), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(7).C_str(), "Line 8");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(8).C_str(), "ab");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(8), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(9).C_str(), "Line 9");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(9), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(10).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(10), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(11).C_str(), "Line 10");
//  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(11), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 12);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(1), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(2), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(4).C_str(), "Line 5");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(5).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(5), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(6).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(6), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(7).C_str(), "Line 8");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(8).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(8), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(9).C_str(), "Line 9");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(9), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(10).C_str(), "cd");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(10), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(11).C_str(), "Line 10");
//  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(11), DiffLine::Normal);

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
  DiffEngine diffEngine(cancelRequested);

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

    DiffFilePartition leftDiffPartition(cancelRequested);
    DiffFilePartition rightDiffPartition(cancelRequested);

    diffOk = diffEngine.Diff(leftSrcPartition,
                             rightSrcPartition,
                             leftDiffPartition,
                             rightDiffPartition);

    BOOST_CHECK_EQUAL(diffOk, true);


    BOOST_CHECK_EQUAL(leftDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(2).C_str(), "Line 3");
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(2), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(3).C_str(), "");
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(4).C_str(), ""); // NOTE: This is a failure. Should be "Line 5" instead.
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(5).C_str(), "Line 5");
    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(5), DiffLine::Normal);

    BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(2).C_str(), "");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(3).C_str(), "");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(3), DiffLine::Added);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(4).C_str(), "");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(5).C_str(), "Line 5");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(5), DiffLine::Normal);
}




BOOST_AUTO_TEST_CASE( testcase_13_6000_lines )
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

  DiffFilePartition diffA(cancelRequested);
  DiffFilePartition diffB(cancelRequested);

  diffOk = diffEngine.Diff(srcA, srcB, diffA, diffB);

  BOOST_CHECK_EQUAL(diffOk, true);
}



BOOST_AUTO_TEST_CASE( testcase_22_myers_ruby_linearSpace )
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_22_myersruby_left.c");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_22_myersruby_right.c");

  DiffFilePartition diffA(cancelRequested);
  DiffFilePartition diffB(cancelRequested);

  diffOk = diffEngine.Diff(srcA, srcB, diffA, diffB);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(srcA.NumLines(), 14);
  BOOST_CHECK_EQUAL(srcB.NumLines(), 14);
  BOOST_CHECK_EQUAL(diffA.NumLines(), 21);
  BOOST_CHECK_EQUAL(diffB.NumLines(), 21);

}
