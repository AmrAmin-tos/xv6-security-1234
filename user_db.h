#ifndef USER_DB_H
#define USER_DB_H

#define MAX_USERS     8
#define MAX_USERNAME  16

#define UID_ADMIN    0
#define UID_PATIENT  1
#define UID_DOCTOR   2

#define NUSR         8

struct user_entry {
  char  uname[MAX_USERNAME];
  uint  passhash;
  int   uid;
  int   gid;
  int   active;
};

#endif
