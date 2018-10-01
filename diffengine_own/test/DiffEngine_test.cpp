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

BOOST_AUTO_TEST_CASE( textDocument )
{
  //
  // Testing normal typing, cursor left, cursor right and return
  //

//  // Is it empty initialized?
//  TextDocument textDocument;
//  BOOST_CHECK_EQUAL(textDocument.GetCurrentLine()->C_str(), "");
//  BOOST_CHECK_EQUAL(textDocument.X(), 0);
//  BOOST_CHECK_EQUAL(textDocument.Y(), 0);

}
