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
  DiffEngineAmiga(DiffFileBase& a,
                  DiffFileBase& b,
                  DiffFileBase& aDiff,
                  DiffFileBase& bDiff,
                  APTR& pPoolHeader,
                  bool& bCancelRequested,
                  LinkedList* pDiffStartIdxsList);

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
