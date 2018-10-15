/* DiffWindow.h                                                      */
/*-------------------------------------------------------------------*/
/* Class for the two application diff windows (left and right)       */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef DIFFWINDOW
#define DIFFWINDOW

#include <intuition/screens.h>
#include "AppScreen.h"
#include "DiffDocument.h"
#include "SimpleString.h"


class DiffWindow : public DiffDocument
{
public:
  DiffWindow(AppScreen* p_pAppScreen);
  virtual ~DiffWindow();

  /**
   * Used to define if this window is the left or the right diff window
   */
  enum DW_TYPE
  {
    LEFT,
    RIGHT,
  };

/**
 * IDs to allow to interprete the events of this window's boopsi system
 * gadgets in the Application event loop.
 */
  enum DW_SYSTEM_GADGET_ID
  {
    SYS_GADGET_YPROP,
  };

  /**
   * Reorganises the window including re-calculating scrollbars. If
   * needed also re-drawing the obscured text areas.
   *
   * This shoud be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  void Resized();

  void Refresh();

  /**
   * Opens the window.
   *
   * @param dwType
   * Regarding to this type the window will be opened on the left or on
   * the right side of the screen
   *
   * @returns
   * false if opning fails
   */
  bool Open(DW_TYPE p_DwType);

  /**
   * Closes the window
   */
  void Close();

  const char* Title();
  void SetTitle(SimpleString p_NewTitle);

  /**
   * Gets the intuition window structure or NULL if window is not open
   */
  struct Window* IntuiWindow();

  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  virtual bool ReadFile(SimpleString p_FileName = "");

  /**
   * This handles the Y-Changes triggered by the vertical scrollbar
   * of the window. It does so by completely re-drawing the text from
   * beginning with p_NewY on the top line.
   *
   * This method should be called from Application event loop if the
   * Y-proportional gadget attached to the window has been moved by
   * the user.
   */
  void YChangedHandler(size_t p_NewY);

  /**
   * Increases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor down key was received.
   */
  void YIncrease();

  /**
   * Decreases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor up key was received.
   */
  void YDecrease();

private:
  enum LAST_SCROLL_DIRECTION
  {
    None,
    Upward,
    Downward,
  };

  AppScreen* m_pAppScreen;
  struct Window* m_pWindow;

  SimpleString m_Title;
  SimpleString m_FileRequesterTitle;
  size_t m_MaxWindowTextLines;  ///> Number of text lines that fit in window
  size_t m_Y;         ///> Index of currently first displayed text line

  WORD m_FontHeight;  ///> Height of current text font
  WORD m_ScrollXMin;  ///> Left x coordinate of scrolling area
  WORD m_ScrollYMin;  ///> Left y coordinate of scrolling area
  WORD m_ScrollXMax;  ///> Right x coordinate of scrolling area
  WORD m_ScrollYMax;  ///> Right y coordinate of scrolling area
  
  /**
   * Stores if the last scroll direction was upward or downward. Allows 
   * the scroll methods scrollDownOneLine() and scrollUpOneLine() to
   * use GetPrevious() and GetNext() instead of GetIndexed() if the 
   * same scroll direction is used again.
   *
   * Possibly makes scrolling by cursor keys faster at the bottom of
   * big files.
   */
  LAST_SCROLL_DIRECTION m_LastScrollDirection;

  struct Gadget* m_pWinPropGadgetX;       ///> horizontal scrollbar
  struct Gadget* m_pWinPropGadgetY;       ///> vertical scrollbar
  struct Gadget* m_pWinLeftArrowGadget;   ///> h-scrollbar left button
  struct Gadget* m_pWinRightArrowGadget;  ///> h-scrollbar right button
  struct Gadget* m_pWinUpArrowGadget;     ///> v-scrollbar up button
  struct Gadget* m_pWinDownArrowGadget;   ///> v-scrollbar down button
  
  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcMaxWindowTextLines();
  
  /**
   * Opens an ASL request asking for a file name.
   *
   * @return The selected file name or an empty string
   */
  SimpleString aslRequestFileName();

  /**
   * Displays the complete file from current m_Y position as firts line
   */
  void displayFile();
  
  /**
   * Displays the given line at given y-position
   */
  void displayLine(SimpleString* p_pLine, WORD p_TopEdge);

  bool scrollDownOneLine();
  bool scrollUpOneLine();

  bool calcSysImageSize(ULONG p_SysImageId, ULONG& p_Widht, ULONG& p_Height);
};


#endif
