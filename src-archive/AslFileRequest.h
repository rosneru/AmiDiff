#ifndef ASL_FILE_REQUEST_H
#define ASL_FILE_REQUEST_H

#include <string>
#include <intuition/intuition.h>

/**
 * Provides an easy to use ASL file requester.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class AslFileRequest
{
public:
  AslFileRequest(struct Window*& pIntuiWindow);
  virtual ~AslFileRequest();

  /**
   * Opens a file requester and let the user select a file
   *
   * @param title
   * Text to be displayed in the file requesters title bar
   *
   * @param initialFileFullPath
   * A string with a full path and file name to pre-select this file
   * at request opening time. If empty or invalid, no file is pre-
   * selected.
   *
   * @param bPreselectPathOnly
   * When set to true, only the path of initialFileFullPath will be
   * pre-selected but not the file itself.
   *
   * @returns
   * Full path to the selected file or an empty string if dialog was
   * canceled by the user.
   */
  std::string SelectFile(const std::string& title,
                          const std::string& initialFileFullPath,
                          bool bPreselectPathOnly = false);

private:
  struct Window*& m_pIntuiWindow;
};

#endif

