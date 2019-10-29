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

#include "Array.h"
#include "LinkedList.h"
#include "Point.h"
#include "SimpleString.h"

// These two functions  are declared in DiffEngine. They are tested here.
bool Between(long number, long min, long max);
size_t IdxConv(int idx, int arraySize);


BOOST_AUTO_TEST_CASE( testcase_IdxConv )
{
  BOOST_CHECK_EQUAL(IdxConv(-1, 5), 4);
  BOOST_CHECK_EQUAL(IdxConv(-2, 5), 3);
  BOOST_CHECK_EQUAL(IdxConv(-3, 5), 2);
  BOOST_CHECK_EQUAL(IdxConv(-4, 5), 1);
  BOOST_CHECK_EQUAL(IdxConv(-5, 5), 0);

  BOOST_CHECK_EQUAL(IdxConv(-6, 5), 4);
  BOOST_CHECK_EQUAL(IdxConv(-7, 5), 3);
  BOOST_CHECK_EQUAL(IdxConv(-11, 5), 4);

  BOOST_CHECK_EQUAL(IdxConv(0, 5), 0);
  BOOST_CHECK_EQUAL(IdxConv(1, 5), 1);
  BOOST_CHECK_EQUAL(IdxConv(2, 5), 2);
  BOOST_CHECK_EQUAL(IdxConv(3, 5), 3);
  BOOST_CHECK_EQUAL(IdxConv(4, 5), 4);


  BOOST_CHECK_EQUAL(IdxConv(5, 5), 0);
  BOOST_CHECK_EQUAL(IdxConv(6, 5), 1);
  BOOST_CHECK_EQUAL(IdxConv(7, 5), 2);
  BOOST_CHECK_EQUAL(IdxConv(8, 5), 3);
  BOOST_CHECK_EQUAL(IdxConv(9, 5), 4);

  BOOST_CHECK_EQUAL(IdxConv(10, 5), 0);
  BOOST_CHECK_EQUAL(IdxConv(11, 5), 1);

}

BOOST_AUTO_TEST_CASE( testcase_Between )
{
  BOOST_CHECK_EQUAL(Between(0, 1, 5), false);
  BOOST_CHECK_EQUAL(Between(1, 1, 5), true);
  BOOST_CHECK_EQUAL(Between(2, 1, 5), true);
  BOOST_CHECK_EQUAL(Between(3, 1, 5), true);
  BOOST_CHECK_EQUAL(Between(4, 1, 5), true);
  BOOST_CHECK_EQUAL(Between(5, 1, 5), true);
  BOOST_CHECK_EQUAL(Between(6, 1, 5), false);
  BOOST_CHECK_EQUAL(Between(7, 1, 5), false);

}


///**
// *  The basic example which Eugene Myers used for the presentation of
// *  his algorithm.
// *
// *     A           |   B
// *   ------------------------------
// *     A           |   C
// *     B           |   B
// *     C           |   A
// *     A           |   B
// *     B           |   A
// *     B           |   C
// *     A           |
// */
//BOOST_AUTO_TEST_CASE( testcase_00_myers )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  DiffFilePartitionLinux srcA(cancelRequested);
//  srcA.PreProcess("../../../testfiles/testcase_00_myers_left.txt");

//  DiffFilePartitionLinux srcB(cancelRequested);
//  srcB.PreProcess("../../../testfiles/testcase_00_myers_right.txt");

//  DiffFilePartition targetA(cancelRequested);
//  DiffFilePartition targetB(cancelRequested);

//  diffOk = diffEngine.Diff(srcA, srcB, targetA, targetB);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(srcA.NumLines(), 7);
//  BOOST_CHECK_EQUAL(srcB.NumLines(), 6);
//  BOOST_CHECK_EQUAL(targetA.NumLines(), 9);
//  BOOST_CHECK_EQUAL(targetB.NumLines(), 9);

//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(0).C_str(), "A");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(0), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(1).C_str(), "B");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(1), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(2).C_str(), "C");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(3).C_str(), "");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(3), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(4).C_str(), "A");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(5).C_str(), "B");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(5), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(6).C_str(), "B");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(6), DiffLine::Deleted);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(7).C_str(), "A");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineText(8).C_str(), "");
//  BOOST_CHECK_EQUAL(targetA.GetDiffLineState(8), DiffLine::Normal);

//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(0).C_str(), "");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(0), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(1).C_str(), "");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(1), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(2).C_str(), "C");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(2), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(3).C_str(), "B");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(3), DiffLine::Added);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(4).C_str(), "A");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(4), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(5).C_str(), "B");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(5), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(6).C_str(), "");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(6), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(7).C_str(), "A");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(7), DiffLine::Normal);
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineText(8).C_str(), "C");
//  BOOST_CHECK_EQUAL(targetB.GetDiffLineState(8), DiffLine::Added);

//}

/**
 * Test the if FindPath() returns the expected result dor the example
 * given at:
 *
 * https://blog.jcoglan.com/2017/04/25/myers-diff-in-linear-space-implementation/
 */
BOOST_AUTO_TEST_CASE( test_FindPath_testcase_22_myers_ruby_linearSpace )
{
  bool cancelRequested = false;
  bool diffOk = false;
  DiffEngine diffEngine(cancelRequested);

  DiffFilePartitionLinux srcA(cancelRequested);
  srcA.PreProcess("../../../testfiles/testcase_22_myersruby_left.c");

  DiffFilePartitionLinux srcB(cancelRequested);
  srcB.PreProcess("../../../testfiles/testcase_22_myersruby_right.c");

  Array<Point>* pPath = diffEngine.FindPath(0, 0,
                                           srcA.NumLines(),
                                           srcB.NumLines(),
                                           srcA,
                                           srcB);

  BOOST_CHECK_EQUAL(pPath->Size(), 17);

  BOOST_CHECK_EQUAL((*pPath)[0].Left(), 0);
  BOOST_CHECK_EQUAL((*pPath)[0].Top(), 0);

  BOOST_CHECK_EQUAL((*pPath)[1].Left(), 1);
  BOOST_CHECK_EQUAL((*pPath)[1].Top(), 0);

  BOOST_CHECK_EQUAL((*pPath)[2].Left(), 2);
  BOOST_CHECK_EQUAL((*pPath)[2].Top(), 2);

  BOOST_CHECK_EQUAL((*pPath)[3].Left(), 3);
  BOOST_CHECK_EQUAL((*pPath)[3].Top(), 2);

  BOOST_CHECK_EQUAL((*pPath)[4].Left(), 4);
  BOOST_CHECK_EQUAL((*pPath)[4].Top(), 2);

  BOOST_CHECK_EQUAL((*pPath)[5].Left(), 5);
  BOOST_CHECK_EQUAL((*pPath)[5].Top(), 4);

  BOOST_CHECK_EQUAL((*pPath)[6].Left(), 6);
  BOOST_CHECK_EQUAL((*pPath)[6].Top(), 4);

  BOOST_CHECK_EQUAL((*pPath)[7].Left(), 6);
  BOOST_CHECK_EQUAL((*pPath)[7].Top(), 5);

  BOOST_CHECK_EQUAL((*pPath)[8].Left(), 9);
  BOOST_CHECK_EQUAL((*pPath)[8].Top(), 7);

  BOOST_CHECK_EQUAL((*pPath)[9].Left(), 10);
  BOOST_CHECK_EQUAL((*pPath)[9].Top(), 9);

  BOOST_CHECK_EQUAL((*pPath)[10].Left(), 11);
  BOOST_CHECK_EQUAL((*pPath)[10].Top(), 9);

  BOOST_CHECK_EQUAL((*pPath)[11].Left(), 11);
  BOOST_CHECK_EQUAL((*pPath)[11].Top(), 10);

  BOOST_CHECK_EQUAL((*pPath)[12].Left(), 11);
  BOOST_CHECK_EQUAL((*pPath)[12].Top(), 11);

  BOOST_CHECK_EQUAL((*pPath)[13].Left(), 12);
  BOOST_CHECK_EQUAL((*pPath)[13].Top(), 12);

  BOOST_CHECK_EQUAL((*pPath)[14].Left(), 13);
  BOOST_CHECK_EQUAL((*pPath)[14].Top(), 12);

  BOOST_CHECK_EQUAL((*pPath)[15].Left(), 13);
  BOOST_CHECK_EQUAL((*pPath)[15].Top(), 13);

  BOOST_CHECK_EQUAL((*pPath)[16].Left(), 14);
  BOOST_CHECK_EQUAL((*pPath)[16].Top(), 14);


  delete pPath;
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

  DiffFilePartition targetA(cancelRequested);
  DiffFilePartition targetB(cancelRequested);

  diffOk = diffEngine.Diff(srcA, srcB, targetA, targetB);

  BOOST_CHECK_EQUAL(diffOk, true);
}


///**
// * This test did loop on Amiga..
// *
// */
//BOOST_AUTO_TEST_CASE( testcase_10_myers )
//{
//  bool cancelRequested = false;
//  bool diffOk = false;
//  DiffEngine diffEngine(cancelRequested);

//  DiffFilePartitionLinux srcA(cancelRequested);
//  srcA.PreProcess("../../../testfiles/testcase_13_6000_left.cpp");

//  DiffFilePartitionLinux srcB(cancelRequested);
//  srcB.PreProcess("../../../testfiles/testcase_13_6000_right.cpp");

//  DiffFilePartition targetA(cancelRequested);
//  DiffFilePartition targetB(cancelRequested);

//  diffOk = diffEngine.Diff(srcA, srcB, targetA, targetB);

//  BOOST_CHECK_EQUAL(diffOk, true);

//  BOOST_CHECK_EQUAL(srcA.NumLines(), 5832);
//  BOOST_CHECK_EQUAL(srcB.NumLines(), 7183);
//  BOOST_CHECK_EQUAL(targetA.NumLines(), 7796);
//  BOOST_CHECK_EQUAL(targetB.NumLines(), 7796);
//}
