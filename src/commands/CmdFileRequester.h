#ifndef CMD_FILE_REQUESTER_H
#define CMD_FILE_REQUESTER_H

#include "AMenu.h"
#include "CommandBase.h"
#include "SimpleString.h"


/**
 * This command opens a file requester, letting the user select a file.
 *
 * @author Uwe Rosner
 * @date 09/09/2020
 */
class CmdFileRequester : public CommandBase
{
public:
  CmdFileRequester(std::vector<WindowBase*>* pAllWindowsVector,
                   const char* pTitle);

  virtual ~CmdFileRequester();
  virtual void Execute(struct Window* pActiveWindow);

  void SetPreselectPathOnly(bool yes);
  void SetInitialFilePath(const char* initialPath);

  const SimpleString& SelectedFile() const;
private:
  const char* m_pTitle;
  SimpleString m_InitialFileFullPath;
  SimpleString m_SelectedFilePath;
  bool m_bPreselectPathOnly;

  void selectFile(struct Window* pActiveWindow);

  static void intuiHook(struct Hook* pHook,
                        struct FileRequester* pFileRequester,
                        struct IntuiMessage* pMsg);
};

#endif
