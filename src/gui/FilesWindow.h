#ifndef FILES_WINDOW_H
#define FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>
#include <workbench/workbench.h>

#include "AslFileRequest.h"
#include "AScreen.h"
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
class FilesWindow : public WindowBase
{
public:
  FilesWindow(AScreen& appScreen,
              struct MsgPort*& pIdcmpMsgPort,
              int& numWindowsOpen,
              SimpleString& leftFilePath,
              SimpleString& rightFilePath,
              const Command& cmdDiff);

  virtual ~FilesWindow();

  void Refresh();

  /**
   * Opening the window.
   *
   * @param pMenuItemDisableAtOpen
   * A pointer to an user data field of a menu item which is associated
   * with this window. If the menu item is found by the given user data
   * it will be disabled at window opening time and enabled when the
   * window is closed. Provide NULL if no menu item should be disabled.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  bool Open(const APTR pMenuItemDisableAtOpen = NULL,
            InitialPosition initialPos = WindowBase::IP_Center);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  bool HandleIdcmp(ULONG msgClass, UWORD msgCode, APTR pItemAddress);

  /**
   * Handles the given progress event.
   *
   * Prints the current pDescription text of given pProgrMsg and
   * draws a progress bar representing the actual percentual value.
   */
  void HandleAppMessage(struct AppMessage* pAppMsg);

private:
  AslFileRequest m_AslRequest;
  bool m_bFileRequestOpen;

  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;

  const Command& m_CmdDiff;

  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets.
   */
  enum GadgetId
  {
    GID_LeftFileString,
    GID_RightFileString,
    GID_LeftFileButton,
    GID_RightFileButton,
    GID_DiffButton,
    GID_SwapButton,
    GID_CancelButton,
  };

  WORD m_FontHeight;  ///> Height of current text font

  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadStrLeftFile;
  struct Gadget* m_pGadStrRightFile;
  struct Gadget* m_pGadBtnSelectLeft;
  struct Gadget* m_pGadBtnSelectRight;
  struct Gadget* m_pGadBtnDiff;
  struct Gadget* m_pGadBtnSwap;
  struct Gadget* m_pGadBtnCancel;

  //
  // The next two are called from HandleIDCMP() to get that method not
  // overblown.
  //
  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);


  /**
   * Initializes the window specific features. Gadgets, etc.
   */
  void initialize();

  //
  // These next four methods are the main functions of this window
  // which are mapped to the butons and keys.
  //
  void selectLeftFile();
  void selectRightFile();
  void swapFiles();
  void compare();

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
   * Enables or disables the 'Diff' and 'Swap' buttons depending on
   * some conditions.
   */
  void enableIfPossible();


  /**
   * Sets the text of given string gadget to given value
   */
  void setStringGadgetText(struct Gadget* pGadget,
                           const char* pText);

  /**
   * Returns the text from given string gadget. This can also be an
   * empty string.
   *
   * Returns NULL if an internal error occurs.
   */
  STRPTR getStringGadgetText(struct Gadget* pGadget);

  /**
   * Returns the first string gadget of
   *   {m_pGadStrLeftFile, m_pGadStrRightFile}
   * which contains currently no text.
   *
   * Returns NULL if both string gadgets contain text
   */
  struct Gadget* getFirstEmptyStringGadget();
};


#endif
