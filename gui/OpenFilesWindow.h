#ifndef OPEN_FILES_WINDOW_H
#define OPEN_FILES_WINDOW_H

#include <intuition/screens.h>
#include "AppScreen.h"
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
    SimpleString& p_LeftFilePath, SimpleString& p_RightFilePath);
  virtual ~OpenFilesWindow();

  /**
   * TODO make virtual?
   */
  void HandleIdcmp(struct IntuiMessage* p_pMsg);


  void Refresh();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  bool Open();

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


//  struct TextAttr m_TextAttr;
//  struct IntuiText m_IntuiText;
};


#endif
