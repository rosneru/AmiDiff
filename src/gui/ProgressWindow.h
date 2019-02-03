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
   * @returns If this event was handled: true; else: false.
   */
  bool HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  void HandleProgress(struct WorkerProgressMsg* p_pProgressMsg);


private:
  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets
   */
  enum GadgetId
  {
    GID_Description,
    GID_Progress,
    GID_BtnCancel,
  };

  WORD m_FontHeight;  ///> Height of current text font

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pDescriptionGadget;
  struct Gadget* m_pProgressGadget;
  struct Gadget* m_pBtnCancel;

  const char* m_pProgressDescription;
  bool& m_bCancelRequested;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();
};


#endif
