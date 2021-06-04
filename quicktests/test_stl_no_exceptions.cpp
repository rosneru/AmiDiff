#include <stdio.h>
#include <vector>
#include <string>

int main()
{
  std::vector<std::string*> myVector;
  myVector.push_back(new std::string("one is not two"));
  myVector.push_back(new std::string("4"));
  myVector.push_back(new std::string("3rd line"));

  // Output the vectors items
  std::vector<std::string*>::iterator it;
  for(it = myVector.begin(); it != myVector.end(); it++)
  {
    printf("%s \n", (*it)->c_str());
    delete (*it);
  }

  printf("\n");

  return 0;
}
