#ifndef FILES_WINDOW_H
#define FILES_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>
#include <workbench/workbench.h>

#include <Array.h>
#include "ScreenBase.h"
#include "CommandBase.h"
#include "CmdFileRequester.h"
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
  FilesWindow(Array<WindowBase*>& windowArray,
              ScreenBase* pScreenBase,
              struct MsgPort*& pIdcmpMsgPort,
              SimpleString& leftFilePath,
              SimpleString& rightFilePath,
              CommandBase& cmdDiff,
              CommandBase& cmdCloseFilesWindow,
              AMenu* pMenu);

  virtual ~FilesWindow();

  void Refresh();

  /**
   * Opening the window.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  bool Open(InitialPosition initialPos = WindowBase::IP_Center);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  void HandleIdcmp(ULONG msgClass, UWORD msgCode, APTR pItemAddress);

  /**
   * Handles the given progress event.
   *
   * Prints the current pDescription text of given pProgrMsg and
   * draws a progress bar representing the actual percentual value.
   */
  void HandleAppMessage(struct AppMessage* pAppMsg);

private:
  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;

  CommandBase& m_CmdDiff;
  CommandBase& m_CmdCloseFilesWindow;

  CmdFileRequester m_CmdSelectLeftFile;
  CmdFileRequester m_CmdSelectRightFile;

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

  //
  // These next four methods are the main functions of this window
  // which are mapped to the butons and keys.
  //
  void selectLeftFile();
  void selectRightFile();
  void swapFiles();
  void compare();


  /**
   * Enables or disables the 'Diff' and 'Swap' buttons depending on
   * some conditions.
   */
  void checkEnableButtons();


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
