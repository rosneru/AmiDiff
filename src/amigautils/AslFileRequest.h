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
  AslFileRequest(struct Window* pIntuiWindow);
  virtual ~AslFileRequest();

  /**
   * Opens a file requester and let the user select a file
   *
   * @param title
   * Text to be displayed in the file requesters title bar
   *
   * @param initialFullFilePath
   * A string with a full path and file name to pre-select this file 
   * at request opening time. If empty or invalid, no file is pre-
   * selected.
   *
   * @returns
   * Full path to the selected file or an empty string if dialog was 
   * canceled by the user.
   */
  SimpleString SelectFile(const SimpleString& title, 
    const SimpleString& initialFullFilePath);

private:
  struct Window* m_pIntuiWindow;
};

#endif
