#ifdef linux
  #include <new>
#else
  #include <new.h> // Needed for 'replacement new', see below.
#endif

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include "LinkedListNode.h"

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

  LinkedListNode* pNode = (LinkedListNode*)
    AllocPooled(m_pPoolHeader, sizeof(LinkedListNode));

  // The next line is called 'replacement new'. It creates an object
  // of LinkedListNode on the address pNode and calls the constructor.
  // This has to be done here because a memory pool is used and the
  // normal operator 'new' which reserves memory automatically
  // wouldn't be appropriate.
  new (pNode) LinkedListNode(pItem, pPrev, pNext);

  if(pNode == NULL)
  {
    m_pError = m_pErrMsgLowMem;
  }

  return pNode;
}

void LinkedListAmiga::freeListNode(LinkedListNode* pNode)
{
  // Should be done outside by deleting the memory pool as a whole.
}
