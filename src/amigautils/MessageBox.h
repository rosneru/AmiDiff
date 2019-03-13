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

  void Show(struct Window* p_pWindow,
            const SimpleString& p_WindowTitle,
            const SimpleString& p_Message,
            const SimpleString& p_ButtonText);

  void Show(struct Window* p_pWindow,
            const SimpleString& p_Message,
            const SimpleString& p_ButtonText);

  void Show(const SimpleString& p_Message,
            const SimpleString& p_ButtonText);

private:
    struct EasyStruct* m_pEasyStruct;
};

#endif
