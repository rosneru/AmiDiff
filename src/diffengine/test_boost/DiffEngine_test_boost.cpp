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

ProgressReporter progress;

void printFile(DiffFileBase& file)
{
    const char* pStates[] = {"[   ]", "[ADD]", "[CHN]", "[DEL]", "[???]"};
    for(size_t i = 0; i < file.NumLines(); i++)
    {
      const char* pLineState;
      DiffLine* pLine = file.GetLine(i);
      switch (pLine->State())
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

      printf("%s: %s %s\n", pLine->LineNum(), pLineState, pLine->Txt());
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 10);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "CCCC");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(5), "DDDD");
    BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(6), "EEEE");
    BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(7), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(8), "FFFF");
    BOOST_CHECK_EQUAL(diffA.GetLineState(8), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(9), "GGGG");
    BOOST_CHECK_EQUAL(diffA.GetLineState(9), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.NumLines(), 10);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "BBBB");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "1111");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "2222");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "CCCC");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(5), "DDDD");
    BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(6), "EEEE");
    BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(7), "3333");
    BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(8), "FFFF");
    BOOST_CHECK_EQUAL(diffB.GetLineState(8), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(9), "GGGG");
    BOOST_CHECK_EQUAL(diffB.GetLineState(9), DiffLine::Normal);

    // This test has 2 difference blocks of lines added to the right side
    BOOST_CHECK_EQUAL(diffEngine.NumAdded(), 2);
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 7);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(0), "1 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(1), "2 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "CCCC");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(2), "3 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "DDDD");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(3), "4 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "EEEE");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(4), "5 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(5), "FFFF");
    BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(5), "6 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(6), "GGGG");
    BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(6), "7 ");



    BOOST_CHECK_EQUAL(diffB.NumLines(), 7);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(0), "1 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "FFFF");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(1), "2 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "CCCC");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(2), "3 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "DDDD");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(3), "4 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "EEEE");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(4), "5 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(5), "FFFF");
    BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(5), "6 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(6), "GGGG");
    BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(6), "7 ");
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 8);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(0), "1 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "BBBB");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(1), "2 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(2), "  ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "CCCC");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(3), "3 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "DDDD");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(4), "4 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(5), "EEEE");
    BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(5), "5 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(6), "FFFF");
    BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(6), "6 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(7), "GGGG");
    BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(7), "7 ");

    BOOST_CHECK_EQUAL(diffB.NumLines(), 8);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(0), "1 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "FFFF");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(1), "2 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "ffff");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(2), "3 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "CCCC");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(3), "4 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "DDDD");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(4), "5 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(5), "EEEE");
    BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(5), "6 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(6), "FFFF");
    BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(6), "7 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(7), "GGGG");
    BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(7), "8 ");
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 4);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.NumLines(), 4);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 4);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.NumLines(), 4);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);

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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 12);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLine(0)->LineNum(), "1 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(1), "  ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(2), "  ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(3), "2 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "Line 5");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(4), "3 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(5), "Line 6");
    BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(5), "4 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(6), "Line 7");
    BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(6), "5 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(7), "Line 8");
    BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(7), "6 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(8), "ab");
    BOOST_CHECK_EQUAL(diffA.GetLineState(8), DiffLine::Deleted);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(8), "7 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(9), "Line 9");
    BOOST_CHECK_EQUAL(diffA.GetLineState(9), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(9), "8 ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(10), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(10), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(10), "  ");
    BOOST_CHECK_EQUAL(diffA.GetLineText(11), "Line 10");
    BOOST_CHECK_EQUAL(diffA.GetLineState(11), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineNum(11), "9 ");

    BOOST_CHECK_EQUAL(diffB.NumLines(), 12);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(0), "1 ");
    BOOST_CHECK_EQUAL(diffB.GetLineNum(1), "2 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(2), "3 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "Line 4");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(3), "4 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "Line 5");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(4), "5 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(5), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(5), "  ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(6), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(6), "  ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(7), "Line 8");
    BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(7), "6 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(8), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(8), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(8), "  ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(9), "Line 9");
    BOOST_CHECK_EQUAL(diffB.GetLineState(9), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(9), "7 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(10), "cd");
    BOOST_CHECK_EQUAL(diffB.GetLineState(10), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(10), "8 ");
    BOOST_CHECK_EQUAL(diffB.GetLineText(11), "Line 10");
    BOOST_CHECK_EQUAL(diffB.GetLineState(11), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineNum(11), "9 ");

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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 5);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "Line 5");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);


    BOOST_CHECK_EQUAL(diffB.NumLines(), 5);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "Line 5");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
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

    BOOST_CHECK_EQUAL(diffA.NumLines(), 6);
    BOOST_CHECK_EQUAL(diffA.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(2), "Line 3");
    BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffA.GetLineText(3), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(4), "");
    BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffA.GetLineText(5), "Line 5");
    BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);

    BOOST_CHECK_EQUAL(diffB.NumLines(), 6);
    BOOST_CHECK_EQUAL(diffB.GetLineText(0), "Line 1");
    BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(1), "Line 2");
    BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(2), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Changed);
    BOOST_CHECK_EQUAL(diffB.GetLineText(3), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Added);
    BOOST_CHECK_EQUAL(diffB.GetLineText(4), "");
    BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    BOOST_CHECK_EQUAL(diffB.GetLineText(5), "Line 5");
    BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
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


size_t NextDiffIndex(std::list<size_t> m_DiffIndices, 
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

size_t PrevDiffIndex(std::list<size_t> m_DiffIndices, 
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
//     BOOST_CHECK_EQUAL(diffA.GetLineNum(0), "   1 ");
//     BOOST_CHECK_EQUAL(diffB.GetLineNum(0), "   1 ");
//     BOOST_CHECK_EQUAL(diffA.GetLineNum(12), "     ");
//     BOOST_CHECK_EQUAL(diffB.GetLineNum(12), "  13 ");

//     long numDiff = diffEngine.NumDifferences();

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

//     size_t pIdx = NextDiffIndex(m_DiffIndices, m_DiffIndicesIterator);
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

//       pIdx = NextDiffIndex(m_DiffIndices, m_DiffIndicesIterator);
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

    BOOST_CHECK_EQUAL(diffEngine.NumDifferences(), 7);
    BOOST_CHECK_EQUAL(diffEngine.NumAdded(), 5);
    BOOST_CHECK_EQUAL(diffEngine.NumDeleted(), 0);
    BOOST_CHECK_EQUAL(diffEngine.NumChanged(), 2);

    BOOST_CHECK_EQUAL(diffA.NumLines(), 36);
    // BOOST_CHECK_EQUAL(diffA.GetLineText(0), "");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(0), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(0), "");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(1), "cmake_minimum_required (VERSION 2.8.11)");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(1), DiffLine::Changed);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(1), "2 ");

    
    // BOOST_CHECK_EQUAL(diffA.GetLineText(2), "");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(2), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(2), "");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(3), "CCCC");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(3), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(3), "3 ");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(4), "DDDD");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(4), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(4), "4 ");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(5), "EEEE");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(5), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(5), "5 ");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(6), "FFFF");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(6), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(6), "6 ");
    // BOOST_CHECK_EQUAL(diffA.GetLineText(7), "GGGG");
    // BOOST_CHECK_EQUAL(diffA.GetLineState(7), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffA.GetLineNum(7), "7 ");

    BOOST_CHECK_EQUAL(diffB.NumLines(), 36);
    // BOOST_CHECK_EQUAL(diffB.GetLineText(0), "AAAA");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(0), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(0), "1 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(1), "FFFF");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(1), DiffLine::Changed);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(1), "2 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(2), "ffff");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(2), DiffLine::Added);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(2), "3 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(3), "CCCC");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(3), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(3), "4 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(4), "DDDD");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(4), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(4), "5 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(5), "EEEE");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(5), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(5), "6 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(6), "FFFF");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(6), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(6), "7 ");
    // BOOST_CHECK_EQUAL(diffB.GetLineText(7), "GGGG");
    // BOOST_CHECK_EQUAL(diffB.GetLineState(7), DiffLine::Normal);
    // BOOST_CHECK_EQUAL(diffB.GetLineNum(7), "8 ");
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
