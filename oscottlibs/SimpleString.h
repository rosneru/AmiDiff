/* SimpleString.h                                                          */
/*-------------------------------------------------------------------*/
/* Class for basic string handling                                   */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created 12/2004, Updated 09/2018                                  */
/*-------------------------------------------------------------------*/
#ifndef OSCOTTLIBS_STRING
#define OSCOTTLIBS_STRING

namespace oscottlibs
{
  class SimpleString
  {
  public:
    SimpleString();
    SimpleString(const char* p_pTxt);
    SimpleString(const SimpleString& other);
    SimpleString(char p_Character, int p_Count);

    ~SimpleString();

    /**
     * Get C string equivalent
     */
    char* C_str();

    /**
     * Get length of string
     */
    int Length();

    // Operators
    SimpleString& operator=(const SimpleString& p_Other);
    SimpleString& operator=(const char* p_pOtherChar);
    SimpleString& operator+(const SimpleString& p_Other);
    SimpleString& operator+(const char* p_pOtherChar);
    bool operator==(const SimpleString& p_Other);
    bool operator<(const SimpleString& p_Other);
    bool operator>(const SimpleString& p_Other);
    char& operator[](int p_Index);

  private:
    char *m_pBuf;
    int   m_Len;
  };
}

#endif
