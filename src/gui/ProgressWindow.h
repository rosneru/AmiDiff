#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <exec/ports.h>

#include "Rect.h"
#include "ScreenBase.h"
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
  ProgressWindow(ScreenBase*& pScreeBase, 
                 struct MsgPort*& pIdcmpMsgPort,
                 bool& bCancelRequested,
                 AMenu* pMenu);

  virtual ~ProgressWindow();

  void Refresh();

  /**
   * Opening the window.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  bool Open(InitialPosition initialPos = WindowBase::IP_Center);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false.
   */
  virtual void HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress);

  /**
   * Handles the given progress event.
   *
   * Prints the current pDescription text of given pProgrMsg and
   * draws a progress bar representing the actual percentual value.
   */
  void HandleProgress(struct ProgressMessage* pProgressMsg);


private:
  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets
   */
  enum GadgetId
  {
    GID_BtnStop,
  };

  bool& m_bCancelRequested;

  struct NewGadget m_NewGadget;
  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadBtnStop;

  Rect m_OuterRect;
  Rect m_ProgressRect;

  SimpleString m_TextZero;
  SimpleString m_TextHundred;
  size_t m_TextZeroWidth;
  size_t m_TextHundredWidth;

  SimpleString m_ProgressDescr;

};


#endif
