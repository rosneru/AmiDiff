#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include "MessageBox.h"

MessageBox::MessageBox()
  : m_pEasyStruct(NULL)
{
  m_pEasyStruct = (struct EasyStruct*)
    AllocVec(sizeof(struct EasyStruct), MEMF_CLEAR);

  if(m_pEasyStruct == NULL)
  {
    return;
  }

  m_pEasyStruct->es_StructSize = sizeof(EasyStruct);
  m_pEasyStruct->es_Flags = 0;
  m_pEasyStruct->es_Title = NULL;
}

MessageBox::~MessageBox()
{
  if(m_pEasyStruct != NULL)
  {
    FreeVec(m_pEasyStruct);
    m_pEasyStruct = NULL;
  }
}

void MessageBox::Show(struct Window* p_pWindow,
                      const SimpleString& p_WindowTitle,
                      const SimpleString& p_Message,
                      const SimpleString& p_ButtonText)
{
  m_pEasyStruct->es_Title = (UBYTE*) p_WindowTitle.C_str();
  Show(p_pWindow, p_Message, p_ButtonText);
}

void MessageBox::Show(struct Window* p_pWindow,
                      const SimpleString& p_Message,
                      const SimpleString& p_ButtonText)
{
  if(m_pEasyStruct == NULL)
  {
    return;
  }

  m_pEasyStruct->es_TextFormat = (UBYTE*)p_Message.C_str();
  m_pEasyStruct->es_GadgetFormat = (UBYTE*)p_ButtonText.C_str();

  EasyRequest(p_pWindow, m_pEasyStruct, NULL);
}

void MessageBox::Show(const SimpleString& p_Message,
                      const SimpleString& p_ButtonText)
{
  Show(NULL, p_Message, p_ButtonText);
}
