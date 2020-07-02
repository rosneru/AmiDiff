#ifndef JOINED_PUBLIC_SCREEN_H
#define JOINED_PUBLIC_SCREEN_H

#include "SimpleString.h"
#include "ScreenBase.h"

class JoinedPublicScreen : public ScreenBase
{
public:
  JoinedPublicScreen(const ADiffViewSettings& settings,
                     const char* pPubScreenName);

  virtual ~JoinedPublicScreen();

  bool Open();
  void Close();

private:
  const SimpleString m_PubScreenName;
};

#endif
