#include "CmdFileOpen.h"


CmdFileOpen::CmdFileOpen(SimpleString p_Name, DiffDocument& p_pDocument)
  : Command(p_Name),
  m_Document(p_pDocument)
{

}

CmdFileOpen::~CmdFileOpen()
{

}

void CmdFileOpen::Execute()
{
  /*
  std::cout << "CmdFileOpen::Execute() => " << m_Name << std::endl;
  std::cout << "> Enter file name: ";

  std::string fileName;

  std::cin.ignore();
  std::getline(std::cin, fileName);

  m_Document.Open(fileName);
  */
}
