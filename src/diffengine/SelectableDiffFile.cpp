#include "SelectableDiffFile.h"

SelectableDiffFile::SelectableDiffFile(DiffFileBase& diffFile)
  : m_DiffFile(diffFile)
{

}

DiffLine* SelectableDiffFile::operator[](unsigned long index)
{
  if(index >= m_DiffFile.NumLines())
  {
    return NULL;
  }

  return m_DiffFile.GetLine(index);
}

unsigned long SelectableDiffFile::getNumLines()
{
  return m_DiffFile.NumLines();
}

void SelectableDiffFile::addSelection(unsigned long fromLine, 
                                      unsigned long toLine, 
                                      unsigned long fromColumn, 
                                      unsigned long toColumn)
{
  m_TextSelection.Add(fromLine, toLine, fromColumn, toColumn);
}
