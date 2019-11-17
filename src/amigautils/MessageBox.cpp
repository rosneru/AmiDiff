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

void MessageBox::Show(struct Window* pWindow,
                      const SimpleString& windowTitle,
                      const SimpleString& message,
                      const SimpleString& buttonText)
{
  m_pEasyStruct->es_Title = (UBYTE*) windowTitle.C_str();
  Show(pWindow, message, buttonText);
}

void MessageBox::Show(struct Window* pWindow,
                      const SimpleString& message,
                      const SimpleString& buttonText)
{
  if(m_pEasyStruct == NULL)
  {
    return;
  }

  m_pEasyStruct->es_TextFormat = (UBYTE*)message.C_str();
  m_pEasyStruct->es_GadgetFormat = (UBYTE*)buttonText.C_str();

  EasyRequest(pWindow, m_pEasyStruct, NULL);
}

void MessageBox::Show(const SimpleString& message,
                      const SimpleString& buttonText)
{
  Show(NULL, message, buttonText);
}
