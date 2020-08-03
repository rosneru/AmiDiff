#include "DiffOutputFileBase.h"

  DiffOutputFileBase::DiffOutputFileBase()
  {

  }

DiffOutputFileBase::~DiffOutputFileBase()
{

}


void DiffOutputFileBase::AddBlankLine()
{
  static const char* pEmptyLine = "";
  AddLine(pEmptyLine, DiffLine::Normal, NULL);
}
