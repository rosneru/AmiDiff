#include "Document.h"

Document::Document()
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
