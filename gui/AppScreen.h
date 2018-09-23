/* AppScreen.h                                                       */
/*-------------------------------------------------------------------*/
/* Class for the applications main screen                            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPSCREEN
#define APPSCREEN

#include <intuition/screens.h>

class AppScreen
{
public:
  AppScreen();
  ~AppScreen();

  /**
   * Opens a Workbench clone screen
   *
   * @returns
   * false if opning fails
   */
  bool Open();

  /**
   * Closes the screen
   */
  void Close();

  /**
   * Gets the intuition screen structure or NULL if screen is not open
   */
  struct Screen* Screen();

private:
  struct Screen* m_pScreen;
};


#endif
