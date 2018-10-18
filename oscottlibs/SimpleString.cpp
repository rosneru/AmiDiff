#include "SimpleString.h"
#include <string.h>
#include <stdio.h>


SimpleString::SimpleString()
  : m_Len(0),
    m_pBuf(new char[m_Len + 1])
{
  m_pBuf[0] = '\0';
}

SimpleString::SimpleString(const char *p_pTxt)
  : m_Len(strlen(p_pTxt)),
    m_pBuf(new char[m_Len + 1])
{
  strcpy(m_pBuf, p_pTxt);
}

SimpleString::SimpleString(const SimpleString& other)
  : m_Len(other.m_Len),
    m_pBuf(new char[m_Len + 1])
{
  strcpy(m_pBuf, other.m_pBuf);
}

SimpleString::SimpleString(char p_Character, int p_Count)
  : m_Len(p_Count),
    m_pBuf(new char[m_Len + 1])
{
  for(int i=0; i<p_Count; i++)
  {
    m_pBuf[i] = p_Character;
  }

  m_pBuf[p_Count]='\0';
}

SimpleString::SimpleString(long p_Number)
  : m_Len(0),
    m_pBuf(NULL)
{
  // Get number of digits
  int digits = 1;
  long div = p_Number;
  while(div /= 10)
  {
    digits++;
  }

  if(p_Number < 0)
  {
    // Space for '-' char
    digits++;
  }

  m_Len = digits;
  m_pBuf = new char[m_Len + 1];

  sprintf(m_pBuf, "%ld", p_Number);
  m_pBuf[m_Len] = '\0';
}

SimpleString::~SimpleString()
{
  delete[] m_pBuf;
}

char* SimpleString::C_str()
{
  return m_pBuf;
}

size_t SimpleString::Length()
{
  return m_Len;
}

SimpleString SimpleString::SubStr(size_t p_Index, size_t p_Len)
{
  if(p_Len > m_Len)
  {
    p_Len = m_Len;
  }

  if(p_Index > (m_Len - 1))
  {
    p_Index = m_Len - 1;
    p_Len = 0;
  }

  if((p_Index + p_Len) >= m_Len)
  {
    size_t shortenBy = p_Index + p_Len - m_Len;
    p_Len -= shortenBy;
  }

  char* pNewBuf = new char[p_Len + 1];

  strncpy(pNewBuf, m_pBuf + p_Index, p_Len);
  pNewBuf[p_Len] = '\0';

  SimpleString newStr(pNewBuf);
  delete[] pNewBuf;
  return newStr;
}

SimpleString& SimpleString::Replace(size_t p_Index, SimpleString& p_ReplaceBy)
{
  size_t copyLen = p_ReplaceBy.Length();
  if((p_Index + copyLen) > m_Len)
  {
    size_t shortenBy = p_Index + copyLen - m_Len;
    copyLen -= shortenBy;
  }

  strncpy(m_pBuf + p_Index, p_ReplaceBy.m_pBuf, copyLen);
  m_pBuf[m_Len] = '\0';

  return *this;
}

SimpleString&SimpleString::Replace(size_t p_Index, const char* p_pReplaceBy)
{
  size_t copyLen = strlen(p_pReplaceBy);
  if((p_Index + copyLen) > m_Len)
  {
    size_t shortenBy = p_Index + copyLen - m_Len;
    copyLen -= shortenBy;
  }

  strncpy(m_pBuf + p_Index, p_pReplaceBy, copyLen);
  m_pBuf[m_Len] = '\0';

  return *this;
}

SimpleString& SimpleString::Insert(size_t p_Index, SimpleString& p_Insert)
{
  if(p_Index > m_Len)
  {
    p_Index = m_Len;
  }

  m_Len = m_Len + p_Insert.m_Len;
  char* pNewBuf = new char[m_Len + 1];

  strncpy(pNewBuf, m_pBuf, p_Index);
  strncpy(pNewBuf + p_Index, p_Insert.m_pBuf, p_Insert.m_Len);
  strcpy(pNewBuf + p_Index + p_Insert.m_Len, m_pBuf + p_Index);

  delete[] m_pBuf;
  m_pBuf = pNewBuf;
  return *this;
}

SimpleString& SimpleString::Insert(size_t p_Index, const char* p_pInsert)
{
  if(p_Index > m_Len)
  {
    p_Index = m_Len;
  }

  m_Len = m_Len + strlen(p_pInsert);
  char* pNewBuf = new char[m_Len + 1];

  strncpy(pNewBuf, m_pBuf, p_Index);
  strncpy(pNewBuf + p_Index, p_pInsert, strlen(p_pInsert));
  strcpy(pNewBuf + p_Index + strlen(p_pInsert), m_pBuf + p_Index);

  delete[] m_pBuf;
  m_pBuf = pNewBuf;
  return *this;
}

SimpleString& SimpleString::Erase(size_t p_Index, size_t p_Len)
{
  if(p_Index >= m_Len)
  {
    return *this;
  }

  size_t copyLen1 = p_Index;
  size_t copyLen2 = m_Len - p_Index - p_Len;
  if(p_Index + p_Len > m_Len)
  {
    copyLen2 = 0;
  }

  m_Len = copyLen1 + copyLen2;
  char* pNewBuf = new char[m_Len + 1];

  if(copyLen1 > 0)
  {
    strncpy(pNewBuf, m_pBuf, copyLen1);
  }

  if(copyLen2 > 0)
  {
    strncpy(pNewBuf + copyLen1, m_pBuf + copyLen1 + p_Len, copyLen2);
  }

  pNewBuf[m_Len] = '\0';

  delete[] m_pBuf;
  m_pBuf = pNewBuf;
  return *this;
}



SimpleString SimpleString::Trim(bool p_bRemoveAlsoLeadingWhiteSp)
{
  if(containsOnlyWhiteSpaces() == true)
  {
    // String contains only white spaces: return an empty string
    SimpleString trimmed = "";
    return trimmed;
  }

  SimpleString trimmed = *this;
  if(p_bRemoveAlsoLeadingWhiteSp == true)
  {
    for (size_t i = 0; i < Length(); i++)
    {
      char& c = trimmed[i];
      if(!(c == '\r') &&
         !(c == '\n') &&
         !(c == '\t') &&
         !(c == ' '))
      {
        trimmed = trimmed.SubStr(i, trimmed.Length() - i);
        break;
      }
    }
  }

  if(trimmed.Length() == 1)
  {
    char& c = trimmed[0];
    if((c == '\r') ||
       (c == '\n') ||
       (c == '\t') ||
       (c == ' '))
    {
      // The only character of the string is a to be trimmed special
      // char: create an new empty string
      trimmed = "";
    }
  }
  else if(trimmed.Length() > 1)
  {
    size_t i = trimmed.Length() - 1;
    char& c = trimmed[i];
    while((c == '\r') ||
       (c == '\n') ||
       (c == '\t') ||
       (c == ' '))
    {
      i--;
      c = trimmed[i];
    }

    size_t shortenBy = trimmed.Length() - i - 1;
    trimmed = trimmed.SubStr(0, trimmed.Length() - shortenBy);
  }

  return trimmed;
}

bool SimpleString::containsOnlyWhiteSpaces()
{
  for(size_t i=0; i < m_Len; i++)
  {
    if(!(m_pBuf[i] == '\r') &&
       !(m_pBuf[i] == '\n') &&
       !(m_pBuf[i] == '\t') &&
       !(m_pBuf[i] == ' '))
    {
      // first non-whitespace character found
      return false;
    }
  }

  return true;
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


SimpleString SimpleString::operator+(const SimpleString& p_Other)
{
  m_Len = m_Len + p_Other.m_Len;
  char* pNewBuf = new char[m_Len + 1];

  strcpy(pNewBuf, m_pBuf);
  strcat(pNewBuf, p_Other.m_pBuf);

  SimpleString newStr(pNewBuf);
  delete[] pNewBuf;
  return newStr;
}

SimpleString SimpleString::operator+(const char* p_pOtherChar)
{
  m_Len = m_Len + strlen(p_pOtherChar);
  char* pNewBuf = new char[m_Len + 1];

  strcpy(pNewBuf, m_pBuf);
  strcat(pNewBuf, p_pOtherChar);

  SimpleString newStr(pNewBuf);
  delete[] pNewBuf;
  return newStr;
}

SimpleString& SimpleString::operator+=(const SimpleString& p_Other)
{
  m_Len = m_Len + p_Other.m_Len;

  char* pOldBuf = m_pBuf;
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, pOldBuf);
  strcat(m_pBuf, p_Other.m_pBuf);

  delete[] pOldBuf;
  return *this;
}

SimpleString& SimpleString::operator+=(const char* p_pOtherChar)
{
  m_Len = m_Len + strlen(p_pOtherChar);

  char* pOldBuf = m_pBuf;
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, pOldBuf);
  strcat(m_pBuf, p_pOtherChar);

  delete[] pOldBuf;
  return *this;
}

SimpleString& SimpleString::operator+=(long p_Number)
{
  SimpleString numberStr(p_Number);

  m_Len = m_Len + numberStr.Length();

  char* pOldBuf = m_pBuf;
  m_pBuf = new char[m_Len + 1];
  strcpy(m_pBuf, pOldBuf);
  strcat(m_pBuf, numberStr.C_str());

  delete[] pOldBuf;
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

char& SimpleString::operator[](size_t p_Index)
{
  if(p_Index < m_Len)
  {
    return m_pBuf[p_Index];
  }
  else
  {
    throw "Array subscript out of range";
  }
}
