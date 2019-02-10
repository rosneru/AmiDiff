#include <proto/exec.h>
#include <proto/intuition.h>
#include "Request.h"

Request::Request()
  :  m_pBodyText(NULL),
     m_pButtonText(NULL)
{
  m_pBodyText = (struct IntuiText*) AllocMem(sizeof(struct IntuiText),
                                             MEMF_CLEAR);

  m_pButtonText = (struct IntuiText*) AllocMem(sizeof(struct IntuiText),
                                               MEMF_CLEAR);

  if(m_pBodyText == NULL || m_pButtonText == NULL)
  {
    return;
  }

  m_pBodyText->NextText  = NULL;
  m_pBodyText->TopEdge   = 10;
  m_pBodyText->LeftEdge  = 10;

  m_pButtonText->NextText  = NULL;
}

Request::~Request()
{
  if(m_pBodyText != NULL)
  {
    FreeMem(m_pBodyText, sizeof(struct IntuiText));
    return;
  }

  if(m_pButtonText != NULL)
  {
    FreeMem(m_pButtonText, sizeof(struct IntuiText));
    return;
  }
}

void Request::Show(struct Window* p_pWindow,
                   const SimpleString& p_Message,
                   const SimpleString& p_ButtonText)
{
  m_pBodyText->IText = (UBYTE*) p_Message.C_str();
  m_pButtonText->IText = (UBYTE*) p_ButtonText.C_str();

  AutoRequest(p_pWindow, m_pBodyText, NULL, m_pButtonText, NULL, NULL,
              180, 80);
}

void Request::Show(const SimpleString& p_Message,
                   const SimpleString& p_ButtonText)
{
  Show(NULL, p_Message, p_ButtonText);
}
