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

/**
 * IDs to allow to interprete the events of this window's boopsi system
 * gadgets in the Application event loop.
 */
#define GID_RARROW 911
#define GID_LARROW 912
#define GID_UARROW 913
#define GID_DARROW 914
#define GID_XPROP  915
#define GID_YPROP  916


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
   * Moves the text **tbd with intelligence** upward or downward to the
   * new y-Position. Should be called from Application event loop if
   * the Y-proportional gadget attached to the window has been moved by
   * the user.
   */
  void YChangedHandler(size_t p_NewY);
  void YIncrease();
  void YDecrease();

private:
  AppScreen* m_pAppScreen;
  struct Window* m_pWindow;

  struct Gadget* m_pWinPropGadgetY;

  SimpleString m_Title;
  SimpleString m_FileRequesterTitle;
  size_t m_MaxWindowTextLines;  ///> Number of text lines that would fit in window
  size_t m_Y;         ///> Index of currently first displayed text line

  WORD m_FontHeight;  ///> Height of current text font
  WORD m_ScrollXMin;  ///> Left x coordinate of scrolling area
  WORD m_ScrollYMin;  ///> Left y coordinate of scrolling area
  WORD m_ScrollXMax;  ///> Right x coordinate of scrolling area
  WORD m_ScrollYMax;  ///> Right y coordinate of scrolling area

  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcMaxWindowTextLines();
  SimpleString aslRequestFileName();
  void displayFile();
  void DiffWindow::displayLine(SimpleString* p_pLine, WORD p_TopEdge);

  bool scrollDownOneLine();
  bool scrollUpOneLine();

};


#endif
