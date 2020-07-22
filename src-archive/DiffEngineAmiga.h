#ifndef DIFFENGINE_AMIGA_H
#define DIFFENGINE_AMIGA_H

#include <exec/types.h>

#include "DiffEngine.h"


/**
 * Extends the DiffEngine to use an Amiga memory pool for allocations.
 *
 *
 * @author Uwe Rosner
 * @date 17/12/2019
 */
class DiffEngineAmiga : public DiffEngine
{
public:
  DiffEngineAmiga(DiffFileBase* pA,
                  DiffFileBase* pB,
                  DiffFileBase* pADiff,
                  DiffFileBase* pBDiff,
                  APTR& pPoolHeader,
                  bool& bCancelRequested,
                  std::vector<size_t>& diffIndices);

  ~DiffEngineAmiga();

  void Clear();

  const char* Error();

private:
  APTR& m_pPoolHeader;
  const char* m_pErrMsgLowMem;
  const char* m_pErrMsgMemPool;
  const char* m_pErrMsgUnknown;
  const char* m_pError;

  void addDiffIdxToList(size_t diffIdx);
};

#endif // DIFFENGINE_AMIGA_H
