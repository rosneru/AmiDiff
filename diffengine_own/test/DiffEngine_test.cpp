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

void deleteAllListStrings(LinkedList& p_List)
{
  SimpleString* pItem = static_cast<SimpleString*>(p_List.GetFirst());
  while(pItem != NULL)
  {
    delete pItem;
    pItem = static_cast<SimpleString*>(p_List.GetNext());
  }
}

BOOST_AUTO_TEST_CASE( testMatch )
{
  //
  // Test case 1: Left file is empty
  //
  LinkedList leftFileLines; // Note that left file stays empty

  LinkedList rightFileLines;
  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 3"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcFilePartition(&leftFileLines);
  leftSrcFilePartition.PreProcess();

  DiffFilePartition rightSrcFilePartition(&rightFileLines);
  rightSrcFilePartition.PreProcess();

  DiffFilePartition leftDestFilePartition;
  DiffFilePartition rightDestFilePartition;

  DiffEngine diffEngine;
  bool diffOk = diffEngine.Diff(leftSrcFilePartition,
                                rightSrcFilePartition,
                                leftDestFilePartition,
                                rightDestFilePartition);

  BOOST_CHECK_EQUAL(diffOk, true);

  // Left destination file should contain 4 empty lines with the LineState "Normal"
  BOOST_CHECK_EQUAL(leftDestFilePartition.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedRawLine(0)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedRawLine(1)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedRawLine(2)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedRawLine(3)->C_str(), "");
  BOOST_CHECK_EQUAL(leftDestFilePartition.GetIndexedLineState(3), DiffLine::Normal);

  // Right destination file should contain the 4 lines as inserted above
  BOOST_CHECK_EQUAL(rightDestFilePartition.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedRawLine(0)->C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedRawLine(1)->C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedRawLine(2)->C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedRawLine(3)->C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDestFilePartition.GetIndexedLineState(3), DiffLine::Normal);


  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);


  //
  // Test case 3: Deleted "Line 3" in right file
  //

//  leftFileLines.InsertTail(new SimpleString("Line 1"));
//  leftFileLines.InsertTail(new SimpleString("Line 2"));
//  leftFileLines.InsertTail(new SimpleString("Line 3"));
//  leftFileLines.InsertTail(new SimpleString("Line 4"));

//  rightFileLines.InsertTail(new SimpleString("Line 1"));
//  rightFileLines.InsertTail(new SimpleString("Line 2"));
//  rightFileLines.InsertTail(new SimpleString("Line 3"));
//  rightFileLines.InsertTail(new SimpleString("Line 4"));

//  // clean up
//  deleteAllListStrings(leftFileLines);
//  deleteAllListStrings(rightFileLines);
}

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
