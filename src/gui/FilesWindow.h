#ifndef FILES_WINDOW_H
#define FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AslFileRequest.h"
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
class FilesWindow : public WindowBase
{
public:
  FilesWindow(AppScreen& appScreen,
              struct MsgPort* pMsgPort,
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
  bool Open(const APTR pMenuItemDisableAtOpen);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  bool HandleIdcmp(ULONG msgClass, UWORD msgCode, APTR pItemAddress);

private:
  AslFileRequest m_AslRequest;
  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;
  const Command& m_CmdDiff;

  bool m_bFileRequestOpen;

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

  struct Gadget* m_pGadgetsHeader;
  struct Gadget* m_pGadStrLeftFile;
  struct Gadget* m_pGadStrRightFile;
  struct Gadget* m_pGadBtnSelectLeft;
  struct Gadget* m_pGadBtnSelectRight;
  struct Gadget* m_pGadBtnDiff;
  struct Gadget* m_pGadBtnSwap;
  struct Gadget* m_pGadBtnCancel;

  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  void initialize();


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
   * Enables or disables the buttons of this window in dependency of
   * some conditions, e.g. is text in both string gadgets or is there
   * an ASL file request openened etc.
   */
  void setDiffButtonState();

  /**
   * Sets the text of given string gadget to given value
   */
  void setStringGadgetText(struct Gadget* pGadget,
                           const SimpleString& text);

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
