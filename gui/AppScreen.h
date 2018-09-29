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

#include "SimpleString.h"

class AppScreen
{
public:
  AppScreen(SimpleString p_Title);
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

  const char* Title();

  /**
   * Gets the intuition screen structure or NULL if screen is not open
   */
  struct Screen* IntuiScreen();

  /**
   * Gets the intuition screen draw info or NULL if it hadnt been
   * akquired successfully
   */
  struct DrawInfo* IntuiDrawInfo();

private:
  SimpleString m_Title;
  struct Screen* m_pScreen;
  struct DrawInfo* m_pDrawInfo;

};


#endif
