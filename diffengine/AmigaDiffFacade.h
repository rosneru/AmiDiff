#ifndef AMIGA_DIFF_FACADE_H
#define AMIGA_DIFF_FACADE_H

#include "SimpleString.h"

/**
 * Facade to hold all objects which are needed to perform a diff and
 * display its results
 *
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class AmigaDiffFacade
{
public:
  AmigaDiffFacade();

  void SetLeftFilePath(const char* p_pLeftFilePath);
  SimpleString LeftFilePath();

  void SetRightFilePath(const char* p_pRightFilePath);
  SimpleString RightFilePath();

private:
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;
};

#endif // AMIGA_DIFF_FACADE_H
