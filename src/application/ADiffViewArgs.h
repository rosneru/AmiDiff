#ifndef ADIFFVIEW_ARGS_H
#define ADIFFVIEW_ARGS_H

#include "SimpleString.h"

/**
 * Class to parse and store the command line / workbench arguments
 * retrieved at the start of ADiffView.
 *
 * @author Uwe Rosner
 * @date 06/12/2019
 */
class ADiffViewArgs
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
  ADiffViewArgs(int argc, char **argv);

  virtual ~ADiffViewArgs();

  /**
   * Name of the public screen to run ADiffView on
   * If empty a own screen will be opened (Workbench clone, 8 colors)
   */
  SimpleString& PubScreenName();

  /**
   * Path to left file
   */
  SimpleString& LeftFile();

  /**
   * Path to right file
   */
  SimpleString& RightFile();

  /**
   * If true, the diff will be performed immediately without waiting 
   * for the user to click the "Diff" button in OpenFilesWindow. 
   * 
   * This only works when both files, left and right are also passed 
   * as arguments.
   */
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

#endif // ADIFFVIEW_ARGS_H