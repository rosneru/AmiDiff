#ifndef ADIFFVIEW_ARGS_H
#define ADIFFVIEW_ARGS_H

#include <string>

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
  const std::string& PubScreenName() const;

  /**
   * Path to left file
   */
  const std::string& LeftFile() const;

  /**
   * Path to right file
   */
  const std::string& RightFile() const;

  /**
   * If true, the diff will be performed immediately without waiting 
   * for the user to click the "Diff" button in OpenFilesWindow. 
   * 
   * This only works when both files, left and right are also passed 
   * as arguments.
   * 
   * Defaults to false.
   */
  bool DontAsk() const;

  /**
   * If true, no AppIcon for file drag'n'drop is installed on the
   * Workbench
   */
  bool NoAppIcon();

  /**
   * If true, the line numbers will be shown in the diff result window.
   *
   * Defaults to true; 
   */
  bool ShowLineNumbers() const;

private:
  int m_ArgC;
  char** m_pArgV;

  std::string m_PubScreenName;
  std::string m_LeftFilePath;
  std::string m_RightFilePath;
  bool m_bDontAsk;
  bool m_bNoAppIcon;
  bool m_bShowLineNumbers;

  void readWorkbenchArgs();
  void readCommandLineArgs();

  /**
   * Uses FindTooltype to find and return the value of given name.
   * Returns NULL if tooltype not found.
   * 
   * Is basically just a wrapper around FindTooltype with the proper
   * types needed. Helps to avoid some casts and STRPTR compiler 
   * warnings in readWorkbenchArgs()
   */
  char* toolTypeValue(const STRPTR* ppTooltypeArray, 
                      const char* pTooltypeName);
};

#endif
