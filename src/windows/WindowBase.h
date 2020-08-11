#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#include <string>

#include <exec/ports.h>
#include <intuition/screens.h>
#include <workbench/workbench.h>

#include "MenuBase.h"
#include "OpenScreenBase.h"

/**
 * Abstract base class for all other windows
 *
 *
 * @author Uwe Rosner
 * @date 19/10/2018
 */
class WindowBase
{
public:
  /**
   * Used to define an initial position of the window on the screen
   * without messing around with too many parameters.
   */
  enum InitialPosition
  {
    IP_Center,  ///> Default. Centered on screen. Depends on window dimensions
    IP_Fill,    ///> Fit to screen below title bar; changes window dimesions
    IP_Left,    ///> Left half of the screen; changes window dimesions
    IP_Right,   ///> Right half of the screen; changes window dimesions
    IP_Explicit,///> The position is explicitly given by the user with
                ///> the four last parameters of SetInitialPosition
  };

  /**
   * IMPORTENT: Resizable childs should override this and re-paint their
   * contents according the new size.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  virtual void Resized();

  /**
   * Base class method for opening the window. Derived classes should
   * call this in their Open() method.
   *
   * Sets the menu strip to the window if one had been provided.
   *
   * If there's given a non-NULL pointer to a menu items user data,
   * this menu item will be disabled at window opening.
   *
   * @param pUserDataMenuItemToDisable
   * A pointer to an user data field of a menu item which is associated
   * with this window. If the menu item is found by the given user data
   * it will be disabled at window opening time and enabled when the
   * window is closed. Provide NULL if no menu item should be disabled.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  virtual bool Open(InitialPosition initialPos = IP_Center);

  /**
   * Closes the window.
   */
  void Close();

  /**
   * Abstract method. Forces inheriting classes to handle the given
   * IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false.
   */
  virtual void HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress) = 0;


  /**
   * Returns true if the window is opened.
   */
  bool IsOpen() const;

  /**
   * Returns the window title
   */
  const char* Title() const;

  /**
   * Sets the window title
   * NOTE: Works only before opening the window
   */
  void SetTitle(std::string newTitle);


  /**
   * Sets the initial position and size of the window. Only applied
   * when called before opening the window.
   * 
   * NOTE #1: Works only before opening the window
   * NOTE #2: This also sets The initial position to IP_Explicit
   *
   * @param left
   * Left edge of the Window.
   *
   * @param top
   * Top edge of the Window.
   *
   * @param width
   * Width of the Window.
   *
   * @param height
   * Height of the Window.
   */
  void SetInitialDimension(ULONG left,
                           ULONG top,
                           ULONG width,
                           ULONG height);


  /**
   * Sets if the window appears fixed or if it is draggable with the
   * mouse. Only is applied when called before opening the window.
   *
   * @param bFixWindow
   * When true the window will not be moveable/draggable after opening.
   */
  void SetFixed(bool bFixWindow);


  /**
   * Sets if the window is without border. Only is applied when
   * called before opening the window
   *
   * @param bFixWindow
   * When true the window will have no border.
   */
  void SetBorderless(bool bBorderless);


  /**
   * The window is prepared to be a Workbench AppWindow using given 
   * port and id.
   * 
   * NOTE: Works only before opening the window
   */
  void EnableAppWindow(struct MsgPort* pAppWindowPort, 
                       ULONG appWindowId);

  /**
   * Returns the intuition window structure or NULL if window is not
   * open.
   */
  struct Window* IntuiWindow();


  /**
   * Set the menu strip for the window.
   * 
   * If the window open, the old menu strip will be replaced by the 
   * given one immediately.
   * 
   * If the window isn't open the given menu strip will be there after
   * opening.
   */
  void SetMenu(MenuBase* pMenu);

  /**
   * Returns the menu strip which is attached to this window
   */
  MenuBase* Menu();

protected:
  ScreenBase& m_Screen;
  struct MsgPort* m_pIdcmpMsgPort;
  struct Window* m_pWindow;
  bool m_bBorderless;

  LONG m_Left;
  LONG m_Top;
  LONG m_Width;
  LONG m_Height;

  MenuBase* m_pMenu;
  struct TextFont* m_pTextFont;
  std::string m_Title;


  /**
   * Creates a new window object
   *
   * @param pScreenBase
   * Screen on which the window will occur at opening time
   *
   * @param pIdcmpMsgPort
   * Message port which is used for this window. Can be shared
   * with other windows.
   *
   * @param numOpenWindows
   * Reference to a counter variable. Will be increased at each
   * successful opening and decreased at each closing of a window.
   *
   */
  WindowBase(ScreenBase& screen, 
             struct MsgPort* pIdcmpMsgPort,
             MenuBase* pMenu);


  virtual ~WindowBase();


  /**
   * *Or*ing the given window flags to the existing ones.
   *
   * Must be done before window opening, no effect after
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window flags. Can be called
   * multiple times. the given flags are then added by performing an
   * OR-operation.
   */
  virtual void addFlags(ULONG flags);


  /**
   * *Or*ing the given idcmp flags to the existing ones.
   *
   * Must be done before window opening, no effect after
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window idcmp messages.
   */
  virtual void addIDCMP(ULONG idcmp);


  /**
   * Setting the first gadget of a potential list of gadgets inside
   * the window. Should be done before window opening.
   *
   * If a first gadget already has been set it is not overwritten.
   * Instead the given gadget will be put at the end of the gadget
   * list.
   *
   * Derived classes should call this inside their implementation of
   * initialize() to enhance the gadget list.
   */
  virtual void setFirstGadget(struct Gadget* pFirstGadget);


  /**
   * Returns a pointer to the first gadget or NULL if none is set.
   *
   * Derived classes should call this in their implementation of
   * initialize() if they want to extend a already existing gadget
   * list.
   */
  struct Gadget* getFirstGadget();


  /**
   * Returns the last gadget in the gadget list for this window.
   */
  struct Gadget* getLastGadget();


  /**
   * Creates an BOOPSI image object of one of the system shipped with
   * the operating system from OS2.04 onward.
   *
   * @param sysImageId
   * Id of the system image
   *
   * @see intuition/imageclass.h
   *
   * @param width
   * If image creation is successful the width of the image will be
   * stored here.
   *
   * @param height
   * If image creation is successful the height of the image will be
   * stored here.
   *
   * @returns
   * A pointer to the image struct if successful or NULL if failed.
   */
  struct Image* createImageObj(ULONG sysImageId, ULONG& width, ULONG& height);

private:
  bool m_bIsFixed;
  InitialPosition m_InitialPosition;
  ULONG m_WindowFlags;
  ULONG m_WindowIdcmp;
  struct Gadget* m_pFirstGadget;

  struct MsgPort* m_pAppWindowPort;
  struct AppWindow* m_pAppWindow;
  ULONG m_AppWindowId;


  /**
   * Use this method to close any windows that share an IDCMP port
   * with another window.
   */
  void closeWindowSafely();

  /**
   * Method to remove and reply all IntuiMessages on a port that have
   * been sent to a particular window (note that we don't rely on the
   * ln_Succ pointer of a message after we have replied it)
   */
  void stripIntuiMessages();

};


#endif