#include "Command.h"


Command::Command(SimpleString p_Name)
{
  m_Name = p_Name;
}

Command::~Command()
{

}

SimpleString Command::Name()
{
  return m_Name;
}

