#ifndef ADIFFVIEW_ARGUMENTS_H
#define ADIFFVIEW_ARGUMENTS_H

#include "SimpleString.h"

/**
 * Class to parse and store the command line / workbench arguments 
 * retrieved at the start of ADiffView.
 *
 * @author Uwe Rosner
 * @date 06/12/2019
 */
class ADiffViewArguments
{
public:

  /**
   * Creates the options object. On creation it extracts the arguments
   * which on program start optionally can be provided via Workbench 
   * or CLI.
   *
   * @param argc 
   * The argc variable from main().
   *
   * @param argv 
   * The argv array from main().
   *
   */
  ADiffViewArguments(int argc, char **argv);

  virtual ~ADiffViewArguments();

  SimpleString& PubScreenName();
  SimpleString& LeftFile();
  SimpleString& RightFile();
  bool DontAsk();

private:
  int m_ArgC;
  char** m_pArgV;

  SimpleString m_PubScreenName;
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;
  bool m_bDontAsk;
  
  void readWorkbenchArgs();
  void readCommandLineArgs();
};

#endif // ADIFFVIEW_ARGUMENTS_H
