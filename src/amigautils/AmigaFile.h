#ifndef AMIGA_FILE_H
#define AMIGA_FILE_H

#include <vector>

#include <exec/types.h>
#include <libraries/dos.h>
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
   * Creates an open file with given name and given access mode.
   *
   * @param pFileName Name of the file to open.
   * @param accessMode One of MODE_NEWFILE, MODE_READWRITE or
   *        MODE_OLDFILE. DMODE_OLDFILE.
   */
  AmigaFile(const char* pFileName, ULONG accessMode = MODE_OLDFILE);
  virtual ~AmigaFile();

  /**
   * Counts the number of lines in file. The file has to be opened
   * before calling this method.
   *
   * NOTE After this call the file reading pointer will be reset to
   *      the start of the file.
   *
   * @returns
   * Number of lines in file. A value of '0' can mean that the file
   * isn't opened or that the number of lines = 0. Check the Open()
   * result to be sure.
   */
  ULONG CountLines();

  /**
   * Gets the file size in bytes. The file has  to be opened before
   * calling this method.
   *
   * NOTE: Changes the file handle position! After this operation the
   * file handle points to the start of the file.
   *
   * @returns
   * File size in bytes. A value of '0' can mean that the file isn't
   * opened or that the file size = 0. Check the Open() result to be
   * sure.
   */
  ULONG GetSize();

  /**
   * Reads the file line by line storing each line at the bottom of the
   * provided list.
   *
   * The lines are stored as dynamically created SimpleString objects,
   * so these objects have to be deleted manually after use.
   *
   * NOTE: This method reports its progress between 0 and 100 (%) if an
   * external ProgressReporter is added to this instance using the
   * SetProgressReporter() method,.
   *
   * @see SimpleString
   *
   * @param array
   * Array to store the lines
   */
  bool ReadLines(std::vector<SimpleString*>& linesVector);

  /**
   * Reads the next line from file
   *
   * @param line
   * Reference to a string variable to read into
   *
   * @returns
   * true when reading was successful or false if eof or file not open
   */
  bool ReadLine(SimpleString& line);

  char* ReadLine();

private:
  const ULONG MAX_LINE_LENGTH;
  STRPTR m_pLineBuf;
  BPTR m_FileDescriptor;
  SimpleString m_FileName;
};

#endif
