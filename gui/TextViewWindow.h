#ifndef TEXT_VIEW_WINDOW_H
#define TEXT_VIEW_WINDOW_H

#include <intuition/screens.h>
#include "AppScreen.h"
//#include "DiffDocument.h" // TODO BaseWindow
#include "SimpleString.h"

/**
 * Class for a text window object. Can be created multiple times, e.g.
 * twice, to have a left and a right diff window.
 *
 * It extends DiffDocument so it inherits all of DiffDocuments public
 * methods.
 *
 * In addition to this it now *is* an DiffDocument which means you can
 * override the ReadFile() method of DiffDocument. If done so and when
 * an open command occurs on a DiffDocument the ReadFile() of this
 * window object will be called first.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class TextViewWindow // : public DiffDocument // TODO BaseWindow
{
public:
  TextViewWindow(AppScreen* p_pAppScreen);
  virtual ~TextViewWindow();

  /**
   * Used to define if this window is the left or the right diff window
   */
  enum DW_TYPE
  {
    LEFT,
    RIGHT,
  };

  /**
   * IDs to allow to interpret the events of this window's BOOPSI system
   * gadgets in the Application event loop.
   */
  enum DW_GADGET_ID
  {
    DGID_XPROP,
    DGID_YPROP,
    DGID_UPARROW,
    DGID_DOWNARROW,
    DGID_LEFTARROW,
    DGID_RIGHTARROW,
  };

  /**
   * Reorganizes the window including re-calculating scrollbars. If
   * needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
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
   * false if opening fails
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
   * Displays the complete file from current m_Y position as first line
   */
  void displayFile();

  /**
   * Displays the given line at given y-position
   */
  void displayLine(SimpleString* p_pLine, WORD p_TopEdge);

  bool scrollDownOneLine();
  bool scrollUpOneLine();

  /**
   * Creates an BOOPSI image object of one of the system shipped with
   * the operating system from OS2.04 onward.
   *
   * @param p_SysImageId
   * Id of the system image
   *
   * @see intuition/imageclass.h
   *
   * @param p_Width
   * If image creation is successful the width of the image will be
   * stored here.
   *
   * @param p_Height
   * If image creation is successful the height of the image will be
   * stored here.
   *
   * @returns
   * A pointer to the image struct if successful or NULL if failed.
   */
  struct Image* createImageObj(ULONG p_SysImageId, ULONG& p_Width, ULONG& p_Height);
};


#endif
