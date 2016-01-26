#include <stdio.h>
#include "info.h"

int
main (void)
{
  list_init();
  routine();
  before_exit();
  return 0;
}
