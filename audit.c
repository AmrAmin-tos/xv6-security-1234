#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "user_db.h"

#define AUDIT_SIZE 128

struct audit_entry {
  int  pid;
  int  uid;
  uint tick;
  char event[32];
};

static struct audit_entry audit_buf[AUDIT_SIZE];
static int audit_head = 0;
static struct spinlock audit_lock;

void
audit_init(void)
{
  initlock(&audit_lock, "audit");
}

void
audit_log(int pid, int uid, uint tick, const char *event)
{
  acquire(&audit_lock);
  struct audit_entry *e = &audit_buf[audit_head % AUDIT_SIZE];
  e->pid  = pid;
  e->uid  = uid;
  e->tick = tick;
  strncpy(e->event, event, 31);
  e->event[31] = '\0';
  audit_head++;
  release(&audit_lock);
}

void
audit_print(void)
{
  for(int i = 0; i < audit_head && i < AUDIT_SIZE; i++){
    struct audit_entry *e = &audit_buf[i];
    cprintf("pid=%d uid=%d tick=%d event=%s\n",
      e->pid, e->uid, e->tick, e->event);
  }
}

int
audit_read(char *buf, int max)
{
  int uid = get_current_uid();
  if(uid != UID_ADMIN)
    return -1;
  audit_print();
  return 0;
}
