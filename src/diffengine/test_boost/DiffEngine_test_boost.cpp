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
#include <boost/test/framework.hpp>

#include <string>
#include <list>

#include "DiffInputFileLinux.h"
#include "DiffOutputFileLinux.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "ProgressReporter.h"
#include "TextSelection.h"
#include "TextSelectionItem.h"

ProgressReporter progress;

void printFile(DiffFileBase& file)
{
    const char* pStates[] = {"[   ]", "[ADD]", "[CHN]", "[DEL]", "[???]"};
    for(size_t i = 0; i < file.getNumLines(); i++)
    {
      const char* pLineState;
      DiffLine* pLine = file[i];
      switch (pLine->getState())
      {
      case DiffLine::Normal:
        pLineState = pStates[0];
        break;

      case DiffLine::Added:
        pLineState = pStates[1];
        break;

      case DiffLine::Changed:
        pLineState = pStates[2];
        break;

      case DiffLine::Deleted:
        pLineState = pStates[3];
        break;
      
      default:
        pLineState = pStates[4];
        break;
      }

      printf("%s: %s %s\n", pLine->getLineNumText(), pLineState, pLine->getText());
    }
}


/**
 * testcase_02
 *
 *   Left.txt     |   Right.txt
 *   ------------------------------
 *   AAAA         |   AAAA
 *   BBBB         |   BBBB
 *   CCCC         |   1111
 *   DDDD         |   2222
 *   EEEE         |   CCCC
 *   FFFF         |   DDDD
 *   GGGG         |   EEEE
 *                |   FFFF
 *                |   GGGG
 *
 */
BOOST_AUTO_TEST_CASE( testcase_02 )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_02_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_02_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    // printf("Left file:\n");
    // printFile(diffA);
    // printf("\nRight file:\n");
    // printFile(diffB);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 10);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "BBBB");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[5]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[6]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffA[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[7]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[7]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[8]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffA[8]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[9]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffA[9]->getState(), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 10);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "BBBB");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "1111");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "2222");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[5]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[6]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffB[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[7]->getText(), "3333");
    BOOST_CHECK_EQUAL(diffB[7]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[8]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffB[8]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[9]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffB[9]->getState(), DiffLine::Normal);

    // This test has 2 difference blocks of lines added to the right side
    BOOST_CHECK_EQUAL(diffEngine.getNumAdded(), 2);
    BOOST_CHECK_EQUAL(m_DiffIndices.size(), 2);

    // Now verify that the two difference blocks start at the line 
    // indexes 2 and 7
    std::list<size_t>::iterator it = m_DiffIndices.begin();
    BOOST_CHECK_EQUAL((*it++), 2);
    BOOST_CHECK_EQUAL((*it), 7);
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_03_FB101-02-Simple
 *
 *   Left.txt     |   Right.txt
 *   ------------------------------
 *   AAAA         |   AAAA
 *   BBBB         |   FFFF
 *   CCCC         |   CCCC
 *   DDDD         |   DDDD
 *   EEEE         |   EEEE
 *   FFFF         |   FFFF
 *   GGGG         |   GGGG
 *                |
 *
 *   >> Changed line 2
 */
BOOST_AUTO_TEST_CASE( testcase_03_simple )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_03_FB101-02-Simple_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_03_FB101-02-Simple_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 7);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "BBBB");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA[1]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[3]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffA[5]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[5]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffA[6]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffA[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[6]->getLineNumText(), "7 ");



    BOOST_CHECK_EQUAL(diffB.getNumLines(), 7);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB[1]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[2]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[3]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[4]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffB[5]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[5]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffB[6]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffB[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[6]->getLineNumText(), "7 ");
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
  

}



BOOST_AUTO_TEST_CASE( testcase_03_var2 )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_03_var2_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_03_var2_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 8);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "BBBB");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA[1]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[3]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffA[5]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[5]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffA[6]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffA[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[6]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffA[7]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffA[7]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[7]->getLineNumText(), "7 ");

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 8);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "AAAA");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB[1]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "ffff");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[2]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "CCCC");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[3]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "DDDD");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[4]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffB[5]->getText(), "EEEE");
    BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[5]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffB[6]->getText(), "FFFF");
    BOOST_CHECK_EQUAL(diffB[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[6]->getLineNumText(), "7 ");
    BOOST_CHECK_EQUAL(diffB[7]->getText(), "GGGG");
    BOOST_CHECK_EQUAL(diffB[7]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[7]->getLineNumText(), "8 ");
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_04
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *   Line 4         |   Line 4
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 */
BOOST_AUTO_TEST_CASE( testcase_04 )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_04_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_04_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 4);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 4);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_05
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 3         |   Line 2
 *   Line 4         |   Line 3
 *                  |   Line 4
 *                  |
 *
 *   >> Inserted "Line 2" in right file
 *
 */
BOOST_AUTO_TEST_CASE( testcase_05 )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_05_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_05_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 4);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 4);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);

  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_06
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 4         |   Line 2
 *   Line 5         |   Line 3
 *   Line 6         |   Line 4
 *   Line 7         |   Line 5
 *   Line 8         |   Line 8
 *   ab             |   Line 9
 *   Line 9         |   cd
 *   Line 10        |   Line 10
 *
 */
BOOST_AUTO_TEST_CASE( DiffTest_06_Mixed )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_06_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_06_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 12);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[3]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffA[5]->getText(), "Line 6");
    BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA[5]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffA[6]->getText(), "Line 7");
    BOOST_CHECK_EQUAL(diffA[6]->getState(), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA[6]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffA[7]->getText(), "Line 8");
    BOOST_CHECK_EQUAL(diffA[7]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[7]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffA[8]->getText(), "ab");
    BOOST_CHECK_EQUAL(diffA[8]->getState(), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA[8]->getLineNumText(), "7 ");
    BOOST_CHECK_EQUAL(diffA[9]->getText(), "Line 9");
    BOOST_CHECK_EQUAL(diffA[9]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[9]->getLineNumText(), "8 ");
    BOOST_CHECK_EQUAL(diffA[10]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[10]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[10]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffA[11]->getText(), "Line 10");
    BOOST_CHECK_EQUAL(diffA[11]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[11]->getLineNumText(), "9 ");

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 12);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[0]->getLineNumText(), "1 ");
    BOOST_CHECK_EQUAL(diffB[1]->getLineNumText(), "2 ");
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[2]->getLineNumText(), "3 ");
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "Line 4");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[3]->getLineNumText(), "4 ");
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[4]->getLineNumText(), "5 ");
    BOOST_CHECK_EQUAL(diffB[5]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[5]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffB[6]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[6]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[6]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffB[7]->getText(), "Line 8");
    BOOST_CHECK_EQUAL(diffB[7]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[7]->getLineNumText(), "6 ");
    BOOST_CHECK_EQUAL(diffB[8]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[8]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[8]->getLineNumText(), "  ");
    BOOST_CHECK_EQUAL(diffB[9]->getText(), "Line 9");
    BOOST_CHECK_EQUAL(diffB[9]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[9]->getLineNumText(), "7 ");
    BOOST_CHECK_EQUAL(diffB[10]->getText(), "cd");
    BOOST_CHECK_EQUAL(diffB[10]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[10]->getLineNumText(), "8 ");
    BOOST_CHECK_EQUAL(diffB[11]->getText(), "Line 10");
    BOOST_CHECK_EQUAL(diffB[11]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[11]->getLineNumText(), "9 ");

  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_12_endless_loop
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *                  |
 *   Line 5         |   Line 5
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 *
 * Within this test case the first implementation of the old
 * (not Myers) algorithm from 2003 did hang in an endless loop.
 *
 */
BOOST_AUTO_TEST_CASE( testcase_12_endless_loop )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_12_endless_loop_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_12_endless_loop_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 5);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);


    BOOST_CHECK_EQUAL(diffB.getNumLines(), 5);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


/**
 * testcase_12a
 *
 *   Left.txt       |   Right.txt
 *   ------------------------------
 *   Line 1         |   Line 1
 *   Line 2         |   Line 2
 *   Line 3         |
 *                  |
 *   Line 5         |
 *                  |   Line 5
 *                  |
 *
 *   >> Cleared "Line 3" (set to empty) in right file
 *   >> Inserted two empty lines in right file
 *
 *
 */
BOOST_AUTO_TEST_CASE( testcase_12a )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_12a_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_12a_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 6);
    BOOST_CHECK_EQUAL(diffA[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[2]->getText(), "Line 3");
    BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA[3]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[4]->getText(), "");
    BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA[5]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 6);
    BOOST_CHECK_EQUAL(diffB[0]->getText(), "Line 1");
    BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[1]->getText(), "Line 2");
    BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[2]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB[3]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB[4]->getText(), "");
    BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB[5]->getText(), "Line 5");
    BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}


size_t getNextDiffLineId(std::list<size_t> m_DiffIndices, 
                     std::list<size_t>::iterator m_DiffIndicesIterator) 
{
  if(m_DiffIndicesIterator == m_DiffIndices.end())
  {
    // Iterator points to the end. This only is true directly after
    // construction of DiffDocument. Set it to the first item.
    m_DiffIndicesIterator = m_DiffIndices.begin();
  }
  else
  {
    m_DiffIndicesIterator++;

    if(m_DiffIndicesIterator == m_DiffIndices.end())
    {
      // Avoid overflow: back to last valid item
      m_DiffIndicesIterator--;
    }
  }

  return (*m_DiffIndicesIterator);
}

size_t getPrevDiffLineId(std::list<size_t> m_DiffIndices, 
                     std::list<size_t>::iterator m_DiffIndicesIterator) 
{
  if(m_DiffIndicesIterator == m_DiffIndices.end())
  {
    // Iterator points to the end. This only is true directly after
    // construction of DiffDocument. Set it to the first item.
    m_DiffIndicesIterator = m_DiffIndices.begin();
  }
  else if(m_DiffIndicesIterator != m_DiffIndices.begin())
  {
    // Only if not already the first item
    m_DiffIndicesIterator--;
  }

  return (*m_DiffIndicesIterator);
}

// /**
//  * testcase_24_1500_numbers
//  *
//  * 11 changes in two textfiles with about 1500 lines each.
//  * Used to test the navigation between diffs.
//  */
// BOOST_AUTO_TEST_CASE( testcase_24_1500_numbers )
// {
//   try
//   {
//     bool cancelRequested = false;
//     std::list<size_t> m_DiffIndices;

//     DiffInputFileLinux srcA(cancelRequested, 
//                             "testfiles/testcase_24_1500_numbers_left.txt",
//                             true);

//     DiffInputFileLinux srcB(cancelRequested, 
//                             "testfiles/testcase_24_1500_numbers_right.txt",
//                             true);

//     DiffOutputFileLinux diffA(srcA);
//     DiffOutputFileLinux diffB(srcB);
//     DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
//                           "Comparing...", 
//                           cancelRequested, 
//                           m_DiffIndices);

//     std::list<size_t>::iterator m_DiffIndicesIterator = m_DiffIndices.end();

//     // Checking the justification of the line numbers
//     BOOST_CHECK_EQUAL(diffA[0]->getLineNumText(), "   1 ");
//     BOOST_CHECK_EQUAL(diffB[0]->getLineNumText(), "   1 ");
//     BOOST_CHECK_EQUAL(diffA[12]->getLineNumText(), "     ");
//     BOOST_CHECK_EQUAL(diffB[12]->getLineNumText(), "  13 ");

//     long numDiff = diffEngine.getNumDifferences();

//     // Testing if the list with the diff start indexes
//     // contains 11 'difference blocks' as showed with Kompare
//     BOOST_CHECK_EQUAL(m_DiffIndices.size(), 11);
//     BOOST_CHECK_EQUAL(m_DiffIndices.size(), numDiff);

//     // Testing the 'States' of DiffLines by the indexes in the list.
//     // They should be in the order (checked with Kompare)
//     // ADD, CHN, DEL, CHN, CHN, CHN, CHN, ADD, DEL, CHN, CHN
//     DiffLine::LineState states[] =
//     {
//       DiffLine::Added,
//       DiffLine::Changed,
//       DiffLine::Deleted,
//       DiffLine::Changed,
//       DiffLine::Changed,
//       DiffLine::Changed,
//       DiffLine::Changed,
//       DiffLine::Added,
//       DiffLine::Deleted,
//       DiffLine::Changed,
//       DiffLine::Changed,
//     };

//     size_t pIdx = getNextDiffLineId(m_DiffIndices, m_DiffIndicesIterator);
//     int counter = 0;
//     do
//     {
//       if(counter >= (sizeof states / sizeof states[0]))
//       {
//         // Don't exceed the array
//         break;
//       }

//       DiffLine::LineState stateNominal = states[counter];

//       switch(stateNominal)
//       {
//         case DiffLine::LineState::Deleted:
//         {
//           DiffLine::LineState stateA = diffA.GetLineState(pIdx);
//           BOOST_CHECK_EQUAL(stateA, DiffLine::Deleted);
//           break;
//         }

//         case DiffLine::LineState::Added:
//         {
//           DiffLine::LineState stateB = diffB.GetLineState(pIdx);
//           BOOST_CHECK_EQUAL(stateB, DiffLine::Added);
//           break;
//         }

//         case DiffLine::LineState::Changed:
//         {
//           DiffLine::LineState stateA = diffA.GetLineState(pIdx);
//           DiffLine::LineState stateB = diffB.GetLineState(pIdx);
//           BOOST_CHECK_EQUAL(stateA, DiffLine::Changed);
//           BOOST_CHECK_EQUAL(stateB, DiffLine::Changed);
//           break;
//         }
//       }

//       pIdx = getNextDiffLineId(m_DiffIndices, m_DiffIndicesIterator);
//       counter++;
//     }
//     while(pIdx != NULL);
//   }
//   catch(const char* pError)
//   {
//     auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
//     std::string location(locationBoost);
//     printf("Exception in test %s: %s\n", 
//            location.c_str(),
//            pError);
//   }
// }


/**
 * testcase_31
 *
 * Testing the text select / marking ability
 */
BOOST_AUTO_TEST_CASE( DiffTest_31_Marking )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "testfiles/testcase_31_marking_left.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "testfiles/testcase_31_marking_right.txt",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 6);
    BOOST_CHECK_EQUAL(diffB.getNumLines(), 6);

    TextSelection selection;
    selection.Add(1, 1, 2, 7);
    selection.Add(5, 5, 4, 5);
    selection.Add(5, 5, 12, 14);

  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}



BOOST_AUTO_TEST_CASE( testcase_crash )
{
  try
  {
    bool cancelRequested = false;
    std::list<size_t> m_DiffIndices;

    DiffInputFileLinux srcA(cancelRequested, 
                            "CMakeLists.txt",
                            true);

    DiffInputFileLinux srcB(cancelRequested, 
                            "LICENSE-3RD-PARTY",
                            true);

    DiffOutputFileLinux diffA(srcA);
    DiffOutputFileLinux diffB(srcB);
    DiffEngine diffEngine(srcA, srcB, diffA, diffB, progress,
                          "Comparing...", cancelRequested, m_DiffIndices);

    // printf("Left file:\n");
    // printFile(diffA);
    // printf("\nRight file:\n");
    // printFile(diffB);

    BOOST_CHECK_EQUAL(diffEngine.getNumDifferences(), 7);
    BOOST_CHECK_EQUAL(diffEngine.getNumAdded(), 5);
    BOOST_CHECK_EQUAL(diffEngine.getNumDeleted(), 0);
    BOOST_CHECK_EQUAL(diffEngine.getNumChanged(), 2);

    BOOST_CHECK_EQUAL(diffA.getNumLines(), 36);
    // BOOST_CHECK_EQUAL(diffA[0]->getText(), "");
    // BOOST_CHECK_EQUAL(diffA[0]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[0]->getLineNumText(), "");
    // BOOST_CHECK_EQUAL(diffA[1]->getText(), "cmake_minimum_required (VERSION 2.8.11)");
    // BOOST_CHECK_EQUAL(diffA[1]->getState(), DiffLine::Changed);
    // BOOST_CHECK_EQUAL(diffA[1]->getLineNumText(), "2 ");

    
    // BOOST_CHECK_EQUAL(diffA[2]->getText(), "");
    // BOOST_CHECK_EQUAL(diffA[2]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[2]->getLineNumText(), "");
    // BOOST_CHECK_EQUAL(diffA[3]->getText(), "CCCC");
    // BOOST_CHECK_EQUAL(diffA[3]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[3]->getLineNumText(), "3 ");
    // BOOST_CHECK_EQUAL(diffA[4]->getText(), "DDDD");
    // BOOST_CHECK_EQUAL(diffA[4]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[4]->getLineNumText(), "4 ");
    // BOOST_CHECK_EQUAL(diffA[5]->getText(), "EEEE");
    // BOOST_CHECK_EQUAL(diffA[5]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[5]->getLineNumText(), "5 ");
    // BOOST_CHECK_EQUAL(diffA[6]->getText(), "FFFF");
    // BOOST_CHECK_EQUAL(diffA[6]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[6]->getLineNumText(), "6 ");
    // BOOST_CHECK_EQUAL(diffA[7]->getText(), "GGGG");
    // BOOST_CHECK_EQUAL(diffA[7]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA[7]->getLineNumText(), "7 ");

    BOOST_CHECK_EQUAL(diffB.getNumLines(), 36);
    // BOOST_CHECK_EQUAL(diffB[0]->getText(), "AAAA");
    // BOOST_CHECK_EQUAL(diffB[0]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[0]->getLineNumText(), "1 ");
    // BOOST_CHECK_EQUAL(diffB[1]->getText(), "FFFF");
    // BOOST_CHECK_EQUAL(diffB[1]->getState(), DiffLine::Changed);
    // BOOST_CHECK_EQUAL(diffB[1]->getLineNumText(), "2 ");
    // BOOST_CHECK_EQUAL(diffB[2]->getText(), "ffff");
    // BOOST_CHECK_EQUAL(diffB[2]->getState(), DiffLine::Added);
    // BOOST_CHECK_EQUAL(diffB[2]->getLineNumText(), "3 ");
    // BOOST_CHECK_EQUAL(diffB[3]->getText(), "CCCC");
    // BOOST_CHECK_EQUAL(diffB[3]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[3]->getLineNumText(), "4 ");
    // BOOST_CHECK_EQUAL(diffB[4]->getText(), "DDDD");
    // BOOST_CHECK_EQUAL(diffB[4]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[4]->getLineNumText(), "5 ");
    // BOOST_CHECK_EQUAL(diffB[5]->getText(), "EEEE");
    // BOOST_CHECK_EQUAL(diffB[5]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[5]->getLineNumText(), "6 ");
    // BOOST_CHECK_EQUAL(diffB[6]->getText(), "FFFF");
    // BOOST_CHECK_EQUAL(diffB[6]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[6]->getLineNumText(), "7 ");
    // BOOST_CHECK_EQUAL(diffB[7]->getText(), "GGGG");
    // BOOST_CHECK_EQUAL(diffB[7]->getState(), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB[7]->getLineNumText(), "8 ");
  }
  catch(const char* pError)
  {
    auto locationBoost = boost::unit_test::framework::current_test_case().p_name;
    std::string location(locationBoost);
    printf("Exception in test %s: %s\n", 
           location.c_str(),
           pError);
  }
}
