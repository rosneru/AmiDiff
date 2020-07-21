#ifndef LINKED_LIST_AMIGA_H
#define LINKED_LIST_AMIGA_H

#include <exec/types.h>

#include "LinkedList.h"
#include "LinkedListNode.h"

/**
 * Extends the oscottlibs/LinkedList to use an Amiga memory pool.
 *
 *
 * @author Uwe Rosner
 * @date 17/12/2019
 */
class LinkedListAmiga : public LinkedList
{
public:
  LinkedListAmiga(APTR& pPoolHeader);
  virtual ~LinkedListAmiga();

  const char* Error();

private:
  APTR& m_pPoolHeader;
  const char* m_pErrMsgLowMem;
  const char* m_pErrMsgMemPool;
  const char* m_pErrMsgUnknown;
  const char* m_pError;

  long* m_pDummy;

  LinkedListNode* allocListNode(void* pItem,
                                LinkedListNode* pPrev,
                                LinkedListNode* pNext);

  void freeListNode(LinkedListNode* pNode);
};

#endif // LINKED_LIST_AMIGA_H
