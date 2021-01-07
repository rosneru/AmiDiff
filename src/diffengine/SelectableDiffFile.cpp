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


long SelectableDiffFile::getNumNormalChars(unsigned long lineId, 
                                           unsigned long columnId)
{
  if(lineId >= m_DiffFile.getNumLines())
  {
    return 0;
  }

  if(columnId > m_DiffFile[lineId]->getNumChars())
  {
    return 0;
  }

  unsigned long numMarkedChars = m_TextSelection.getNumMarkedChars(lineId, 
                                                                   columnId);

  long nextSelectionStart = m_TextSelection.getNextSelectionStart(lineId, 
                                                                  columnId);

  if(numMarkedChars > 0)
  {
    return 0;
  }

  if(nextSelectionStart > 0)
  {
    return nextSelectionStart - columnId;
  }
  
  return columnId > m_DiffFile[lineId]->getNumChars() - columnId;
}


long SelectableDiffFile::getNumMarkedChars(unsigned long lineId, 
                                           unsigned long columnId)
{
  return m_TextSelection.getNumMarkedChars(lineId, columnId);
}
