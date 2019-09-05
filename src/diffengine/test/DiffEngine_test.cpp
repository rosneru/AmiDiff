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

/*
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(1).C_str(), "BBBB");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(2).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(3).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(4).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(5).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineText(6).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(targetA1.GetDiffLineState(6), DiffLine::Normal);

  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(0).C_str(), "AAAA");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(1).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(1), DiffLine::Changed);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(2).C_str(), "CCCC");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(3).C_str(), "DDDD");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(4).C_str(), "EEEE");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(5).C_str(), "FFFF");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(5), DiffLine::Normal);
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineText(6).C_str(), "GGGG");
  BOOST_CHECK_EQUAL(targetB1.GetDiffLineState(6), DiffLine::Normal);
*/
}
