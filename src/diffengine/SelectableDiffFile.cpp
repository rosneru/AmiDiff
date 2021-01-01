#include "SelectableDiffFile.h"

SelectableDiffFile::SelectableDiffFile(const DiffFileBase& diffFile)
  : m_DiffFile(diffFile)
{

}

const DiffLine* SelectableDiffFile::operator[](unsigned long index) const
{
  if(index >= m_DiffFile.getNumLines())
  {
    return NULL;
  }

  return m_DiffFile[index];
}

unsigned long SelectableDiffFile::getNumLines() const
{
  return m_DiffFile.getNumLines();
}

void SelectableDiffFile::addSelection(unsigned long fromLine, 
                                      unsigned long toLine, 
                                      unsigned long fromColumn, 
                                      unsigned long toColumn)
{
  m_TextSelection.Add(fromLine, toLine, fromColumn, toColumn);
}
