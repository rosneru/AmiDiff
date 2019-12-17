#include <new.h> // Needed for 'replacement new', see below.

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>


#include "LinkedListAmiga.h"

LinkedListAmiga::LinkedListAmiga(APTR& pPoolHeader)
  : m_pPoolHeader(pPoolHeader),
    m_pErrMsgLowMem("Not enough memory."),
    m_pErrMsgMemPool("Memory pool not initialized."),
    m_pErrMsgUnknown("Unknown error in class LinkedListAmiga."),
    m_pError(m_pErrMsgUnknown)
{
}


LinkedListAmiga::~LinkedListAmiga()
{
  Clear();
}


void LinkedListAmiga::Clear()
{
  // The array address is cleared but nothing else is deleted or freed
  // here, because an external memory pool is used for all heap allocs.
  // On exit or when performing another diff that memory pool is
  // deleted outside with just one call. On the Amiga this is way
  // faster than e.g. calling 5000 single delete [] in random order.

  m_pDiffLinesArray = NULL;
}


const char* LinkedListAmiga::Error()
{
  return m_pError;
}


LinkedListNode* LinkedListAmiga::allocListNode(void* pItem,
                                               LinkedListNode* pPrev,
                                               LinkedListNode* pNext)
{
  if(m_pPoolHeader == NULL)
  {
    // Won't work without memory pool
    m_pError = m_pErrMsgMemPool;
    return false;
  }

  LinkedListNode* pNode = AllocPooled(m_pPoolHeader, 
                                      sizeof(LinkedListNode));

  if(pNode == NULL)
  {
    m_pError = m_pErrMsgLowMem;
  }

  return pNode;
}
