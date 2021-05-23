#ifndef SEARCH_OPTIONS_H
#define SEARCH_OPTIONS_H

enum SearchLocation
{
  SL_BothFiles,
  SL_LeftFile,
  SL_RightFile
};

enum StartSearchFrom
{
  SF_CurrentPage,
  SF_DocumentTop,
};

enum SearchDirection
{
  SD_Downward,
  SD_Upward,
};

#endif
