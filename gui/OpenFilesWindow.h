#ifndef OPEN_FILES_WINDOW_H
#define OPEN_FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>
#include "AppScreen.h"
#include "Command.h"
#include "SimpleString.h"
#include "WindowBase.h"

/**
 * Class for the window to open the two files on which the diff will
 * be performed.
 *
 *
 * @author Uwe Rosner
 * @date 21/10/2018
 */
class OpenFilesWindow : public WindowBase
{
public:
  OpenFilesWindow(AppScreen* p_pAppScreen,
    SimpleString& p_LeftFilePath, SimpleString& p_RightFilePath, Command& p_DiffCommand);
  virtual ~OpenFilesWindow();

  /**
   * TODO make virtual?
   */
  void HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);


  void Refresh();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  bool Open(struct MsgPort* p_pMsgPort);

  /**
   * Closes the window
   */
  virtual void Close();

private:
  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets in the Application event loop.
   */
  enum GadgetId
  {
    GID_LeftFileString,
    GID_RightFileString,
    GID_LeftFileButton,
    GID_RightFileButton,
    GID_DiffButton,
    GID_CancelButton,
  };

  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;
  Command& m_DiffCommand;

  WORD m_FontHeight;  ///> Height of current text font
  WORD m_WinWidth;
  WORD m_WinHeight;

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pLeftFileStringGadget;
  struct Gadget* m_pRightFileStringGadget;
  struct Gadget* m_pOpenLeftFileButton;
  struct Gadget* m_pOpenRightFileButton;
  struct Gadget* m_pDiffButton;
  struct Gadget* m_pCancelButton;
};


#endif
