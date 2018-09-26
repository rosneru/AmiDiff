#include <stdlib.h>
#include "LinkedList.h"


LinkedList::LinkedList()
  : m_pFirst(nullptr),
    m_pActual(nullptr),
    m_pLast(nullptr)
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

  if (m_pActual != nullptr)
  {
    if (m_pActual->m_pNxt != nullptr)
    {
      if (m_pActual->m_pPrv != nullptr)
      {
        m_pActual->m_pPrv->m_pNxt = m_pActual->m_pNxt;
      }
      else
      {
        m_pActual->m_pNxt->m_pPrv = nullptr;
        m_pFirst = m_pActual->m_pNxt;
      }

      m_pActual->m_pNxt->m_pPrv = m_pActual->m_pPrv;
      m_pActual = m_pActual->m_pNxt;
      delete tmp;
      return true;
    }
    else if (m_pActual->m_pPrv != nullptr)
    {
      if (m_pActual->m_pNxt != nullptr)
      {
        m_pActual->m_pNxt->m_pPrv = m_pActual->m_pPrv;
      }
      else
      {
        m_pActual->m_pPrv->m_pNxt = nullptr;
        m_pLast = m_pActual->m_pPrv;
      }

      m_pActual->m_pPrv->m_pNxt = m_pActual->m_pNxt;
      m_pActual = m_pActual->m_pPrv;
      delete tmp;
      return true;
    }
    else
    {
      m_pFirst = m_pLast = m_pActual = nullptr;
      delete tmp;
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

  if (m_pFirst == nullptr)
  {
    if ((m_pFirst = new LinkedListNode(p_pItemIns, nullptr, nullptr)) != nullptr)
    {
      m_pLast = m_pActual = m_pFirst;
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

    if ((m_pFirst = new LinkedListNode(p_pItemIns, nullptr, m_pFirst)) != nullptr)
    {
      tmp->m_pPrv = m_pFirst;
      m_pActual = m_pFirst;
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

  if (m_pLast == nullptr)
  {
    if ((m_pLast = new LinkedListNode(p_pItemIns, nullptr, nullptr)) != nullptr)
    {
      m_pFirst = m_pActual = m_pLast;
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

    if ((m_pLast = new LinkedListNode(p_pItemIns, m_pLast, nullptr)) != nullptr)
    {
      tmp->m_pNxt = m_pLast;
      m_pActual = m_pLast;
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

  if (m_pActual == nullptr)
  {
    return InsertTail(p_pItemIns);
  }

  if ((tmp = new LinkedListNode(p_pItemIns, m_pActual->m_pPrv, m_pActual)) != nullptr)
  {
    if (m_pActual->m_pPrv != nullptr)
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

  if (m_pActual == nullptr)
  {
    return InsertHead(p_pItemIns);
  }

  if ((tmp = new LinkedListNode(p_pItemIns, m_pActual, m_pActual->m_pNxt)) != nullptr)
  {
    if (m_pActual->m_pNxt != nullptr)
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
        if (GetNext() == nullptr)
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
  if (GetFirst() != nullptr)
  {
    do
    {
      if (fcmp(p_pItemSearch, m_pActual->m_pData) == 0)
      {
        return m_pActual->m_pData;
      }
    }
    while (GetNext() != nullptr);

    return nullptr;
  }
  return nullptr;
}
void* LinkedList::GetFirst(void)
{
  if (m_pFirst != nullptr)
  {
    m_pActual = m_pFirst;
    return m_pActual->m_pData;
  }

  return nullptr;
}

void* LinkedList::GetLast(void)
{
  if (m_pLast != nullptr)
  {
    m_pActual = m_pLast;
    return m_pActual->m_pData;
  }

  return nullptr;
}

void* LinkedList::GetNext(void)
{
  if(m_pActual == nullptr)
  {
    return nullptr;
  }

  if (m_pActual->m_pNxt != nullptr)
  {
    m_pActual = m_pActual->m_pNxt;
    return m_pActual->m_pData;
  }

  return nullptr;
}

void* LinkedList::GetPrev(void)
{
  if(m_pActual == nullptr)
  {
    return nullptr;
  }

  if (m_pActual->m_pPrv != nullptr)
  {
    m_pActual = m_pActual->m_pPrv;
    return m_pActual->m_pData;
  }

  return nullptr;
}

void* LinkedList::GetSelected(void)
{
  if (m_pActual != nullptr)
  {
    return m_pActual->m_pData;
  }

  return nullptr;
}

void* LinkedList::GetIndexed(int p_Id)
{
  int i = 1;
  void* element;

  if ((element = GetFirst()) != nullptr)
  {
    for (i = 2; i <= p_Id; i++)
    {
      element = GetNext();
    }

    return element;
  }
  return nullptr;
}
