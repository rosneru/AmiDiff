#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "WindowBase.h"

/**
 * Window to display a progress bar
 *
 * @author Uwe Rosner
 * @date 02/02/2019
 */
class ProgressWindow : public WindowBase
{
public:
  ProgressWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
                 int& p_NumWindowsOpen, bool& p_bCancelRequested);

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
  bool Open(const APTR p_pMenuItemDisableAtOpen = NULL);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  bool HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  /**
   * Handles the given progress event.
   *
   * Prints the current pDescription text of given p_pProgressMsg and
   * draws a progress bar representing the actual percentual value.
   */
  void HandleProgress(struct WorkerProgressMsg* p_pProgressMsg);


private:
  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets
   */
  enum GadgetId
  {
    GID_Description,
    GID_ButtonCancel,
  };

  WORD m_FontHeight;  ///> Height of current text font

  bool& m_bCancelRequested;

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pLabelDescription;
  struct Gadget* m_pButtonCancel;

  int m_ProgressBarLeft;
  int m_ProgressBarTop;
  int m_ProgressBarWidth;
  int m_ProgressBarHeight;

  struct IntuiText m_ProgressValueIText;
  const char* m_pProgressDescription;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();
};


#endif
