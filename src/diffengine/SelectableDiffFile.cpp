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

void SelectableDiffFile::addSelection(unsigned long lineId, 
                                      unsigned long fromColumn, 
                                      unsigned long toColumn)
{
  m_TextSelection.add(lineId, fromColumn, toColumn);
}


unsigned long SelectableDiffFile::getNumNormalChars(unsigned long lineId, 
                                                    unsigned long columnId)
{
  unsigned long numMarkedChars = m_TextSelection.getNumMarkedChars(lineId, 
                                                                   columnId);

  unsigned long nextSelectionStart = m_TextSelection.getNextSelectionStart(lineId, 
                                                                           columnId);

  // TODO Continue
  
  return 0;
}

unsigned long SelectableDiffFile::getNumMarkedChars(unsigned long lineId, 
                                                    unsigned long columnId)
{
  return m_TextSelection.getNumMarkedChars(lineId, columnId);
}
