/* SimpleString.h                                                          */
/*-------------------------------------------------------------------*/
/* Class for basic string handling                                   */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created 12/2004, Updated 09/2018                                  */
/*-------------------------------------------------------------------*/
#ifndef OSCOTTLIBS_STRING
#define OSCOTTLIBS_STRING

#include <stdlib.h>

class SimpleString
{
public:
  SimpleString();
  SimpleString(const char* p_pTxt);
  SimpleString(const SimpleString& other);
  SimpleString(char p_Character, int p_Count);
  SimpleString(long p_Number);

  ~SimpleString();

  // Operators
  SimpleString& operator=(const SimpleString& p_Other);
  SimpleString& operator=(const char* p_pOtherChar);
  SimpleString operator+(const SimpleString& p_Other);
  SimpleString operator+(const char* p_pOtherChar);
  SimpleString& operator+=(const SimpleString& p_Other);
  SimpleString& operator+=(const char* p_pOtherChar);
  SimpleString& operator+=(long p_Number);
  bool operator==(const SimpleString& p_Other);
  bool operator<(const SimpleString& p_Other);
  bool operator>(const SimpleString& p_Other);
  char& operator[](size_t p_Index);

  /**
   * Get C string equivalent
   */
  char* C_str();

  /**
   * Get length of string
   */
  size_t Length();


  /**
   * @brief
   * Returns a newly constructed string object with its value
   * initialized to a copy of a substring of this object.
   *
   * @param p_Index
   * Index of the first character to be copied as a substring. If this
   * is equal or greate to the string length, the function returns an
   * empty string.
   *
   * @param p_Len
   * Number of characters to include in the substring. If the string
   * is shorter, as many characters as possible are used.
   */
  SimpleString SubStr(size_t p_Index, size_t p_Len);


  /**
   * @brief
   * Replaces the portion of the string that begins at index pos and
   * spans len characters (or the part of the string in the range
   * between [i1,i2)) with a given SimpleString.
   *
   * WARNING This creates *no* copy. It changes the string directly!
   *
   * @param p_Index
   * Index of the first character to be replaced. If this is greater
   * than the string length, nothing will be done.
   *
   *
   * @param p_ReplaceBy
   * String that will be inserted at replace position
   *
   */
  SimpleString& Replace(size_t p_Index, SimpleString& p_ReplaceBy);

  /**
   * @brief
   * Replaces the portion of the string that begins at index pos and
   * spans len characters (or the part of the string in the range
   * between [i1,i2)) with a given text.
   *
   * WARNING This creates *no* copy. It changes the string directly!
   *
   * @param p_Index
   * Index of the first character to be replaced. If this is greater
   * than the string length, nothing will be done.
   *
   *
   * @param p_pReplaceBy
   * Pointer to the C string that will be inserted at replace position
   *
   */
  SimpleString& Replace(size_t p_Index, const char* p_pReplaceBy);

  /**
   * @brief
   * Inserts a given SimpleString at given position into this string
   *
   * WARNING This creates *no* copy. It changes the string directly!
   *
   */
  SimpleString& Insert(size_t p_Index, SimpleString& p_Insert);

  /**
   * @brief
   * Inserts a given C string at given position into this string
   *
   * WARNING This creates *no* copy. It changes the string directly!
   *
   */
  SimpleString& Insert(size_t p_Index, const char* p_pInsert);

  /**
   * @brief
   * Erases part of the string, reducing its length.
   *
   * Erases the portion of the string value that begins at p_Index and
   * spans p_Len characters (or until the end of the string, if the
   * content is too short).
   *
   * WARNING This creates *no* copy. It changes the string directly!
   */
  SimpleString& Erase(size_t p_Index, size_t p_Len);

  /**
   * @brief
   * Returns a newly constructed string object with all occurrences of
   * '\r', '\n', '\t' and ' ' from the end of the string are removed.
   *
   * Per Default the parameter p_bRemoveAlsoLeadingWhiteSp is set to
   * false so all leading and trailing white spaces are removed.
   *
   * @param p_bRemoveAlsoLeadingWhiteSp
   * When true only trailing white spaces are removed. When false the
   * trailing and also leading white spaces are removed.
   */
  SimpleString Trim(bool p_bRemoveAlsoLeadingWhiteSp = false);

private:
  size_t m_Len;
  char *m_pBuf;

  bool containsOnlyWhiteSpaces();
};

#endif
