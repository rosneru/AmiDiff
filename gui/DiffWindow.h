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

  /**
   * Re-drawing the obscured window areas.
   *
   * This should be called from the application if the signal
   * IDCMP_REFRESHWINDOW for this window is received.
   */
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
   * Extended method from base class Document
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool SetContent(DiffDocument* p_pLeftDocument,
    DiffDocument* p_pRightDocument);

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
  DiffDocument* m_pLeftDocument;
  DiffDocument* m_pRightDocument;

  WORD m_IndentX;           ///> Distance of TextArea to left and right window border
  WORD m_IndentY;           ///> Distance of TextArea to window title bar and bottom border

  WORD m_TextArea1Left;     ///> X-position of the 1st text area
  WORD m_TextArea2Left;     ///> X-position of the 2nd text area
  WORD m_TextAreaTop;       ///> Y-position of the text areas
  WORD m_TextAreasWidth;    ///> Width of each text area
  WORD m_TextAreasHeight;    ///> Height of the text areas (equally heigh)

  WORD m_InnerWindowRight;  ///> X-posiiton of the right-most pixel before the scrollbar
  WORD m_InnerWindowBottom; ///> X-posiiton of the bottom-most pixel before the scrollbar

  WORD m_LineNumbersWidth;

  /**
   * Initializes some window specific features. Gadgets, etc.
   */
  void initialize();


  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

  /**
   * Displays the left and right diff file inside the main text area.
   * The file is displayed starting from current text position m_Y as
   * first line at the very top of the text area.
   */
  void paintFile();

  /**
   * Displays the currently selected line at given y-position. The line
   * is composed from the left diff document and right diff documents
   * currently selected lines.
   */
  void paintLine(const SimpleString* p_pLeftLine,
    const SimpleString* p_pRightLine, WORD p_TopEdge);

  /**
   * Paints the basic window content: the 2 BevelBoxes for the original
   * (left) and changed (right) diff file, the Document names and the
   * state bar at the bottom.
   */
  void paintWindowDecoration();

  /**
   * Displays the names of the documents above the two BevelBoxes
   */
  void paintDocumentNames();

  /**
   * Gets an appropriate pen from a given ColorName
   */
  LONG colorNameToPen(DiffDocument::ColorName p_pColorName);

  bool scrollDownOneLine();
  bool scrollUpOneLine();
};


#endif
