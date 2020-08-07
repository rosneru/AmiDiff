#include "DiffOutputFileBase.h"

  DiffOutputFileBase::DiffOutputFileBase(const DiffInputFileBase& diffInputFile)
    : m_pEmptyLine(""),
      m_EmptyLineNumber(numDigits(diffInputFile.NumLines()) + 1, ' ')
  {

  }

DiffOutputFileBase::~DiffOutputFileBase()
{

}


void DiffOutputFileBase::AddBlankLine()
{
  AddLine(m_pEmptyLine, DiffLine::Normal, m_EmptyLineNumber.c_str());
}
