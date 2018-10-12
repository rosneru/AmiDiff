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
#include "DiffEngine.h"
#include "DiffLine.h"
#include "LinkedList.h"
#include "SimpleString.h"



BOOST_AUTO_TEST_CASE( testFilePartition )
{
  SimpleString text1 = "abc";
  SimpleString text2 = "defg";

  DiffFilePartition partition1(NULL);
  partition1.AddString(&text1);
  partition1.AddBlankLine();
  partition1.AddString(&text2);
  partition1.AddString(&text1);

  BOOST_CHECK_EQUAL(partition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(0)->GetLine()->C_str(), "abc");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(1)->GetLine()->C_str(), "");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(2)->GetLine()->C_str(), "defg");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(3)->GetLine()->C_str(), "abc");
}


void deleteAllListStrings(LinkedList& p_List)
{
  SimpleString* pItem = static_cast<SimpleString*>(p_List.GetFirst());
  while(pItem != NULL)
  {
    delete pItem;
    p_List.RemoveItem();
    pItem = static_cast<SimpleString*>(p_List.GetFirst());
  }
}

BOOST_AUTO_TEST_CASE( testDiff )
{
  LinkedList leftFileLines;
  LinkedList rightFileLines;
  bool diffOk = false;
  DiffEngine diffEngine;

  //
  // Test case 1: Left file is empty
  //
  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 3"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcPartition1(&leftFileLines);
  leftSrcPartition1.PreProcess();

  DiffFilePartition rightSrcPartition1(&rightFileLines);
  rightSrcPartition1.PreProcess();

  DiffFilePartition leftDiffPartition1;
  DiffFilePartition rightDiffPartition1;

  diffOk = diffEngine.Diff(leftSrcPartition1,
                           rightSrcPartition1,
                           leftDiffPartition1,
                           rightDiffPartition1);

  BOOST_CHECK_EQUAL(diffOk, true);

  // Left destination file should contain 4 empty lines with the LineState "Normal"
  BOOST_CHECK_EQUAL(leftDiffPartition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(0)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(1)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(2)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(3)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(3), DiffLine::Normal);

  // Right destination file should contain the 4 lines as inserted above
  BOOST_CHECK_EQUAL(rightDiffPartition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(0)->C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(1)->C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(2)->C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(3)->C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(3), DiffLine::Normal);


  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);


  //
  // Test case 2: Right file is empty
  //
  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 2"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcPartition2(&leftFileLines);
  leftSrcPartition2.PreProcess();

  DiffFilePartition rightSrcPartition2(&rightFileLines);
  rightSrcPartition2.PreProcess();

  DiffFilePartition leftDiffPartition2;
  DiffFilePartition rightDiffPartition2;

  diffOk = diffEngine.Diff(leftSrcPartition2,
                           rightSrcPartition2,
                           leftDiffPartition2,
                           rightDiffPartition2);

  BOOST_CHECK_EQUAL(diffOk, true);

  // Left destination file should contain the 4 lines as inserted above
  BOOST_CHECK_EQUAL(leftDiffPartition2.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(0)->C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(0), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(1)->C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(1), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(2)->C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(3)->C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(3), DiffLine::Deleted);

  // Right destination file should contain 4 empty lines with the LineState "Normal"
  BOOST_CHECK_EQUAL(rightDiffPartition2.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(0)->C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(1)->C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(2)->C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(3)->C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(3), DiffLine::Normal);



  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);


  //
  // Test case 3: Deleted "Line 3" in right file
  //

  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 2"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));

  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcPartition3(&leftFileLines);
  leftSrcPartition3.PreProcess();

  DiffFilePartition rightSrcPartition3(&rightFileLines);
  rightSrcPartition3.PreProcess();

  DiffFilePartition leftDiffPartition3;
  DiffFilePartition rightDiffPartition3;

  diffOk = diffEngine.Diff(leftSrcPartition3,
                           rightSrcPartition3,
                           leftDiffPartition3,
                           rightDiffPartition3);

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition3.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(0)->C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(1)->C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(2)->C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(3)->C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition3.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(0)->C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(1)->C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(2)->C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(3)->C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);
}


