#include "types.h"
#include "user.h"
#include "fcntl.h"

int
main(void)
{
  int fd;

  fd = open("patient/records", O_CREATE | O_RDWR);
  if(fd >= 0){
    write(fd, "Patient medical records\n", 24);
    close(fd);
    chown("patient/records", 1);
    chmod("patient/records", 1);
    printf(1, "Created patient/records\n");
  }

  fd = open("dosage/insulin.log", O_CREATE | O_RDWR);
  if(fd >= 0){
    write(fd, "Insulin dosage log\n", 19);
    close(fd);
    chown("dosage/insulin.log", 2);
    chmod("dosage/insulin.log", 3);
    printf(1, "Created dosage/insulin.log\n");
  }

  fd = open("device/config", O_CREATE | O_RDWR);
  if(fd >= 0){
    write(fd, "Device configuration\n", 21);
    close(fd);
    chown("device/config", 0);
    chmod("device/config", 3);
    printf(1, "Created device/config\n");
  }

  fd = open("audit/syscall.log", O_CREATE | O_RDWR);
  if(fd >= 0){
    write(fd, "Audit log\n", 10);
    close(fd);
    chown("audit/syscall.log", 0);
    chmod("audit/syscall.log", 3);
    printf(1, "Created audit/syscall.log\n");
  }

  printf(1, "Medical filesystem setup complete!\n");
  exit();
}
