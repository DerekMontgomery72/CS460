#include "type.h"
#include "ucode.c"


int main(int argc, char *argv[])
{
  if(argc == 1)
  {
    chdir("/");
  }
  else{
    chdir(argv[1]);
  }
  return;
}