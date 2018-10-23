#ifndef COMMAND_H
#define COMMAND_H

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
class Command
{
public:
  virtual ~Command();
  virtual void Execute() = 0;

protected:
  Command();
};

#endif // COMMAND_H
