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

private:
  DiffDocument* m_pLeftDocument;
  DiffDocument* m_pRightDocument;

  WORD m_IndentX;           ///> Distance of TextArea to left and right window border
  WORD m_IndentY;           ///> Distance of TextArea to window title bar and bottom border

  WORD m_TextArea1Left;     ///> X-position of the 1st text area
  WORD m_TextArea2Left;     ///> X-position of the 2nd text area
  WORD m_TextAreasTop;      ///> Y-position of the text areas
  WORD m_TextAreasWidth;    ///> Width of each text area
  WORD m_TextAreasHeight;   ///> Height of the text areas (equally heigh)

  WORD m_InnerWindowRight;  ///> X-position of the right-most pixel before the scrollbar
  WORD m_InnerWindowBottom; ///> Y-position of the bottom-most pixel before the scrollbar

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
   *
   * @param p_bStartFromCurrentY
   * When true: no GetIndexed() is done to left and right document to
   * find the start position. Instead it is assumed that the right y-
   * position is set already ang acn be get with GetCurrent for left
   * and right document.
   */
  void paintDocument(bool p_bStartFromCurrentY = false);

  /**
   * Prints the given line (p_pLeftLine and p_pRightLine) at given
   * y-position p_TopEdge.
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

  /**
   * Scrolls the current text in the text area down by p_pNumLinesDown
   * lines and fills the gap at top with the previous lines
   */
  void scrollNLinesDown(int p_pNumLinesDown);

  /**
   * Prints the given number of next (to the current last line in
   * text area) lines at bottom
   */
  void scrollNLinesUp(int p_pNumLinesUp);

  /**
   * Gets the line (left and right) which at current text position m_Y
   * is the previous to the first line in window
   *
   * @param p_pLeftLine
   * Reference to a pointer to SimpleString. After success this points
   * to the previous line. Will be NULL if no previous left line exists.
   *
   * @param p_pRightLine
   * Reference to a pointer to SimpleString. After success this points
   * to the previous right line. Will be NULL if no previous right line exists.
   */
  void getPreviousLineAtTop(const SimpleString*& p_pLeftLine,
    const SimpleString*& p_pRightLine);

  /**
   * Gets the line (left and right) which at current text position m_Y
   * is the following to the last line in window
   *
   * @param p_pLeftLine
   * Reference to a pointer to SimpleString. After success this points
   * to the next left line. Will be NULL if no next left line exists.
   *
   * @param p_pRightLine
   * Reference to a pointer to SimpleString. After success this points
   * to the next right line. Will be NULL if no next right line exists.
   */
  void getNextLineAtBottom(const SimpleString*& p_pLeftLine,
    const SimpleString*& p_pRightLine);

};


#endif
