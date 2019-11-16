#include "Document.h"

Document::Document()
: m_MaxLineLength(0),
  m_LastScrollDirection(None)
{

}

Document::~Document()
{

}

const char* Document::FileName() const
{
  return m_FileName;
}

void Document::SetFileName(const char* pFileName)
{
  m_FileName = const_cast<char*>(pFileName);
}
