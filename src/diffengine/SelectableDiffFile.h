#ifndef SELECTABLE_DIFF_FILE_H
#define SELECTABLE_DIFF_FILE_H

#include "DiffFileBase.h"
#include "TextSelection.h"

/**
 * Adds the ability to select [0; n] blocks of text to an existing
 * DiffFile.
 *
 * @author Uwe Rosner
 * @date 30/12/2020
 */
class SelectableDiffFile
{
public:
  SelectableDiffFile(DiffFileBase& diffFile);

  DiffLine* operator[](unsigned long index);

  unsigned long getNumLines();

  void addSelection(unsigned long fromLine, 
                    unsigned long toLine, 
                    unsigned long fromColumn, 
                    unsigned long toColumn);

private:
  DiffFileBase& m_DiffFile;
  TextSelection m_TextSelection;
};

#endif
