/* LinkedList.h                                                      */
/*-------------------------------------------------------------------*/
/* Class of a linked template list                                   */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created 11/2004, Updated 09/2018                                  */
/*-------------------------------------------------------------------*/

#ifndef OSCOTTLIBS_LINKEDLIST
#define OSCOTTLIBS_LINKEDLIST

#include <stdlib.h>
#include "LinkedListNode.h"

using namespace std;

class LinkedList
{
  LinkedListNode* m_pFirst;
  LinkedListNode* m_pActual;
  LinkedListNode* m_pLast;

public:
  LinkedList(void);
  ~LinkedList(void);

  bool InsertHead(void* p_pItemIns);
  bool InsertTail(void* p_pItemIns);
  bool InsertBefore(void* p_pItemIns);
  bool InsertBehind(void* p_pItemIns);
  bool AddItemToList(void* p_pItemIns, int(*fcmp) (void* p_ItList, void* p_ItNew));

  bool RemoveItem(void);

  void* GetFirst(void);
  void* GetLast(void);
  void* GetNext(void);
  void* GetPrev(void);
  void* GetSelected(void);

  void* GetIndexed(size_t p_Id);

  void* searchList(void* p_pItemSearch, int(fcmp) (void* pItList, void* pItNew));

  size_t Size();

private:
  size_t m_Size;
};

#endif
