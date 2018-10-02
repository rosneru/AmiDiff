#include "DiffEngine.h"



bool DiffEngine::Diff(DiffFilePartition* p_File1Src,
                      DiffFilePartition* p_File2Src,
                      DiffFilePartition* p_File1Diff,
                      DiffFilePartition* p_File2Diff)
{
  size_t nf2CurrentLine;
  size_t i;

  while(i < p_File1Src->NumberOfLines())
  {
    // process this line
    size_t nLinef2;
    if(p_File1Src->MatchLine(i, p_File2Src, nLinef2))
    {
      // matched, either lines were identical or f2 has added something
      if (nLinef2 > nf2CurrentLine)
      {
         // add blank lines to f1_bis
         long j = nLinef2 - nf2CurrentLine;
         while ( j>0 )
         {
           p_File1Diff->AddBlankLine();
//           p_File2Diff->AddString( f2.GetRawLine(nLinef2-j), Added );

           j--;
         }
      }
    }
  }
}
