#include "DiffFileSearchResult.h"

DiffFileSearchResult::DiffFileSearchResult(Location location, 
                                           size_t lineId, 
                                           size_t charId)
  : m_Location(location),
    m_LineId(lineId),
    m_CharId(charId)
{

}

DiffFileSearchResult::~DiffFileSearchResult()
{

}

DiffFileSearchResult::Location DiffFileSearchResult::getLocation() const
{
  return m_Location;
}


size_t DiffFileSearchResult::getLineId() const
{
  return m_LineId;
}


size_t DiffFileSearchResult::getCharId() const
{
  return m_CharId;
}
