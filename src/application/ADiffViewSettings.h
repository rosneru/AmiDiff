#ifndef ADIFFVIEW_SETTINGS_H
#define ADIFFVIEW_SETTINGS_H

#include "SimpleString.h"

/**
 * Class to store the settings of ADiffView.
 *
 * @author Uwe Rosner
 * @date 09/12/2019
 */
class ADiffViewOptions
{
public:

  /**
   * Creates the options object. On creation it extracts the arguments
   * which on program start optionally can be provided via Workbench 
   * or CLI.
   *
   * @param argc The argc variable from main()
   * @param argv The argv array from main.
   *
   */
  ADiffViewOptions(int argc, char **argv);

  virtual ~ADiffViewOptions();

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

#endif // ADIFFVIEW_SETTINGS_H
