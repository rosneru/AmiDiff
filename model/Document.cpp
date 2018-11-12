#include "Document.h"

Document::Document()
{

}

Document::~Document()
{

}

const SimpleString& Document::FileName()
{
  return m_FileName;
}

const SimpleString& Document::TimeStatistics()
{
  return m_TimeStatistics;
}
