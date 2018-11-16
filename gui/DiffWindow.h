#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "DiffDocument.h"
#include "TextWindow.h"

/**
 * Class for a diff window object. Can be created multiple times.
 *
 * @author Uwe Rosner
 * @date 16/11/2018
 */
class DiffWindow : public TextWindow
{
public:
  DiffWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort);
  virtual ~DiffWindow();


  /**
   * Reorganizes the window including re-calculating the scrollbars.
   * If needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  void Resized();

  void Refresh();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(APTR p_pMenuItemDisableAtOpen = NULL);


  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool SetContent(DiffDocument* p_pLeftDiffDocument,
    DiffDocument* p_pRightDiffDocument);

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
  DiffDocument* m_pLeftDiffDocument;
  DiffDocument* m_pRightDiffDocument;

  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcMaxWindowTextLines();

  /**
   * Displays the given line at given y-position
   */
  void displayLine(const SimpleString* p_pLine, WORD p_TopEdge);

  /**
   * Displays the complete file from current m_Y position as first line
   */
  void displayFile();

  bool scrollDownOneLine();
  bool scrollUpOneLine();
};


#endif
