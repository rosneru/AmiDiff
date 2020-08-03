#ifndef DIFF_INPUT_FILE_LINUX_H
#define DIFF_INPUT_FILE_LINUX_H


#include "DiffInputFileBase.h"
#include "ProgressReporter.h"

/**
 *
 * @author Uwe Rosner
 * @date 18/07/2020
 */
class DiffInputFileLinux : public DiffInputFileBase
{
public:
  DiffInputFileLinux(bool& bCancelRequested, 
                     ProgressReporter& progress,
                     const char* pProgressDescription,
                     const char* pFileName,
                     bool lineNumbersEnabled);

  virtual ~DiffInputFileLinux();


private:
  char* m_pFileBuffer;
  void collectLineNumbers(size_t maxNumLines);
};

#endif
