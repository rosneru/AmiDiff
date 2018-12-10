#ifndef OPEN_FILES_WINDOW_H
#define OPEN_FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AmigaDiffFacade.h"
#include "AppScreen.h"
#include "Command.h"
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
class OpenFilesWindow : public WindowBase
{
public:
  OpenFilesWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
    AmigaDiffFacade& p_DiffFacade);
  virtual ~OpenFilesWindow();

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
  bool Open(APTR p_pMenuItemDisableAtOpen);

private:
  bool m_bFileRequestOpen;

  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets in the Application event loop.
   */
  enum GadgetId
  {
    GID_LeftFileString,
    GID_RightFileString,
    GID_LeftFileButton,
    GID_RightFileButton,
    GID_DiffButton,
    GID_CancelButton,
  };

  AmigaDiffFacade& m_DiffFacade;


  WORD m_FontHeight;  ///> Height of current text font

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pLeftFileStringGadget;
  struct Gadget* m_pRightFileStringGadget;
  struct Gadget* m_pSelectLeftFileButton;
  struct Gadget* m_pSelectRightFileButton;
  struct Gadget* m_pDiffButton;
  struct Gadget* m_pCancelButton;

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();

  /**
   * Handles given IDCMP event.
   * 
   * @returns If this event was handled: true; else: false.
   */
  bool handleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  /**
   * Opens a file requestor and lets the user select a file.
   *
   * @param p_FilePath
   * Reference to a string variable to set the text of the user
   * selected path/file into. If the user cancels the file requestor
   * this variable is not changed.
   *
   * @param p_RequestTitle
   * Title to set in the filer requestor window
   *
   * @returns
   * When user selected a file: true; when user cancelled the
   * requestor: false
   */
  bool selectFile(SimpleString& p_FilePath,
    const SimpleString& p_RequestTitle);

  /**
   * Enables all gadgets in the window except the 'Diff' button which
   * only is enabled when both string gadgets contain text.
   */
  void enableAll();

  /**
   * Disables all gadgets in the window.
   *
   * Should be called e.g. before an ASL reuest is opened to prevent
   * from input during request is open.
   */
  void disableAll();

  /**
   * Enables or disables the buttons of this window in dependency of
   * some conditions, e.g. is text in both string gadgets or is there
   * an ASL request openened etc.
   */
  void setDiffButtonState();

  void setStringGadgetText(struct Gadget* p_pGadget,
    const SimpleString& p_Text);

  /**
   * Reads the text from both string gadgets and updates the
   * LeftFilePath and RightFilePath in m_DiffFacade accordingly.
   *
   * Also sets the 'Diff' button state to enabled or disabled
   * depending if both string gadgets contain text.
   */
  void readStringGadgetsText();
};


#endif
