#ifndef COMMAND_H
#define COMMAND_H

#include <intuition/intuition.h>

#include "Array.h"
#include "WindowBase.h"

/**
 * Base class of all commands. Is an abstract class because of the
 * abstract Execute() method.
 *
 * Derived concrete command objects are mainly wired to the app menu.
 * On menu pick event the Execute() method is called.
 *
 * @author Uwe Rosner
 * @date 24/09/2018
 */
class CommandBase
{
public:
  CommandBase(Array<WindowBase*>& windowArray);
  virtual ~CommandBase();
  virtual void Execute(struct Window* pActiveWindow) = 0;

  void DisableInAllWindowMenus() const;
  void EnableInAllWindowMenus() const;

protected:
  CommandBase();

private:
    Array<WindowBase*>& m_Windows;
};

#endif
