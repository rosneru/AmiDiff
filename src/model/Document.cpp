#include "Document.h"

Document::Document()
{

}

Document::~Document()
{

}

const SimpleString& Document::FileName() const
{
  return m_FileName;
}

void Document::SetFileName(const SimpleString& p_FileName)
{
  m_FileName = p_FileName;
}
