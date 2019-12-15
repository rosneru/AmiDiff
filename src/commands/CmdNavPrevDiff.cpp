#include "CmdNavPrevDiff.h"
#include "MessageBox.h"

CmdNavPrevDiff::CmdNavPrevDiff(DiffWindow& diffWindow)
  : m_DiffWindow(diffWindow)
{

}

CmdNavPrevDiff::~CmdNavPrevDiff()
{

}

void CmdNavPrevDiff::Execute() const
{
  MessageBox request;
  request.Show("This program needs at least OS 3.0 / v39 to run.",
               "Ok");

  m_DiffWindow.NavigateToPrevDiff();
}

