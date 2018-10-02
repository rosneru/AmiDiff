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

BOOST_AUTO_TEST_CASE( textDocument )
{
  //
  // Creating a first pair of diff files
  //
  LinkedList leftFileLines;
  leftFileLines.InsertTail(new SimpleString("Line 1"));
  leftFileLines.InsertTail(new SimpleString("Line 3"));
  leftFileLines.InsertTail(new SimpleString("Line 4"));

  LinkedList rightFileLines;
  rightFileLines.InsertTail(new SimpleString("Line 1"));
  rightFileLines.InsertTail(new SimpleString("Line 2"));
  rightFileLines.InsertTail(new SimpleString("Line 3"));
  rightFileLines.InsertTail(new SimpleString("Line 4"));

  DiffFilePartition leftFilePartition(&leftFileLines);
  leftFilePartition.PreProcess();

  // clean up
  deleteAllListStrings(leftFileLines);
  deleteAllListStrings(rightFileLines);




//  // Is it empty initialized?
//  TextDocument textDocument;
//  BOOST_CHECK_EQUAL(textDocument.GetCurrentLine()->C_str(), "");
//  BOOST_CHECK_EQUAL(textDocument.X(), 0);
//  BOOST_CHECK_EQUAL(textDocument.Y(), 0);

}
