#include "SimpleString.h"
#include <string.h>

SimpleString::SimpleString()
{
  m_pBuf = new char[1];
  m_pBuf[0] = '\0';
  m_Len = 0;
}

SimpleString::SimpleString(const char *p_pTxt)
{
  m_Len = strlen(p_pTxt);
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, p_pTxt);
}

SimpleString::SimpleString(char p_Character, int p_Count)
{
  m_Len = p_Count;
  m_pBuf = new char[p_Count + 1];

  for(int i=0; i<p_Count; i++)
  {
    m_pBuf[i] = p_Character;
  }

  m_pBuf[p_Count]='\0';
}

char* SimpleString::C_str()
{
  return m_pBuf;
}

int SimpleString::Length()
{
  return m_Len;
}

SimpleString::~SimpleString()
{
  delete[] m_pBuf;
}


SimpleString& SimpleString::operator=(const SimpleString& p_Other)
{
  m_Len = p_Other.m_Len;
  if(&p_Other == this)
  {
    return *this;
  }

  delete[] m_pBuf;
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, p_Other.m_pBuf);
  return *this;
}

SimpleString& SimpleString::operator=(const char* p_pOtherChar)
{
  delete[] m_pBuf;

  m_Len = strlen(p_pOtherChar);
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, p_pOtherChar);
  return *this;
}


SimpleString& SimpleString::operator+(const SimpleString& p_Other)
{
  m_Len = m_Len + p_Other.m_Len;

  char* pNewBuf;
  pNewBuf = new char[m_Len + 1];

  strcpy(pNewBuf, m_pBuf);
  strcat(pNewBuf, p_Other.m_pBuf);

  delete[] m_pBuf;
  m_pBuf = pNewBuf;

  return *this;
}

SimpleString& SimpleString::operator+(const char* p_pOtherChar)
{
  m_Len = m_Len + strlen(p_pOtherChar);

  char* pNewBuf;
  pNewBuf = new char[m_Len + 1];

  strcpy(pNewBuf, m_pBuf);
  strcat(pNewBuf, p_pOtherChar);

  delete[] m_pBuf;
  m_pBuf = pNewBuf;

  return *this;
}

bool SimpleString::operator==(const SimpleString& p_Other)
{
  if(strcmp(m_pBuf, p_Other.m_pBuf) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool SimpleString::operator<(const SimpleString& p_Other)
{
  if(strcmp(m_pBuf, p_Other.m_pBuf) < 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool SimpleString::operator>(const SimpleString& p_Other)
{
  if(strcmp(m_pBuf, p_Other.m_pBuf) > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

char& SimpleString::operator[](int p_Index)
{
  if(p_Index >= 0 && p_Index < m_Len)
  {
    return m_pBuf[p_Index];
  }
  else
  {
    throw "Array subscript out of range";
  }
}
