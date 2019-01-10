#ifndef AMIGA_FILE_H
#define AMIGA_FILE_H

#include <exec/types.h>
#include <libraries/dos.h>
#include "Array.h"
#include "SimpleString.h"

/**
 * Provides an easy to use object for using dos file operation.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class AmigaFile
{
public:
  AmigaFile();
  virtual ~AmigaFile();

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
  bool Open(const SimpleString& p_FileName,
    AccessMode p_AccessMode = AM_OldFile);

  /**
   * Closes the file
   */
   void Close();

  /**
   * Counts the number of lines in file. The file has  to be opened
   * before calling this method.
   *
   * @returns
   * Number of lines in file or -1 if file is not opened.
   */
  int CountLines();

  /**
   * Gets the file size in bytes. The file has  to be opened before
   * calling this method.
   *
   * NOTE: Changes the file handle position! After this operation the
   * file handle points to the start of tze file.
   *
   * @returns
   * File size in bytes, 0 if not opened.
   */
  ULONG GetSize();

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
  bool ReadLines(Array<SimpleString*>& p_Array);

  /**
   * Reads the next line from file
   *
   * @param p_Line
   * Reference to a string variable to read into
   *
   * @returns
   * true when reading was successful or false if eof or file not open
   */
  bool ReadLine(SimpleString& p_Line);

private:
  const ULONG MAX_LINE_LENGTH;
  STRPTR m_pLineBuf;
  BPTR m_pFile;
  SimpleString m_FileName;
};

#endif
