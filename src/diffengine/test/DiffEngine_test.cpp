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
#include "DiffTracer.h"
#include "LinkedList.h"
#include "SimpleString.h"


//BOOST_AUTO_TEST_CASE( testDiff_PassAddString )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  //
//  // Test 3
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 2         |   Line 2
//  // Line 3         |   Line 4
//  // Line 4         |   <<empty line>>
//  //                |
//  //
//  // >> Deleted "Line 3" in right file.

//  DiffFilePartitionLinux leftSrcPartition1(cancelRequested);
//  leftSrcPartition1.PreProcess("../../../testfiles/testcase_03_left.txt");

//  DiffFilePartitionLinux rightSrcPartition1(cancelRequested);
//  rightSrcPartition1.PreProcess("../../../testfiles/testcase_03_right.txt");

//  DiffFilePartition leftDiffPartition1(cancelRequested);
//  DiffFilePartition rightDiffPartition1(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition1,
//                           rightSrcPartition1,
//                           leftDiffPartition1,
//                           rightDiffPartition1);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(leftDiffPartition1.NumLines(), 4);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(2), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(3), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition1.NumLines(), 4);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(2).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(3), DiffLine::Normal);

//  //
//  // Test 4
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 2         |   Line 2
//  // Line 3         |
//  // Line 4         |   Line 4
//  //                |
//  //
//  // >> Cleared "Line 3" (set to empty) in right file

//  DiffFilePartitionLinux leftSrcPartition2(cancelRequested);
//  leftSrcPartition2.PreProcess("../../../testfiles/testcase_04_left.txt");

//  DiffFilePartitionLinux rightSrcPartition2(cancelRequested);
//  rightSrcPartition2.PreProcess("../../../testfiles/testcase_04_right.txt");

//  DiffFilePartition leftDiffPartition2(cancelRequested);
//  DiffFilePartition rightDiffPartition2(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition2,
//                           rightSrcPartition2,
//                           leftDiffPartition2,
//                           rightDiffPartition2);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(leftDiffPartition2.NumLines(), 4);
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(2), DiffLine::Changed);
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition2.GetDiffLineState(3), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition2.NumLines(), 4);
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(2).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(2), DiffLine::Changed);
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition2.GetDiffLineState(3), DiffLine::Normal);



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

//BOOST_AUTO_TEST_CASE( DiffTest_11_RightLineOneDelThreeAdds )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  //
//  // Test 11
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 2         |   Line 3
//  // Line 3         |   Line 4
//  // Line 4         |   Line 5
//  //                |   Line 6
//  //                |   Line 7
//  //
//  // >> Deleted one line and added 3 lines in right file

//  DiffFilePartitionLinux leftSrcPartition1(cancelRequested);
//  leftSrcPartition1.PreProcess("../../../testfiles/testcase_11_left.txt");

//  DiffFilePartitionLinux rightSrcPartition1(cancelRequested);
//  rightSrcPartition1.PreProcess("../../../testfiles/testcase_11_right.txt");

//  DiffFilePartition leftDiffPartition1(cancelRequested);
//  DiffFilePartition rightDiffPartition1(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition1,
//                           rightSrcPartition1,
//                           leftDiffPartition1,
//                           rightDiffPartition1);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(leftSrcPartition1.NumLines(), 4);
//  BOOST_CHECK_EQUAL(rightSrcPartition1.NumLines(), 6);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.NumLines(), 7);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.NumLines(), 7);

//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(1).C_str(), "Line 2");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(1), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(4).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(5).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(5), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(6).C_str(), "");
//  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(6), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(0).C_str(), "Line 1");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(1).C_str(), "");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(2).C_str(), "Line 3");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(3).C_str(), "Line 4");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(4).C_str(), "Line 5");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(4), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(5).C_str(), "Line 6");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(5), DiffLine::Added);
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(6).C_str(), "Line 7");
//  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(6), DiffLine::Added);
//}


//BOOST_AUTO_TEST_CASE( DiffTest_12_EndlessLoop )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  //
//  // Test case: endless loop in version 1 from 2003
//  // as spotlighted by an user comment on code
//  //
//  // Left.txt       |   Right.txt
//  // ------------------------------
//  // Line 1         |   Line 1
//  // Line 2         |   Line 2
//  // Line 3         |
//  //                |
//  // Line 5         |   Line 5
//  //                |
//  //
//  // >> Cleared "Line 3" (set to empty) in right file
//  //

//  DiffFilePartitionLinux leftSrcPartition(cancelRequested);
//  leftSrcPartition.PreProcess("../../../testfiles/testcase_12_endless_loop_left.txt");

//  DiffFilePartitionLinux rightSrcPartition(cancelRequested);
//  rightSrcPartition.PreProcess("../../../testfiles/testcase_12_endless_loop_right.txt");

//    DiffFilePartition leftDiffPartition(cancelRequested);
//    DiffFilePartition rightDiffPartition(cancelRequested);

//    diffOk = diffEngine.Diff(leftSrcPartition,
//                             rightSrcPartition,
//                             leftDiffPartition,
//                             rightDiffPartition);

//    BOOST_CHECK_EQUAL(diffOk, true);


//    BOOST_CHECK_EQUAL(leftDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(0), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(1), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(2).C_str(), "Line 3");
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(2), DiffLine::Deleted);
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(3).C_str(), "");
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(3), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(4).C_str(), ""); // NOTE: This is a failure. Should be "Line 5" instead.
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(4), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineText(5).C_str(), "Line 5");
//    BOOST_CHECK_EQUAL(leftDiffPartition.GetDiffLineState(5), DiffLine::Normal);

//    BOOST_CHECK_EQUAL(rightDiffPartition.NumLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(0).C_str(), "Line 1");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(0), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(1).C_str(), "Line 2");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(1), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(2).C_str(), "");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(2), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(3).C_str(), "");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(3), DiffLine::Normal);
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(4).C_str(), "");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(4), DiffLine::Added);
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineText(5).C_str(), "Line 5");
//    BOOST_CHECK_EQUAL(rightDiffPartition.GetDiffLineState(5), DiffLine::Normal);
//}


///**
// * In this test case both files are about 6000 lines long.
// *
// * In ADiffView release version 1.0 this crashes:-(
// */
//BOOST_AUTO_TEST_CASE( DiffTest_13_LongFiles_Crash )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);


//  DiffFilePartitionLinux leftSrcPartition(cancelRequested);
//  leftSrcPartition.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

//  DiffFilePartitionLinux rightSrcPartition(cancelRequested);
//  rightSrcPartition.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

//    DiffFilePartition leftDiffPartition(cancelRequested);
//    DiffFilePartition rightDiffPartition(cancelRequested);

//    diffOk = diffEngine.Diff(leftSrcPartition,
//                             rightSrcPartition,
//                             leftDiffPartition,
//                             rightDiffPartition);

//    BOOST_CHECK_EQUAL(diffOk, true);

//    //
//    // Collecting the number of changes
//    //
//    int leftNumAdded;
//    int leftNumChanged;
//    int leftNumDeleted;
//    leftDiffPartition.NumChanges(leftNumAdded, leftNumChanged,
//                                 leftNumDeleted);

//    int rightNumAdded;
//    int rightNumChanged;
//    int rightNumDeleted;
//    rightDiffPartition.NumChanges(rightNumAdded, rightNumChanged,
//                                  rightNumDeleted);

//    int sumChanges = leftNumAdded + leftNumChanged + leftNumDeleted
//                   + rightNumAdded + rightNumChanged + rightNumDeleted;

//    BOOST_CHECK_EQUAL(7603, sumChanges);
//}

///**
// * User-provided testcase, see doc/feedback/FB101-02 files
// *
// * "Comparing the included files gives a somewhat odd result, one
// * string changed, but we get a full block of changes"
// */
//BOOST_AUTO_TEST_CASE( DiffTest_FB101_02 )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);


//  DiffFilePartitionLinux leftSrcPartition(cancelRequested);
//  leftSrcPartition.PreProcess("../../../testfiles/testcase_14_FB101-02-Left.txt");

//  DiffFilePartitionLinux rightSrcPartition(cancelRequested);
//  rightSrcPartition.PreProcess("../../../testfiles/testcase_14_FB101-02-Right.txt");

//  DiffFilePartition leftDiffPartition(cancelRequested);
//  DiffFilePartition rightDiffPartition(cancelRequested);

//  diffOk = diffEngine.Diff(leftSrcPartition,
//                           rightSrcPartition,
//                           leftDiffPartition,
//                           rightDiffPartition);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  //
//  // Collecting the number of changes
//  //
//  int leftNumAdded;
//  int leftNumChanged;
//  int leftNumDeleted;
//  leftDiffPartition.NumChanges(leftNumAdded, leftNumChanged,
//                               leftNumDeleted);

//  int rightNumAdded;
//  int rightNumChanged;
//  int rightNumDeleted;
//  rightDiffPartition.NumChanges(rightNumAdded, rightNumChanged,
//                                rightNumDeleted);

//  int sumChanges = leftNumAdded + leftNumChanged + leftNumDeleted
//                 + rightNumAdded + rightNumChanged + rightNumDeleted;

//  BOOST_CHECK_EQUAL(7, sumChanges);
//}

/**
 * Testing if the DiffTracer stores the given array properly.
 *
 * Doing so by clearing the src arrays after adding them to DiffTracer
 * and then checking if the former src values are still there.
 */
BOOST_AUTO_TEST_CASE( test_DiffTracer_Storage )
{
  int src1[] = {3, 4, 5, 6};
  int src2[] = {83, 293, 1377};

  DiffTracer diffTracer;
  diffTracer.AddArray(src1, sizeof(src1) / sizeof(src1[0]));
  diffTracer.AddArray(src2, sizeof(src2) / sizeof(src2[0]));

  src1[0] = 0;
  src1[1] = 0;
  src1[2] = 0;
  src1[3] = 0;

  src2[0] = 0;
  src2[1] = 0;
  src2[2] = 0;

  Array<int>* pFirstStored = diffTracer.GetFirst();
  Array<int>* pSecondStored = diffTracer.GetNext();

  BOOST_CHECK_EQUAL((*pFirstStored)[0], 3);
  BOOST_CHECK_EQUAL((*pFirstStored)[1], 4);
  BOOST_CHECK_EQUAL((*pFirstStored)[2], 5);
  BOOST_CHECK_EQUAL((*pFirstStored)[3], 6);

  BOOST_CHECK_EQUAL((*pSecondStored)[0], 83);
  BOOST_CHECK_EQUAL((*pSecondStored)[1], 293);
  BOOST_CHECK_EQUAL((*pSecondStored)[2], 1377);
}

/**
 * Simplified variant of user-provided testcase,
 * see doc/feedback/FB101-02 files
 *
 * "Comparing the included files gives a somewhat odd result, one
 * string changed, but we get a full block of changes"
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   AAAA           |   AAAA
 *   BBBB           |   FFFF
 *   CCCC           |   CCCC
 *   DDDD           |   DDDD
 *   EEEE           |   EEEE
 *   FFFF           |   FFFF
 *   GGGG           |   GGGG
 *
 * In Right.txt, line 2 is changed to 'FFFF'. This should be reported
 * as 1 CHANGE.
 *
 * Instead current algorithm is reporting 8 changes:
 *   4 DELETIONS in Left.txt
 *   4 INSERTION in Right.txt
 *
 * Analysis: the reason for this wrong result seems to be that the
 * Match() method resulting true. True is resulted because the 'FFFF'
 * in changed Right.txt / line 2 **is indeed found** in Left.txt
 */
BOOST_AUTO_TEST_CASE( DiffTest_FB101_02_Simplified )
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux leftSrcPartition1(cancelRequested);
  leftSrcPartition1.PreProcess("../../../testfiles/testcase_00_myers_left.txt");

  DiffFilePartitionLinux rightSrcPartition1(cancelRequested);
  rightSrcPartition1.PreProcess("../../../testfiles/testcase_00_myers_right.txt");

  DiffFilePartition leftDiffPartition1(cancelRequested);
  DiffFilePartition rightDiffPartition1(cancelRequested);

  diffOk = diffEngine.Diff(leftSrcPartition1,
                           rightSrcPartition1,
                           leftDiffPartition1,
                           rightDiffPartition1);

  BOOST_CHECK_EQUAL(diffOk, true);
/*
  BOOST_CHECK_EQUAL(leftSrcPartition1.NumLines(), 7);
  BOOST_CHECK_EQUAL(rightSrcPartition1.NumLines(), 7);
  BOOST_CHECK_EQUAL(leftDiffPartition1.NumLines(), 7);
  BOOST_CHECK_EQUAL(rightDiffPartition1.NumLines(), 7);

  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(1).C_str(), "BBBB");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(2).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(3).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(4).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(5).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineText(6).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetDiffLineState(6), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(1).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(2).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(3).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(4).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(5).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineText(6).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetDiffLineState(6), DiffLine::Normal);
*/
}
