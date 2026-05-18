#include "types.h"
#include "user.h"

int
main(void)
{
  int uid = whoami();
  printf(1, "current uid = %d\n", uid);
  exit();
}
