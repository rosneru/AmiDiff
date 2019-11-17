#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <intuition/intuition.h>
#include "SimpleString.h"

/**
 * Displays a requester with a message to the user.
 *
 * @author Uwe Rosner
 * @date 10/02/2019
 */
class MessageBox
{
public:
  MessageBox();
  virtual ~MessageBox();

  void Show(struct Window* pWindow,
            const SimpleString& windowTitle,
            const SimpleString& message,
            const SimpleString& buttonText);

  void Show(struct Window* p_pWindow,
            const SimpleString& message,
            const SimpleString& buttonText);

  void Show(const SimpleString& message,
            const SimpleString& buttonText);

private:
    struct EasyStruct* m_pEasyStruct;
};

#endif
