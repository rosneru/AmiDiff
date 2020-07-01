#ifndef CLONED_WORKBENCH_SCREEN_H
#define CLONED_WORKBENCH_SCREEN_H

#include "ScreenBase.h"

class ClonedWorkbenchScreen : public ScreenBase
{
public:
  ClonedWorkbenchScreen(const ADiffViewSettings& settings, 
                        const char* pTitle,
                        short depth);
  virtual ~ClonedWorkbenchScreen();

  bool Open();
  void Close();

private:
  const char* m_pTitle;
  short m_Depth;
};

#endif
