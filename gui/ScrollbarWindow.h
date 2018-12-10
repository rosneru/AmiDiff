#ifndef SCROLBARL_WINDOW_H
#define SCROLBARL_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "TextDocument.h"
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
  ScrollbarWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort);
  virtual ~ScrollbarWindow();


  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(APTR p_pMenuItemDisableAtOpen = NULL);



  /**
   * This handles the Y-Changes triggered by the vertical scrollbar
   * of the window. It does so by completely re-drawing the text from
   * beginning with p_NewY on the top line.
   *
   * This method should be called from Application event loop if the
   * Y-proportional gadget attached to the window has been moved by
   * the user.
   */
  virtual void YChangedHandler(size_t p_NewY) = 0;

  /**
   * Increases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor down key was received.
   */
  virtual void YIncrease() = 0;

  /**
   * Decreases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor up key was received.
   */
  virtual void YDecrease() = 0;


protected:
  /**
   * IDs to help to interpret the events of this window's BOOPSI system
   * gadgets in the Application event loop.
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

  enum LastDocumentScrollDirection
  {
    None,
    NextLine,
    PreviousLine,
  };

  size_t m_MaxTextLines;  ///> Number of text lines that fit in window
  size_t m_Y;             ///> Index of currently first displayed text line

  /**
   * Stores if the last scroll direction was upward or downward. Allows
   * the scroll methods scrollDownOneLine() and scrollUpOneLine() to
   * use GetPrevious() and GetNext() instead of GetIndexed() if the
   * same scroll direction is used again.
   *
   * Possibly makes scrolling by cursor keys faster at the bottom of
   * big files.
   */
  LastDocumentScrollDirection m_LastDocumentScrollDirection;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  virtual void initialize();

  /**
   * Handles given IDCMP event.
   * 
   * @returns If this event was handled: true; else: false.
   */
  virtual bool handleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  virtual bool scrollDownOneLine();
  virtual bool scrollUpOneLine();

private:
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
};


#endif
