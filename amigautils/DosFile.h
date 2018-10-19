#ifndef DOS_FILE_H
#define DOS_FILE_H

#include <libraries/dos.h>
#include "LinkedList.h"
#include "SimpleString.h"

/**
 * Provides an easy to use object for using dos file operation.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class DosFile
{
public:
  DosFile();
  virtual ~DosFile();

  /**
   * Wrapper for the AmigaOS Access modes
   */
  enum AccessMode
  {
    AM_OldFile,   ///> Opens an existing file for reading or writing
    AM_NewFile,   ///> Creates a new file for writing
    AM_ReadWrite  ///> Opens a file with an shared lock, creates it if it didn't exist
  };

  /**
   * Opens a file
   *
   * If file is already open it must be closed before opening a
   * different file for this object. Otherwise open will return false.
   *
   * @param p_FileName
   * Name of the file to open.

   *
   * @param p_AcsessMode
   * AccessMode of file to open; @see AccessMode
   *
   * @returns
   * If file successfully opened: true; if not: false
   */
  bool Open(SimpleString p_FileName, AccessMode p_AccessMode = AM_OldFile);

  /**
   * Closes the file
   */
   void Close();

  /**
   * Counts the number of lines in file. The file has  to be opened
   * before calling this function.
   *
   * @returns
   * Number of lines in file or -1 if file is not opened.
   */
  int CountLines();

  /**
   * Reads the file line by line storing each line at the bottom of the
   * provided list.
   *
   * The lines are stored as dynamically created SimpleString objects,
   * so the list has to be cleared manually after use
   *
   * @see SimpleString
   *
   * @param p_List
   * List to store the lines
   */
  bool ReadLines(LinkedList& p_List);

  /**
   * Returns a string with statistical informations about e.g. how long
   * counting lines or loading text into a list took.
   *
   * This information will be cleared at file closing time.
   */
  SimpleString GetTimeStatistics();

private:
  const size_t MAX_LINE_LENGTH;
  char* m_pLineBuf;
  BPTR m_pFile;
  SimpleString m_FileName;
  SimpleString m_TimeStatistics;
};

#endif
