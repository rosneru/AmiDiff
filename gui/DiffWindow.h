/* DiffWindow.h                                                      */
/*-------------------------------------------------------------------*/
/* Class for the two application diff windows (left and right)       */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef DIFFWINDOW
#define DIFFWINDOW

#include <intuition/screens.h>
#include "DiffDocument.h"
#include "SimpleString.h"

class DiffWindow : public DiffDocument
{
public:
  DiffWindow(const struct Screen* p_pScreen);
  virtual ~DiffWindow();

  /**
   * Used to define if this window is the left or the right diff window
   */
  enum DW_TYPE
  {
    LEFT,
    RIGHT,
  };

  /**
   * Opens the window.
   *
   * @param dwType
   * Regarding to this type the window will be opened on the left or on
   * the right side of the screen
   *
   * @returns
   * false if opning fails
   */
  bool Open(DW_TYPE p_DwType);

  /**
   * Closes the window
   */
  void Close();

  /**
   * Gets the intuition window structure or NULL if window is not open
   */
  struct Window* IntuiWindow();

  const char* Title();
  void SetTitle(SimpleString p_NewTitle);

  /**
   * Open a text file
   *
   * Implemented abstract method from interface DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  virtual bool Open(SimpleString p_FileName = "");

private:
  const struct Screen* m_pScreen;
  struct Window* m_pWindow;
  SimpleString m_Title;

  SimpleString aslRequestFileName();
};


#endif
