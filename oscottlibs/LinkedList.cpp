#include <stdlib.h>
#include "LinkedList.h"


LinkedList::LinkedList()
  : m_pFirst(NULL),
    m_pActual(NULL),
    m_pLast(NULL),
    m_Size(0)
{

}

LinkedList::~ LinkedList(void)
{
  while (RemoveItem() == true);
}

bool LinkedList::RemoveItem()
{
  LinkedListNode* tmp;
  tmp = m_pActual;

  if (m_pActual != NULL)
  {
    if (m_pActual->m_pNxt != NULL)
    {
      if (m_pActual->m_pPrv != NULL)
      {
        m_pActual->m_pPrv->m_pNxt = m_pActual->m_pNxt;
      }
      else
      {
        m_pActual->m_pNxt->m_pPrv = NULL;
        m_pFirst = m_pActual->m_pNxt;
      }

      m_pActual->m_pNxt->m_pPrv = m_pActual->m_pPrv;
      m_pActual = m_pActual->m_pNxt;
      delete tmp;
      m_Size--;
      return true;
    }
    else if (m_pActual->m_pPrv != NULL)
    {
      if (m_pActual->m_pNxt != NULL)
      {
        m_pActual->m_pNxt->m_pPrv = m_pActual->m_pPrv;
      }
      else
      {
        m_pActual->m_pPrv->m_pNxt = NULL;
        m_pLast = m_pActual->m_pPrv;
      }

      m_pActual->m_pPrv->m_pNxt = m_pActual->m_pNxt;
      m_pActual = m_pActual->m_pPrv;
      delete tmp;
      m_Size--;
      return true;
    }
    else
    {
      m_pFirst = m_pLast = m_pActual = NULL;
      delete tmp;
      m_Size--;
      return true;
    }
  }
  else
  {
    return false;
  }

}


bool LinkedList::InsertHead(void* p_pItemIns)
{
  LinkedListNode* tmp;

  if (m_pFirst == NULL)
  {
    if ((m_pFirst = new LinkedListNode(p_pItemIns, NULL, NULL)) != NULL)
    {
      m_pLast = m_pActual = m_pFirst;
      m_Size++;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    tmp = m_pFirst;

    if ((m_pFirst = new LinkedListNode(p_pItemIns, NULL, m_pFirst)) != NULL)
    {
      tmp->m_pPrv = m_pFirst;
      m_pActual = m_pFirst;
      m_Size++;
      return true;
    }
    else
    {
      return false;
    }
  }
}


bool LinkedList::InsertTail(void* p_pItemIns)
{
  LinkedListNode* tmp;

  if (m_pLast == NULL)
  {
    if ((m_pLast = new LinkedListNode(p_pItemIns, NULL, NULL)) != NULL)
    {
      m_pFirst = m_pActual = m_pLast;
      m_Size++;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    tmp = m_pLast;

    if ((m_pLast = new LinkedListNode(p_pItemIns, m_pLast, NULL)) != NULL)
    {
      tmp->m_pNxt = m_pLast;
      m_pActual = m_pLast;
      m_Size++;
      return true;
    }
    else
    {
      return false;
    }
  }
}


bool LinkedList::InsertBefore(void* p_pItemIns)
{
  LinkedListNode* tmp;

  if (m_pActual == NULL)
  {
    return InsertTail(p_pItemIns);
  }

  if ((tmp = new LinkedListNode(p_pItemIns, m_pActual->m_pPrv, m_pActual)) != NULL)
  {
    if (m_pActual->m_pPrv != NULL)
    {
      m_pActual->m_pPrv->m_pNxt = tmp;
    }
    else
    {
      m_pFirst = tmp;
    }

    tmp->m_pPrv = m_pActual->m_pPrv;
    tmp->m_pNxt = m_pActual;
    m_pActual->m_pPrv = tmp;
    m_pActual = tmp;
    m_Size++;
    return true;
  }
  else
  {
    return false;
  }
}


bool LinkedList::InsertBehind(void* p_pItemIns)
{
  LinkedListNode* tmp;

  if (m_pActual == NULL)
  {
    return InsertHead(p_pItemIns);
  }

  if ((tmp = new LinkedListNode(p_pItemIns, m_pActual, m_pActual->m_pNxt)) != NULL)
  {
    if (m_pActual->m_pNxt != NULL)
    {
      m_pActual->m_pNxt->m_pPrv = tmp;
    }
    else
    {
      m_pLast = tmp;
    }

    tmp->m_pPrv = m_pActual;
    tmp->m_pNxt = m_pActual->m_pNxt;
    m_pActual->m_pNxt = tmp;
    m_pActual = tmp;
    m_Size++;
    return true;
  }
  else
  {
    return false;
  }
}


bool LinkedList::AddItemToList(void* p_pItemIns, int(*fcmp) (void* p_ItList, void* p_ItNew))
{
  if (GetFirst())        // Liste enthaelt schon Element(e)
  {
    do
    {
      if (fcmp(p_pItemIns, m_pActual->m_pData) > 0)
      {
        if (GetNext() == NULL)
        {
          return InsertTail(p_pItemIns);
        }
      }
      else
      {
        return InsertBefore(p_pItemIns);
      }
    }
    while (1);
  }
  else             // Liste enthaelt noch kein Element
  {
    return InsertHead(p_pItemIns);
  }
}


void* LinkedList::searchList(void* p_pItemSearch, int(fcmp) (void* pItList, void* pItNew))
{
  if (GetFirst() != NULL)
  {
    do
    {
      if (fcmp(p_pItemSearch, m_pActual->m_pData) == 0)
      {
        return m_pActual->m_pData;
      }
    }
    while (GetNext() != NULL);

    return NULL;
  }
  return NULL;
}

size_t LinkedList::Size()
{
  return m_Size;
}
void* LinkedList::GetFirst(void)
{
  if (m_pFirst != NULL)
  {
    m_pActual = m_pFirst;
    return m_pActual->m_pData;
  }

  return NULL;
}

void* LinkedList::GetLast(void)
{
  if (m_pLast != NULL)
  {
    m_pActual = m_pLast;
    return m_pActual->m_pData;
  }

  return NULL;
}

void* LinkedList::GetNext(void)
{
  if(m_pActual == NULL)
  {
    return NULL;
  }

  if (m_pActual->m_pNxt != NULL)
  {
    m_pActual = m_pActual->m_pNxt;
    return m_pActual->m_pData;
  }

  return NULL;
}

void* LinkedList::GetPrev(void)
{
  if(m_pActual == NULL)
  {
    return NULL;
  }

  if (m_pActual->m_pPrv != NULL)
  {
    m_pActual = m_pActual->m_pPrv;
    return m_pActual->m_pData;
  }

  return NULL;
}

void* LinkedList::GetSelected(void)
{
  if (m_pActual != NULL)
  {
    return m_pActual->m_pData;
  }

  return NULL;
}

void* LinkedList::GetIndexed(size_t p_Id)
{
  size_t currIdx = 0;
  void* pItem = GetFirst();
  while (currIdx != p_Id)
  {
    currIdx++;
    pItem = GetNext();
    if(pItem == NULL)
    {
      return pItem;
    }
  }

  return pItem;
}
