#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <string>

#include <exec/ports.h>
#include <intuition/screens.h>

#include "ADiffViewPens.h"
#include "DiffDocument.h"
#include "DiffLine.h"
#include "DiffWindowRastports.h"
#include "DiffWindowTextArea.h"
#include "OpenScreenBase.h"
#include "ScrollbarWindow.h"

/**
 * Class for a diff window object. Can be created multiple times.
 *
 * @author Uwe Rosner
 * @date 16/11/2018
 */
class DiffWindow : public ScrollbarWindow
{
public:
  DiffWindow(ScreenBase& screen,
             const ADiffViewPens& pens,
             struct MsgPort* pIdcmpMsgPort,
             MenuBase* pMenu);

  virtual ~DiffWindow();

  /**
   * Reorganizes the window including re-calculating the scrollbars. If
   * needed also re-drawing the obscured text areas.
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
  virtual bool Open(InitialPosition initialPos = WindowBase::IP_Center);


  /**
   * Set the diff document to be displayed in DiffWindow.
   */
  bool SetContent(DiffDocument* pDiffDocument);


  /**
   * Navigate to the next difference.
   */
  void NavigateToNextDiff();

  /**
   * Navigate to the previous difference.
   */
  void NavigateToPrevDiff();

  /**
   * Return the left text area
   */
  DiffWindowTextArea* getLeftTextArea() const;

  /**
   * Return the right text area
   */
  DiffWindowTextArea* getRightTextArea() const;


  /**
   * Extends method of base class ScrollbarWindow.
   */
  virtual void HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress);


  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleChangeOfTopPos(size_t newX);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleChangeOfLeftPos(size_t newY);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleXIncrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleXDecrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleYIncrease(size_t numLines, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void handleYDecrease(size_t numLines, bool bTriggeredByScrollPot);

private:
  const ADiffViewPens& m_Pens;

  DiffWindowRastports* m_pRPorts;
  DiffDocument* m_pDocument;
  char m_EmptyChar;

  struct Gadget* m_pLastParentGadget;
  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadTxtLeftFile;
  struct Gadget* m_pGadTxtRightFile;

  long m_NumDifferences;      ///> Number of differences

  ULONG m_IndentX;  ///> X-distance of the text areas to window borders
  ULONG m_IndentY;  ///> Y-distance of the text areas to window borders
  ULONG m_TextAreasWidth;
  ULONG m_TextAreasHeight;

  DiffWindowTextArea* m_pLeftTextArea;
  DiffWindowTextArea* m_pRightTextArea;

  char m_ChangedText[20];
  char m_AddedText[20];
  char m_DeletedText[20];
  char m_StatusBarText[60];

  struct TextAttr m_TextAttr;

  //
  // The next two are called from HandleIDCMP() to get that method not
  // overblown.
  //
  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);

  void cleanup();

  bool createGadgets();

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

  /**
   * Resize the gadgets. Should be called after the window size has
   * changed. As gadget-resizing or re-positioning is not supported with
   * Gadtools, the gadgets are re-created completely.
   */
  void resizeGadgets();


  /**
   * Paint the basic window content: the 2 BevelBoxes for the original
   * (left) and changed (right) diff file, the Document names and the
   * state bar at the bottom.
   */
  void paintWindowDecoration();


  /**
   * Display a status bar with statistical information and a legend
   * below the two text areas
   */
  void paintStatusBar();
};


#endif
