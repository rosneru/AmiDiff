#include "CmdSearch.h"


CmdSearch::CmdSearch(std::vector<WindowBase*>* pAllWindowsVector)
  : CommandBase(pAllWindowsVector)
{
}

CmdSearch::~CmdSearch()
{

}

void CmdSearch::Execute(struct Window* pActiveWindow)
{

}


  const char* CmdSearch::getSearchText()
  {
    return m_SearchText.c_str();
  }

  void CmdSearch::setSearchText(const char* pSearchText)
  {
    m_SearchText = pSearchText;
  }
  