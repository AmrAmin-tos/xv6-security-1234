#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAX_INPUT 32

void
getinput(char *buf, int max)
{
  int i = 0;
  char c;
  while(i < max - 1){
    if(read(0, &c, 1) <= 0) break;
    if(c == '\n' || c == '\r') break;
    buf[i++] = c;
  }
  buf[i] = '\0';
}

int
main(void)
{
  char uname[MAX_INPUT];
  char pass[MAX_INPUT];
  int uid;

  while(1){
    printf(1, "\n=== xv6 Medical Device Login ===\n");
    printf(1, "Username: ");
    getinput(uname, MAX_INPUT);

    printf(1, "Password: ");
    getinput(pass, MAX_INPUT);
    printf(1, "\n");

    uid = login(uname, pass);
    if(uid >= 0){
      printf(1, "Login successful! Welcome %s (uid=%d)\n", uname, uid);
int pid = fork();
if(pid == 0){
  setuid(uid);
  char *argv[] = { "sh", 0 };
  exec("sh", argv);
        exit();
      } else {
        // parent waits for shell to exit
        wait();
        printf(1, "\nSession ended. Please login again.\n");
      }
    } else {
      printf(1, "Login failed. Invalid credentials.\n");
    }
  }
  exit();
}
