#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "Document.h"
#include "LinkedList.h"
#include "SimpleString.h"


class TextDocument : public Document
{
public:
  TextDocument();
  virtual ~TextDocument();
  bool Load(const SimpleString& p_FileName);
  void Clear();



  virtual const size_t NumLines() const;

  virtual const SimpleString* GetFirstLine();
  virtual const SimpleString* GetCurrentLine();
  virtual const SimpleString* GetPreviousLine();
  virtual const SimpleString* GetNextLine();
  virtual const SimpleString* GetIndexedLine(int p_LineIdx);

  /**
   * Gets the next line using GetNext() when LastScrollDirection is
   * already set to NextLine. If this is not the case GetIndexed()
   * is used to get the desired "next" line.
   * 
   * Sets the mode to NextLine, so the next call will use the faster
   * GetNext() instead of GetIndexed() if the mode is not changed 
   * outside.
   * 
   * @param p_NextId
   * Id of the line to get with GetIndexed() if not already in 
   * NextLine mode.
   * 
   * @returns
   * Next text line or NULL if there is none.
   */
  const SimpleString* GetNextOrIndexedLine(int p_NextId);

  /**
   * Gets the previous line using GetNext() when LastScrollDirection 
   * is already set to PreviousLine. If this is not the case 
   * GetIndexed() is used to get the desired "previous" line.
   * 
   * Sets the mode to PreviousLine, so the next call will use the 
   * faster GetPrevious() instead of GetIndexed() if the mode is not 
   * changed outside.
   * 
   * @param p_NextId
   * Id of the line to get with GetIndexed() if not already in 
   * PreviousLine mode.
   * 
   * @returns
   * Previous text line or NULL if there is none.
   */
  const SimpleString* GetPreviousOrIndexedLine(int p_PreviousId);

protected:
  /**
   * Used to describe if the last line obtaining method was 
   * getNextLine(), getPreviousLine() ore none of both.
   */
  enum LastScrollDirection
  {
    None,
    NextLine,
    PreviousLine,
  };

  /**
   * Stores if the last scroll line obtaining method was getNextLine(),
   * getPreviousLine() ore none of both.
   */
  LastScrollDirection m_LastScrollDirection;

private:
  LinkedList m_Lines;
};


#endif // TEXT_DOCUMENT_H
