#ifndef OPEN_FILES_WINDOW_H
#define OPEN_FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AmigaDiffFacade.h"
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
  OpenFilesWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
    AmigaDiffFacade& p_DiffFacade);
  virtual ~OpenFilesWindow();

  /**
   * TODO make virtual?
   */
  void HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);


  void Refresh();

  /**
   * Opening the window.
   *
   * @param p_pUserDataMenuItemToDisable
   * A pointer to an user data field of a menu item which is associated
   * with this window. If the menu item is found by the given user data
   * it will be disabled at window opening time and enabled when the
   * window is closed. Provide NULL if no menu item should be disabled.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  bool Open(APTR p_pUserDataMenuItemToDisable = NULL,
    InitialWindowPosition p_pInitialPosition = IWP_Center);

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

  AmigaDiffFacade& m_DiffFacade;


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

  /**
   * Enables or disables the buttons of this window in dependency of
   * some conditions, e.g. is text in both string gadgets or is there
   * an ASL request openened etc.
   */
  void setDiffButtonState();

  void setStringGadgetText(struct Gadget* p_pGadget,
    const SimpleString& p_Text);
};


#endif
