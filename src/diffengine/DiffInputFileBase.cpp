#include "DiffInputFileBase.h"

DiffInputFileBase::DiffInputFileBase(bool& isCancelRequested)
  : m_IsCancelRequested(isCancelRequested)
{

}

DiffInputFileBase::~DiffInputFileBase()
{

}


size_t DiffInputFileBase::numDigits(size_t number)
{
  size_t digits = 1;
  if ( number >= 100000000 ) { digits += 8; number /= 100000000; }
  if ( number >= 10000     ) { digits += 4; number /= 10000; }
  if ( number >= 100       ) { digits += 2; number /= 100; }
  if ( number >= 10        ) { digits += 1; }

  return digits;
}
