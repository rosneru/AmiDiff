#include "LinkedListNode.h"


LinkedListNode::LinkedListNode(void* p_pItemData, LinkedListNode* p_PrevListItem, LinkedListNode* p_NextListItem)
  : m_pPrv(p_PrevListItem),
    m_pNxt (p_NextListItem),
    m_pData(p_pItemData)
{

}

LinkedListNode::~ LinkedListNode()
{
//  delete m_pItem;
}
