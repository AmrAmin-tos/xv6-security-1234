#include "types.h"
#include "param.h"
#include "user_db.h"
#include "defs.h"


#define NUSR 8

uint
passhash(const char *s)
{
  uint h = 5381;
  int c;
  while((c = (unsigned char)*s++))
    h = ((h << 5) + h) + c;
  return h;
}

struct user_entry user_table[NUSR] = {
  {"admin",   0, UID_ADMIN,   UID_ADMIN,   1},
  {"patient", 0, UID_PATIENT, UID_PATIENT, 1},
  {"doctor",  0, UID_DOCTOR,  UID_DOCTOR,  1},
};

void
userdb_init(void)
{
  user_table[0].passhash = passhash("admin123");
  user_table[1].passhash = passhash("patient123");
  user_table[2].passhash = passhash("doctor123");
}

int
userdb_login(const char *uname, const char *pass)
{
  uint h = passhash(pass);
  for(int i = 0; i < NUSR; i++){
    if(!user_table[i].active) continue;
    if(strncmp(user_table[i].uname, uname, MAX_USERNAME) == 0
       && user_table[i].passhash == h)
      return user_table[i].uid;
  }
  return -1;
}

int
userdb_getuid(const char *uname)
{
  for(int i = 0; i < NUSR; i++){
    if(user_table[i].active &&
       strncmp(user_table[i].uname, uname, MAX_USERNAME) == 0)
      return user_table[i].uid;
  }
  return -1;
}
