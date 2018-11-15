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



BOOST_AUTO_TEST_CASE( testFilePartition )
{
  SimpleString text1 = "abc";
  SimpleString text2 = "defg";

  DiffFilePartition partition1(NULL);
  partition1.AddString(text1);
  partition1.AddBlankLine();
  partition1.AddString(text2);
  partition1.AddString(text1);

  BOOST_CHECK_EQUAL(partition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(0)->GetLine().C_str(), "abc");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(1)->GetLine().C_str(), "");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(2)->GetLine().C_str(), "defg");
  BOOST_CHECK_EQUAL(partition1.GetIndexedDiffLine(3)->GetLine().C_str(), "abc");
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



BOOST_AUTO_TEST_CASE( testDiff_PassAddString )
{
  bool diffOk = false;
  DiffEngine diffEngine;

  //
  // Test case 3
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
  leftSrcPartition1.PreProcess("../../testfiles/Testcase_03_Left.txt");

  DiffFilePartitionLinux rightSrcPartition1;
  rightSrcPartition1.PreProcess("../../testfiles/Testcase_03_Right.txt");

  DiffFilePartition leftDiffPartition1;
  DiffFilePartition rightDiffPartition1;

  diffOk = diffEngine.Diff(leftSrcPartition1,
                           rightSrcPartition1,
                           leftDiffPartition1,
                           rightDiffPartition1);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(3), DiffLine::Normal);

  // Extra: Repeat test for left diff partition but use GetFirst../
  // GetNext.. instead of GetIndexed.. methods
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetFirstDiffLine()->GetLine().C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetCurrentDiffLine()->GetState(), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetNextDiffLine()->GetLine().C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetCurrentDiffLine()->GetState(), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetNextDiffLine()->GetLine().C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetCurrentDiffLine()->GetState(), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetNextDiffLine()->GetLine().C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetCurrentDiffLine()->GetState(), DiffLine::Normal);

  //
  // Test case 4
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
  leftSrcPartition2.PreProcess("../../testfiles/Testcase_04_Left.txt");

  DiffFilePartitionLinux rightSrcPartition2;
  rightSrcPartition2.PreProcess("../../testfiles/Testcase_04_Right.txt");

  DiffFilePartition leftDiffPartition2;
  DiffFilePartition rightDiffPartition2;

  diffOk = diffEngine.Diff(leftSrcPartition2,
                           rightSrcPartition2,
                           leftDiffPartition2,
                           rightDiffPartition2);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition2.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition2.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(3), DiffLine::Normal);



  //
  // Test case 5
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
  leftSrcPartition3.PreProcess("../../testfiles/Testcase_05_Left.txt");

  DiffFilePartitionLinux rightSrcPartition3;
  rightSrcPartition3.PreProcess("../../testfiles/Testcase_05_Right.txt");

  DiffFilePartition leftDiffPartition3;
  DiffFilePartition rightDiffPartition3;

  diffOk = diffEngine.Diff(leftSrcPartition3,
                           rightSrcPartition3,
                           leftDiffPartition3,
                           rightDiffPartition3);

  BOOST_CHECK_EQUAL(diffOk, true);

  BOOST_CHECK_EQUAL(leftDiffPartition3.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition3.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);
}

BOOST_AUTO_TEST_CASE( testDiff_PassFileListInConstructor )
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
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(0).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedRawLine(3).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition1.GetIndexedLineState(3), DiffLine::Normal);

  // Right destination file should contain the 4 lines as inserted above
  BOOST_CHECK_EQUAL(rightDiffPartition1.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition1.GetIndexedRawLine(3).C_str(), "Line 4");
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
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(0), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(1), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition2.GetIndexedLineState(3), DiffLine::Deleted);

  // Right destination file should contain 4 empty lines with the LineState "Normal"
  BOOST_CHECK_EQUAL(rightDiffPartition2.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(0).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(1).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition2.GetIndexedRawLine(3).C_str(), "");
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
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition3.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition3.GetIndexedLineState(3), DiffLine::Normal);

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);

  //
  // Test case 4: Cleared "Line 3" (set to empty) in right file
  //

  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 2"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));

  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString(""));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcPartition4(&leftFileLines);
  leftSrcPartition4.PreProcess();

  DiffFilePartition rightSrcPartition4(&rightFileLines);
  rightSrcPartition4.PreProcess();

  DiffFilePartition leftDiffPartition4;
  DiffFilePartition rightDiffPartition4;

  diffOk = diffEngine.Diff(leftSrcPartition4,
                           rightSrcPartition4,
                           leftDiffPartition4,
                           rightDiffPartition4);

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition4.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition4.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition4.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedLineState(2), DiffLine::Changed);
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition4.GetIndexedLineState(3), DiffLine::Normal);

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);

  //
  // Test case 5: Deleted "Line 2" in left file
  //

  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));

  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 3"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftSrcPartition5(&leftFileLines);
  leftSrcPartition5.PreProcess();

  DiffFilePartition rightSrcPartition5(&rightFileLines);
  rightSrcPartition5.PreProcess();

  DiffFilePartition leftDiffPartition5;
  DiffFilePartition rightDiffPartition5;

  diffOk = diffEngine.Diff(leftSrcPartition5,
                           rightSrcPartition5,
                           leftDiffPartition5,
                           rightDiffPartition5);

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition5.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedRawLine(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition5.GetIndexedLineState(3), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition5.NumberOfLines(), 4);
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition5.GetIndexedLineState(3), DiffLine::Normal);

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);

  //
  // Test case 6: Mixed it up with 6 changes
  //

  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));
  leftFileLines.InsertTail(new SimpleString("Line 5"));
  leftFileLines.InsertTail(new SimpleString("Line 6"));
  leftFileLines.InsertTail(new SimpleString("Line 7"));
  leftFileLines.InsertTail(new SimpleString("Line 8"));
  leftFileLines.InsertTail(new SimpleString("ab"));
  leftFileLines.InsertTail(new SimpleString("Line 9"));
  leftFileLines.InsertTail(new SimpleString("Line 10"));

  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 3"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));
  rightFileLines.InsertTail(new SimpleString("Line 5"));
  rightFileLines.InsertTail(new SimpleString("Line 8"));
  rightFileLines.InsertTail(new SimpleString("Line 9"));
  rightFileLines.InsertTail(new SimpleString("cd"));
  rightFileLines.InsertTail(new SimpleString("Line 10"));

  DiffFilePartition leftSrcPartition6(&leftFileLines);
  leftSrcPartition6.PreProcess();

  DiffFilePartition rightSrcPartition6(&rightFileLines);
  rightSrcPartition6.PreProcess();

  DiffFilePartition leftDiffPartition6;
  DiffFilePartition rightDiffPartition6;

  diffOk = diffEngine.Diff(leftSrcPartition6,
                           rightSrcPartition6,
                           leftDiffPartition6,
                           rightDiffPartition6);

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition6.NumberOfLines(), 12);
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedRawLine(1).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(leftDiffPartition6.GetIndexedLineState(3), DiffLine::Normal);
  // TODO to be continued

  BOOST_CHECK_EQUAL(rightDiffPartition6.NumberOfLines(), 12);
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedLineState(1), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedLineState(2), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedRawLine(3).C_str(), "Line 4");
  BOOST_CHECK_EQUAL(rightDiffPartition6.GetIndexedLineState(3), DiffLine::Normal);


  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);


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

  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 2"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString(""));
  leftFileLines.InsertTail(new SimpleString("Line 5"));

  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString(""));
  rightFileLines.InsertTail(new SimpleString(""));
  rightFileLines.InsertTail(new SimpleString("Line 5"));

  DiffFilePartition leftSrcPartition7(&leftFileLines);
  leftSrcPartition7.PreProcess();

  DiffFilePartition rightSrcPartition7(&rightFileLines);
  rightSrcPartition7.PreProcess();

  DiffFilePartition leftDiffPartition7;
  DiffFilePartition rightDiffPartition7;

  diffOk = diffEngine.Diff(leftSrcPartition7,
                           rightSrcPartition7,
                           leftDiffPartition7,
                           rightDiffPartition7);

  BOOST_CHECK_EQUAL(diffOk, true);


  BOOST_CHECK_EQUAL(leftDiffPartition7.NumberOfLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(2).C_str(), "Line 3");
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(2), DiffLine::Deleted);
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(3).C_str(), "");
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(4).C_str(), ""); // NOTE: This is a failure. Should be "Line 5" instead.
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(4), DiffLine::Normal);
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedRawLine(5).C_str(), "Line 5");
  BOOST_CHECK_EQUAL(leftDiffPartition7.GetIndexedLineState(5), DiffLine::Normal);

  BOOST_CHECK_EQUAL(rightDiffPartition7.NumberOfLines(), 6); // // NOTE: This is a failure. Should be "5" instead.
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(0).C_str(), "Line 1");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(0), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(1).C_str(), "Line 2");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(1), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(2).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(2), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(3).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(3), DiffLine::Normal);
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(4).C_str(), "");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(4), DiffLine::Added);
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedRawLine(5).C_str(), "Line 5");
  BOOST_CHECK_EQUAL(rightDiffPartition7.GetIndexedLineState(5), DiffLine::Normal);

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);
}
