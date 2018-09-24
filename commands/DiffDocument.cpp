
#include "DiffDocument.h"


DiffDocument::DiffDocument(SimpleString p_Name)
  : m_Name(p_Name)
{

}

DiffDocument::~DiffDocument()
{

}

bool DiffDocument::Open(SimpleString p_FileName)
{
  /*
  std::cout << "  DiffDocument::Open() ==> Document name = " << m_Name
            << " ==> FileName = " << p_FileName << std::endl;
  */
  return true;
}

SimpleString DiffDocument::Name()
{
  return m_Name;
}
