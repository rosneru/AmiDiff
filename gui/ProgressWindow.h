#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <exec/ports.h>
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
class ProgressWindow : public WindowBase
{
public:
  ProgressWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort);
  virtual ~ProgressWindow();

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
  bool Open(APTR p_pMenuItemDisableAtOpen = NULL);

  /**
   * Handles given IDCMP event.
   *
   * @returns If this event was handled: true; else: false.
   */
  bool HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

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

  WORD m_FontHeight;  ///> Height of current text font

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pLeftFileStringGadget;
  struct Gadget* m_pRightFileStringGadget;
  struct Gadget* m_pSelectLeftFileButton;
  struct Gadget* m_pSelectRightFileButton;
  struct Gadget* m_pDiffButton;
  struct Gadget* m_pCancelButton;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();


  /**
   * Enables all gadgets in the window except the 'Diff' button which
   * only is enabled when both string gadgets contain text.
   */
  void enableAll();

  /**
   * Disables all gadgets in the window.
   *
   * Should be called e.g. before an ASL reuest is opened to prevent
   * from input during request is open.
   */
  void disableAll();

};


#endif
