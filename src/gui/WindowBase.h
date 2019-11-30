#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#include <exec/ports.h>
#include <intuition/screens.h>
#include "ApplicationMenu.h"
#include "AppScreen.h"
#include "SimpleString.h"

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
   * Base class method for opening the window. Derived classes should
   * call this in their Open() method.
   *
   * Sets the menu strip to the window if one had been provided.
   *
   * If there's given a non-NULL pointer to a menu items user data,
   * this menu item will be disabled at window opening.
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
  virtual bool Open(const APTR pMenuItemDisableAtOpen,
                    InitialPosition initialPosition = IP_Center) = 0;

  /**
   * Closes the window.
   */
  void Close();


  /**
   * Returns a pointer to the menu item which was disabled at window
   * opening or NULL if there was no item disabled.
   */
  const APTR DisabledMenuItem() const;


  /**
   * Abstract method. Forces inheriting classes to handle the given
   * IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false.
   */
  virtual bool HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress) = 0;


  /**
   * Returns true if the window is opened.
   */
  bool IsOpen() const;

  const char* Title() const;
  void SetTitle(SimpleString p_NewTitle);


  /**
   * Sets the initial position and size of the window. Only applied
   * when called before opening the window.
   *
   * NOTE: Sets The initial position to IP_Explicit.
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
   * @param p_bFixWindow
   * When true the window will not be moveable/draggable after opening.
   */
  void SetFixed(bool p_bFixWindow);


  /**
   * Sets if the window is without border. Only is applied when
   * called before opening the window.
   *
   * @param p_bFixWindow
   * When true the window will have no border.
   */
  void SetBorderless(bool p_bBorderless);

  /**
   * Sets if the window is a smart refresh window. Only is applied when
   * called before opening the window.
   *
   * @param p_bFixWindow
   * When true the window will be a smart refresh window after opening.
   * When false it will be simple refresh.
   */
  void SetSmartRefresh(bool p_bSmartRefresh);


  /**
   * Moves an opened window to new position
   *
   * @param p_DX
   * Delta of window position on x-axis. Can be pos. or neg.
   *
   * @param p_DY
   * Delta of window position on y-axis. Can be pos. or neg.
   */
  void Move(long p_DX, long p_DY);


  /**
   * Resizes an opened window.
   *
   * @param p_DX
   * Delta of window size on x-axis. Can be pos. or neg.
   *
   * @param p_DY
   * Delta of window size on y-axis. Can be pos. or neg.
   */
  void Size(long p_DX, long p_DY);


  /**
   * Resizes an opened window and moves it to a new position.
   * Coordinates are absolute and no deltas.
   */
  void ChangeWindowBox(long p_Left, long p_Top, long width, long height);


  /**
   * Returns the intuition window structure or NULL if window is not
   * open.
   */
  struct Window* IntuiWindow();


  /**
   * Returns the app screen object on which the window is opened or
   * NULL if window is not open.
   */
  AppScreen& WindowScreen();

  void SetMenu(ApplicationMenu* p_pMenu);

protected:
  AppScreen& m_AppScreen;
  struct MsgPort* m_pMsgPort;
  struct Window* m_pWindow;
  bool m_bBorderless;
  bool m_bSmartRefresh;
  ULONG m_WinLeft;
  ULONG m_WinTop;
  ULONG m_WinWidth;
  ULONG m_WinHeight;
  bool m_bInitialized;
  ApplicationMenu* m_pMenu;
  SimpleString m_Title;


  /**
   * Creates a new window object
   *
   * @param p_pAppScreen
   * Screen on which the window will occur at opening time
   *
   * @param pMsgPort
   * Message port which is used for this window. Can be shared
   * with other windows.
   *
   * @param numOpenWindows
   * Reference to a counter variable. Will be increased at each
   * successful opening and decreased at each closing of a window.
   *
   */
  WindowBase(AppScreen& appScreen, struct MsgPort* pMsgPort,
             int& numOpenWindows);


  virtual ~WindowBase();


  /**
   * Gadget initialization etc should be done here. Must be
   * implemented by derived classes.
   */
  virtual void initialize() = 0;


  /**
   * Setting the window flags. Should be done before window opening.
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window flags. Can be called
   * multiple times. the given flags are then added by performing an
   * OR-operation.
   */
  virtual void setFlags(ULONG flags);


  /**
   * Setting the window IDCMP flags, oring them to the already existing
   * ones.
   *
   * Should be done before window opening.
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window idcmp messages.
   */
  virtual void setIDCMP(ULONG idcmp);


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
   * Also sets the given variable to the number of gadgets which
   * are already added to the window.
   */
  struct Gadget* getLastGadget(int& refNumGadgets);


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
  int& m_NumOpenWindows;
  bool m_bIsFixed;
  InitialPosition m_InitialPosition;
  ULONG m_WindowFlags;
  ULONG m_WindowIdcmp;
  struct Gadget* m_pFirstGadget;
  APTR m_pMenuItemDisableAtOpen;

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
