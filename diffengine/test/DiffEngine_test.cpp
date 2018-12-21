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
#include "LinkedList.h"
#include "SimpleString.h"


BOOST_AUTO_TEST_CASE( testDiff_PassAddString )
{
  bool diffOk = false;
  DiffEngine diffEngine;

  //
  // Test 3
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 2         |   Line 2
  // Line 3         |   Line 4
  // Line 4         |   <<empty line>>
  //                |
  //
  // >> Deleted "Line 3" in right file.

  DiffFilePartitionLinux leftSrcPartition1;
  leftSrcPartition1.PreProcess("../../testfiles/testcase_03_left.txt");

  DiffFilePartitionLinux rightSrcPartition1;
  rightSrcPartition1.PreProcess("../../testfiles/testcase_03_right.txt");

  DiffFilePartition leftDiffPartition1;
  DiffFilePartition rightDiffPartition1;

  diffOk = diffEngine.Diff(leftSrcPartition1,
                           rightSrcPartition1,
                           leftDiffPartition1,
                           rightDiffPartition1);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition1.NumLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition1.NumLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(3), DiffLine::Normal);

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

  DiffFilePartitionLinux leftSrcPartition2;
  leftSrcPartition2.PreProcess("../../testfiles/testcase_04_left.txt");

  DiffFilePartitionLinux rightSrcPartition2;
  rightSrcPartition2.PreProcess("../../testfiles/testcase_04_right.txt");

  DiffFilePartition leftDiffPartition2;
  DiffFilePartition rightDiffPartition2;

  diffOk = diffEngine.Diff(leftSrcPartition2,
                           rightSrcPartition2,
                           leftDiffPartition2,
                           rightDiffPartition2);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition2.NumLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition2.NumLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(3), DiffLine::Normal);



  //
  // Test 5
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 3         |   Line 2
  // Line 4         |   Line 3
  //                |   Line 4
  //                |
  //
  // >> Deleted "Line 2" in left file

  DiffFilePartitionLinux leftSrcPartition3;
  leftSrcPartition3.PreProcess("../../testfiles/testcase_05_left.txt");

  DiffFilePartitionLinux rightSrcPartition3;
  rightSrcPartition3.PreProcess("../../testfiles/testcase_05_right.txt");

  DiffFilePartition leftDiffPartition3;
  DiffFilePartition rightDiffPartition3;

  diffOk = diffEngine.Diff(leftSrcPartition3,
                           rightSrcPartition3,
                           leftDiffPartition3,
                           rightDiffPartition3);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition3.NumLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetDiffLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition3.NumLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetDiffLineState(3), DiffLine::Normal);
}


BOOST_AUTO_TEST_CASE( DiffTest_06_Mixed )
{
  bool diffOk = false;
  DiffEngine diffEngine;

  //
  // Test 6: Mixed it up with 6 changes
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 4         |   Line 2
  // Line 5         |   Line 3
  // Line 6         |   Line 4
  // Line 7         |   Line 5
  // Line 8         |   Line 8
  // ab             |   Line 9
  // Line 9         |   cd
  // Line 10        |   Line 10
  //

  DiffFilePartitionLinux leftSrcPartition;
  leftSrcPartition.PreProcess("../../testfiles/testcase_06_left.txt");

  DiffFilePartitionLinux rightSrcPartition;
  rightSrcPartition.PreProcess("../../testfiles/testcase_06_right.txt");

  DiffFilePartition leftDiffPartition;
  DiffFilePartition rightDiffPartition;

  diffOk = diffEngine.Diff(leftSrcPartition,
                           rightSrcPartition,
                           leftDiffPartition,
                           rightDiffPartition);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition.NumLines(), 12);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(2).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(4).C_str(), "Line 5");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(5).C_str(), "Line 6");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(5), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(6).C_str(), "Line 7");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(6), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(7).C_str(), "Line 8");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(8).C_str(), "ab");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(8), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(9).C_str(), "Line 9");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(9), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(10).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(10), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(11).C_str(), "Line 10");
  BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(11), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 12);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(4).C_str(), "Line 5");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(5).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(6).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(6), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(7).C_str(), "Line 8");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(7), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(8).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(8), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(9).C_str(), "Line 9");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(9), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(10).C_str(), "cd");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(10), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(11).C_str(), "Line 10");
  BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(11), DiffLine::Normal);

  int leftAdded, leftChanged, leftDeleted, rightAdded, rightChanged, rightDeleted, sumChanges;

  leftDiffPartition.NumChanges(leftAdded, leftChanged, leftDeleted);
  rightDiffPartition.NumChanges(rightAdded, rightChanged, rightDeleted);

  sumChanges = leftAdded + leftChanged + leftDeleted + rightAdded + rightChanged + rightDeleted;
  BOOST_CHECK_EQUAL(sumChanges, 6);
}

BOOST_AUTO_TEST_CASE( DiffTest_11_RightLineOneDelThreeAdds )
{
  bool diffOk = false;
  DiffEngine diffEngine;

  //
  // Test 11
  //
  // Left.txt       |   Right.txt
  // ------------------------------
  // Line 1         |   Line 1
  // Line 2         |   Line 3
  // Line 3         |   Line 4
  // Line 4         |   Line 5
  //                |   Line 6
  //                |   Line 7
  //
  // >> Deleted one line and added 3 lines in right file

  DiffFilePartitionLinux leftSrcPartition1;
  leftSrcPartition1.PreProcess("../../testfiles/testcase_11_left.txt");

  DiffFilePartitionLinux rightSrcPartition1;
  rightSrcPartition1.PreProcess("../../testfiles/testcase_11_right.txt");

  DiffFilePartition leftDiffPartition1;
  DiffFilePartition rightDiffPartition1;

  diffOk = diffEngine.Diff(leftSrcPartition1,
                           rightSrcPartition1,
                           leftDiffPartition1,
                           rightDiffPartition1);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftSrcPartition1.NumLines(), 4);
  BOOST_CHECK_EQUAL(rightSrcPartition1.NumLines(), 6);
  BOOST_CHECK_EQUAL(leftDiffPartition1.NumLines(), 7);
  BOOST_CHECK_EQUAL(rightDiffPartition1.NumLines(), 7);

  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(1), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(4).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(5).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(6).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(6), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(1).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(4).C_str(), "Line 5");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(4), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(5).C_str(), "Line 6");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(5), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(6).C_str(), "Line 7");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(6), DiffLine::Added);
}


BOOST_AUTO_TEST_CASE( DiffTest_12_EndlessLoop )
{
  bool diffOk = false;
  DiffEngine diffEngine;

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

  DiffFilePartitionLinux leftSrcPartition;
  leftSrcPartition.PreProcess("../../testfiles/testcase_12_endless_loop_left.txt");

  DiffFilePartitionLinux rightSrcPartition;
  rightSrcPartition.PreProcess("../../testfiles/testcase_12_endless_loop_right.txt");

    DiffFilePartition leftDiffPartition;
    DiffFilePartition rightDiffPartition;

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
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(4).C_str(), "");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(4), DiffLine::Added);
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(5).C_str(), "Line 5");
    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(5), DiffLine::Normal);
}
