#ifndef SCROLBARL_WINDOW_H
#define SCROLBARL_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "WindowBase.h"

/**
 * Class a window that contains vertical and horizontal scrollbars in
 * its border.
 *
 * @author Uwe Rosner
 * @date 10/12/2018
 */
class ScrollbarWindow : public WindowBase
{
public:
  ScrollbarWindow(AppScreen& appScreen, 
                  struct MsgPort*& pIdcmpMsgPort,
                  int& numOpenWindows);

  virtual ~ScrollbarWindow();


  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(const APTR pMenuItemDisableAtOpen = NULL,
                    InitialPosition initialPos = WindowBase::IP_Center);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  virtual bool HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress);

  /**
   * Setting a new top position to the x-scrollbar pot.
   */
  void SetXScrollLeft(int left);


  /**
   * Setting a new top position to the y-scrollbar pot.
   */
  void SetYScrollTop(int top);


  /**
   * This handles the x-changes triggered by the horizontal scrollbar
   * of the window. It is called from the handleIDCMP method of this
   * class.
   *
   * Child classes must implement it and therin should perform the
   * scrolling to the new x-position.
   */
  virtual void XChangedHandler(size_t newX) = 0;

  /**
   * This handles the y-changes triggered by the vertical scrollbar
   * of the window. It is called from the handleIDCMP method of this
   * class.
   *
   * Child classes must implement it and therin should perform the
   * scrolling to the new y-position.
   */
  virtual void YChangedHandler(size_t newY) = 0;

  /**
   * This handles the increase by 1 of the x-position triggered by the
   * cursor-right-key or the right-arrow-gadget. It is called from the
   * handleIDCMP method of this class.
   *
   * Child classes must implement it and therein should perform the
   * scrolling to x+1.
   *
   * @param numChars
   * Amount to increase the x-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  virtual void XIncrease(size_t numChars,
                         bool bTriggeredByScrollPot = false) = 0;

  /**
   * This handles the decrease by 1 of the x-position triggered by the
   * cursor-left-key or the left-arrow-gadget. It is called from the
   * handleIDCMP method of this class.
   *
   * Child classes must implement it and therein should perform the
   * scrolling to x-1.
   *
   * @param numChars
   * Amount to decrease the x-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  virtual void XDecrease(size_t numChars,
                         bool bTriggeredByScrollPot = false) = 0;

  /**
   * This handles the increase by 1 of the y-position triggered by the
   * cursor-down-key or the down-arrow-gadget. It is called from the
   * handleIDCMP method of this class.
   *
   * Child classes must implement it and therein should perform the
   * scrolling to y+1.
   *
   * @param numChars
   * Amount to increase the y-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  virtual void YIncrease(size_t numChars,
                         bool bTriggeredByScrollPot = false) = 0;

  /**
   * This handles the decrease by 1 of the y-position triggered by the
   * cursor-up-key or the up-arrow-gadget. It is called from the
   * handleIDCMP method of this class.
   *
   * Child classes must implement it and therein should perform the
   * scrolling to y-1.
   *
   * @param numChars
   * Amount to decrease the y-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  virtual void YDecrease(size_t numChars,
                         bool bTriggeredByScrollPot = false) = 0;

protected:
  long m_InnerWindowRight;  ///> X-position of the right-most pixel before the scrollbar
  long m_InnerWindowBottom; ///> Y-position of the bottom-most pixel before the scrollbar

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  virtual void initialize();

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  virtual void calcSizes();

  /**
   * Setting the pot size of the x-scrollbar.
   *
   * @param maxVisibleChars
   * Number of max visible chars / points / etc in window. Has to be
   * updated e.g. after the window resizes.
   *
   * @param totalChars
   * Number of total visible chars / points / etc in document.
   */
  void setXScrollPotSize(int maxVisibleChars, int totalChars);

  /**
   * Setting the pot size of the y-scrollbar.
   *
   * @param maxVisibleLines
   * Number of max visible lines / points / etc in window. Has to be
   * updated e.g. after the window resizes.
   *
   * @param totalLines
   * Number of total visible lines / points / etc in document.
   */
  void setYScrollPotSize(int maxVisibleLines, int totalLines);

private:
  /**
   * IDs to help to interpret the events of this window's BOOPSI system
   * gadgets.
   */
  enum GadgetId
  {
    GID_PropX,
    GID_PropY,
    GID_ArrowUp,
    GID_ArrowDown,
    GID_ArrowLeft,
    GID_ArrowRight,
  };

  ULONG m_SizeImageWidth;
  ULONG m_SizeImageHeight;

  struct Image* m_pLeftArrowImage;    ///> h-scrollbar left button image
  struct Image* m_pRightArrowImage;   ///> h-scrollbar right button image
  struct Image* m_pUpArrowImage;      ///> v-scrollbar up button image
  struct Image* m_pDownArrowImage;    ///> v-scrollbar down button image

  struct Gadget* m_pXPropGadget;      ///> horizontal scrollbar
  struct Gadget* m_pYPropGadget;      ///> vertical scrollbar
  struct Gadget* m_pLeftArrowButton;  ///> h-scrollbar left button
  struct Gadget* m_pRightArrowButton; ///> h-scrollbar right button
  struct Gadget* m_pUpArrowButton;    ///> v-scrollbar up button
  struct Gadget* m_pDownArrowButton;  ///> v-scrollbar down button

  /**
   * Browses through all IDCMP_IDCMPUPDATE messages for the given
   * proportional gadget (BOOPSI) which are addressed to this window.
   * It deletes each of this messages by replying it to intuition.
   * Finally the given value argument variable is set to the last
   * found top value.
   *
   * This should be used on proportional gadgets to get only the latest
   * top value and delete all previous messages of this kind. Usefull
   * on slow systems to not get flooded by messages for this gadget.
   *
   * @param gadgetId
   * Id of the BOOPSI gadget to browse for messages
   *
   * @param lastVal
   * If one or more IDCMP_IDCMPUPDATE messages are found while browsing,
   * the tag value of the latest found message is written to this given
   * argument. If no appropriate message is found the argument variable
   * will not be overwritten.
   */
  void extractLatestPropGadTopValue(GadgetId gadgetId, size_t& lastVal);
};


#endif
