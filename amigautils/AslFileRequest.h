#ifndef ASL_FILE_REQUEST_H
#define ASL_FILE_REQUEST_H

#include <intuition/intuition.h>
#include "SimpleString.h"

/**
 * Provides an easy to use ASL file requester.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class AslFileRequest
{
public:
  AslFileRequest(struct Window* p_pIntuiWindow);
  virtual ~AslFileRequest();

  /**
   * Opens a file requester and let the user select a file
   *
   * @param p_Title
   * Text to be displayed in the file requesters title bar
   *
   * @returns
   * Selected filename or empty string if dialog was canceled by the 
   * user
   */
  SimpleString SelectFileName(SimpleString p_Title);

private:
  struct Window* m_pIntuiWindow;
};

#endif
