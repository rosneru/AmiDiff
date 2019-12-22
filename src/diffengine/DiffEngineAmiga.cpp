#ifdef linux
  #include <new>
#else
  #include <new.h> // Needed for 'replacement new', see below.
#endif

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>


#include "DiffEngineAmiga.h"

DiffEngineAmiga::DiffEngineAmiga(DiffFileBase& a,
                                 DiffFileBase& b,
                                 DiffFileBase& aDiff,
                                 DiffFileBase& bDiff,
                                 APTR& pPoolHeader,
                                 bool& bCancelRequested,
                                 LinkedList* pDiffStartIdxsList)
  : DiffEngine(a, b, aDiff, bDiff, bCancelRequested, pDiffStartIdxsList),
    m_pPoolHeader(pPoolHeader),
    m_pErrMsgLowMem("Not enough memory."),
    m_pErrMsgMemPool("Memory pool not initialized."),
    m_pErrMsgUnknown("Unknown error in class DiffEngineAmiga."),
    m_pError(m_pErrMsgUnknown)
{
}


DiffEngineAmiga::~DiffEngineAmiga()
{
  Clear();
}


void DiffEngineAmiga::Clear()
{

}


const char* DiffEngineAmiga::Error()
{
  return m_pError;
}


void DiffEngineAmiga::addDiffIdxToList(size_t diffIdx)
{
  if(m_pPoolHeader == NULL)
  {
    // Won't work without memory pool
    m_pError = m_pErrMsgMemPool;
    return;
  }

  size_t* pItem = (size_t*) AllocPooled(m_pPoolHeader, sizeof(size_t));
  if(pItem == NULL)
  {
    m_pError = m_pErrMsgLowMem;
    return;
  }

  pItem[0] = diffIdx;

  m_pDiffStartIdxsList->InsertTail(pItem);
}
