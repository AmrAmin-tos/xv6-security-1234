#include "types.h"
#include "user.h"

int
main(void)
{
  int r = audit_read();
  if(r < 0)
    printf(1, "Permission denied: admin only!\n");
  exit();
}

