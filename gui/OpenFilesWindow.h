#ifndef OPEN_FILES_WINDOW_H
#define OPEN_FILES_WINDOW_H

#include <intuition/screens.h>
#include "AppScreen.h"
#include "SimpleString.h"
#include "TextDocument.h"
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
  OpenFilesWindow(AppScreen* p_pAppScreen);
  virtual ~OpenFilesWindow();

  /**
   * TODO make virtual?
   */
  void HandleIdcmp(struct IntuiMessage* p_pMsg);


  /**
   * IDs to allow to interpret the events of this window's BOOPSI system
   * gadgets in the Application event loop.
   */
  enum OFW_GADGET_ID
  {
    OFW_LEFT_FILE_STRING,
    OFW_RIGHT_FILE_STRING,
    OFW_LEFT_FILE_BUTTON,
    OFW_RIGHT_FILE_BUTTON,
    OFW_DIFF_BUTTON,
    OFW_CANCEL_BUTTON,
  };


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
  WORD m_FontHeight;  ///> Height of current text font
  WORD m_WinWidth;
  WORD m_WinHeight;

  struct Gadget* m_pLeftFileStringGadget;      ///> horizontal scrollbar
  struct Gadget* m_pRightFileStringGadget;      ///> vertical scrollbar
  struct Gadget* m_pOpenLeftFileButton;  ///> h-scrollbar left button
  struct Gadget* m_pOpenRightFileButton; ///> h-scrollbar right button
  struct Gadget* m_pDiffButton;    ///> v-scrollbar up button
  struct Gadget* m_pCancelButton;  ///> v-scrollbar down button

  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;
};


#endif
