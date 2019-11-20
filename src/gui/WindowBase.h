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
    IP_Center,  ///> Center of the screen; depending on window dimensions
    IP_Fill,    ///> Fit to screen below title bar; changes window dimesions
    IP_Left,    ///> Left half of the screen; changes window dimesions
    IP_Right,   ///> Right half of the screen; changes window dimesions
    IP_Explicit,///> The position is explicitly given by the user with
                ///> the four last parameters of SetInitialPosition
  };

  /**
   * Base class method for opening the window. Derived classes should
   * call this in their Open() method after the window has opened.
   *
   * Sets the menu strip to the window if one had been provided. If the
   * given pointer to a menu items user data is not null, this menu
   * item will be disabled.
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
  virtual bool Open(const APTR p_pMenuItemDisableAtOpen) = 0;

  /**
   * Closes the window.
   */
  void Close();

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
   * Sets the initial position of the window. Only is applied when
   * called before opening the window.
   *
   * @param p_InitialPosition
   * One of the positions defined in @see InitialPosition
   *
   * @param p_Left
   * Left edge of the Window. Only used when InitialPosition is set to
   * IP_Explicit.
   *
   * @param p_Top
   * Top edge of the Window. Only used when InitialPosition is set to
   * IP_Explicit.
   *
   * @param p_Width
   * Width of the Window. Only used when InitialPosition is set to
   * IP_Explicit.
   *
   * @param p_Height
   * Height of the Window. Only used when InitialPosition is set to
   * IP_Explicit.
   *
   */
  void SetInitialPosition(InitialPosition p_InitialPosition,
    long p_Left = 0, long p_Top = 0, long p_Width = 0,
    long p_Height = 0);

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
  void ChangeWindowBox(long p_Left, long p_Top, long p_Width,
    long p_Height);

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
  long m_WinLeft;
  long m_WinTop;
  long m_WinWidth;
  long m_WinHeight;
  bool m_bInitialized;
  ApplicationMenu* m_pMenu;
  SimpleString m_Title;

  /**
   * Creates a new window object
   *
   * @param p_pAppScreen
   * Screen on which the window will occur at opening time
   *
   * @param p_pMsgPort
   * Message port which is used for this window. Can be shared
   * with other windows.
   *
   * @param p_NumWindowsOpen
   * Reference to a counter variable. Will be increased at each
   * successful opening and decreased at each closing of a window.
   *
   */
  WindowBase(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
             int& p_NumWindowsOpen);

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
   * multiple times. the given p_Flags are then added by performing an
   * OR-operation.
   */
  virtual void setFlags(ULONG p_Flags);

  /**
   * Setting the window IDCMP flags. Should be done before window
   * opening. Can be called multiple times. the given p_Idcmp are
   * then added by performing an OR-operation.
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window idcmp messages.
   */
  virtual void setIDCMP(ULONG p_Idcmp);

  /**
   * Setting the first gadget of a potential list of gadgets to be
   * displayed by the window. Should be done before window opening.
   *
   * Derived classes can and should set this inside their initialize()
   * implementation to set the needed window idcmp messages.
   */
  virtual void setFirstGadget(struct Gadget* p_pFirstGadget);


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

private:
  int& m_NumWindowsOpen;
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
