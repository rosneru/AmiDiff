#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
  #include <proto/intuition.h>
#endif

#include "CmdNavigateDiffBase.h"

CmdNavigateDiffBase::CmdNavigateDiffBase(std::vector<WindowBase*>* pAllWindowsVector,
                                         const DiffWorker& diffWorker,
                                         DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
    m_DiffWorker(diffWorker),
    m_DiffWindow(diffWindow),
    m_pDiffDocument(NULL),
    m_FormerLineId(-1)
{

}


long CmdNavigateDiffBase::getPrevResult()
{
  applyDocumentChanged();
  if((m_pDiffDocument == NULL) || 
     (m_pDiffDocument->getDiffIndices().size() < 1))
  {
    return -1;
  }

  if(m_DiffIndicesIterator == m_pDiffDocument->getDiffIndices().begin())
  {
    return -1;
  }

  m_DiffIndicesIterator--;
  return (*m_DiffIndicesIterator);
}


long CmdNavigateDiffBase::getPrevResult(size_t startLineId)
{
  applyDocumentChanged();
  if((m_pDiffDocument == NULL) || 
     (m_pDiffDocument->getDiffIndices().size() < 1))
  {
    return -1;
  }

  // if(startLineId >= m_LeftFile.getNumLines())
  // {
  //   // Avoid limit overrun
  //   startLineId = m_LeftFile.getNumLines() -1;
  // }
  std::list<size_t>::const_iterator itBeforeStart= 
    m_pDiffDocument->getDiffIndices().begin();
  itBeforeStart--;

  m_DiffIndicesIterator = m_pDiffDocument->getDiffIndices().end();
  m_DiffIndicesIterator--;

  // Forward iterator to the first result after or equal startLineId
  while((*m_DiffIndicesIterator) >= startLineId)
  {
    m_DiffIndicesIterator--;
    if(m_DiffIndicesIterator == itBeforeStart)
    {
      // Avoid overflow: back to last valid item
      m_DiffIndicesIterator = m_pDiffDocument->getDiffIndices().begin();
      return -1;
    }
  }

  return (*m_DiffIndicesIterator);
}


long CmdNavigateDiffBase::getNextResult()
{
  applyDocumentChanged();
  if((m_pDiffDocument == NULL) || 
     (m_pDiffDocument->getDiffIndices().size() < 1))
  {
    return -1;
  }

  m_DiffIndicesIterator++;

  if(m_DiffIndicesIterator == m_pDiffDocument->getDiffIndices().end())
  {
    // Avoid overflow: back to last valid item
    m_DiffIndicesIterator--;
    return -1;
  }

  return (*m_DiffIndicesIterator);
}


long CmdNavigateDiffBase::getNextResult(size_t startLineId)
{
  applyDocumentChanged();
  if((m_pDiffDocument == NULL) || 
     (m_pDiffDocument->getDiffIndices().size() < 1))
  {
    return -1;
  }

  m_DiffIndicesIterator = m_pDiffDocument->getDiffIndices().begin();

  // Forward iterator to the first result after or equal startLineId
  while((*m_DiffIndicesIterator) < startLineId)
  {
    m_DiffIndicesIterator++;
    if(m_DiffIndicesIterator == m_pDiffDocument->getDiffIndices().end())
    {
      // Avoid overflow: back to last valid item
      m_DiffIndicesIterator = m_pDiffDocument->getDiffIndices().begin();
    }
  }

  return (*m_DiffIndicesIterator);
}


void CmdNavigateDiffBase::signalNoResultFound()
{
  DisplayBeep(m_DiffWindow.getScreen().IntuiScreen());
}


void CmdNavigateDiffBase::applyDocumentChanged()
{
  if(m_pDiffDocument != m_DiffWorker.getDiffDocument())
  {
    m_FormerLineId = -1;
    m_pDiffDocument = m_DiffWorker.getDiffDocument();
    if(m_pDiffDocument == NULL)
    {
      return;
    }

    m_DiffIndicesIterator = m_pDiffDocument->getDiffIndices().begin();
  }
}
