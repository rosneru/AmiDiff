#ifndef REQUEST_H
#define REQUEST_H

#include <intuition/intuition.h>
#include "SimpleString.h"

/**
 * Displays a requester with a message to the user.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class Request
{
public:
  Request();
  virtual ~Request();

  void Show(struct Window* p_pWindow,
            const SimpleString& p_Message,
            const SimpleString& p_ButtonText);

  void Show(const SimpleString& p_Message,
            const SimpleString& p_ButtonText);

private:
  struct IntuiText* m_pBodyText;
  struct IntuiText* m_pButtonText;
};

#endif
