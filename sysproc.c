#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "user_db.h"
int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int
sys_whoami(void)
{
  struct proc *p = myproc();
  cprintf("uid=%d\n", p->uid);
  return p->uid;
}

int
sys_useradd(void)
{
  char *uname, *pass;
  int uid;
  if(argstr(0, &uname) < 0 || argstr(1, &pass) < 0 || argint(2, &uid) < 0)
    return -1;
  if(myproc()->uid != UID_ADMIN)
    return -1;  // only admin can add users
  for(int i = 0; i < NUSR; i++){
    if(!user_table[i].active){
      strncpy(user_table[i].uname, uname, MAX_USERNAME);
      user_table[i].passhash = passhash(pass);
      user_table[i].uid  = uid;
      user_table[i].gid  = uid;
      user_table[i].active = 1;
      return 0;
    }
  }
  return -1;  // table full
}

int
sys_userdel(void)
{
  char *uname;
  if(argstr(0, &uname) < 0)
    return -1;
  if(myproc()->uid != UID_ADMIN)
    return -1;  // only admin can delete users
  for(int i = 0; i < NUSR; i++){
    if(user_table[i].active &&
       strncmp(user_table[i].uname, uname, MAX_USERNAME) == 0){
      user_table[i].active = 0;
      return 0;
    }
  }
  return -1;  // user not found
}

int
sys_passwd(void)
{
  char *uname, *newpass;
  if(argstr(0, &uname) < 0 || argstr(1, &newpass) < 0)
    return -1;
  struct proc *p = myproc();
  // only admin or the user themselves can change password
  int target_uid = userdb_getuid(uname);
  if(p->uid != UID_ADMIN && p->uid != target_uid)
    return -1;
  for(int i = 0; i < NUSR; i++){
    if(user_table[i].active &&
       strncmp(user_table[i].uname, uname, MAX_USERNAME) == 0){
      user_table[i].passhash = passhash(newpass);
      return 0;
    }
  }
  return -1;
}
int
sys_login(void)
{
  char *uname, *pass;
  if(argstr(0, &uname) < 0 || argstr(1, &pass) < 0)
    return -1;
  int uid = userdb_login(uname, pass);
  if(uid >= 0){
    myproc()->uid = uid;
    audit_log(myproc()->pid, uid, ticks, "login_success");
  } else {
    audit_log(myproc()->pid, -1, ticks, "login_failed");
  }
  return uid;
}

int
sys_setuid(void)
{
  int uid;
  if(argint(0, &uid) < 0)
    return -1;
  myproc()->uid = uid;
  return 0;
}
int
sys_chmod(void)
{
  char *path;
  int mode;
  struct inode *ip;
  if(argstr(0, &path) < 0 || argint(1, &mode) < 0)
    return -1;
  begin_op();
  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  // only owner or admin can chmod
  if(myproc()->uid != UID_ADMIN && myproc()->uid != ip->uid){
    iunlockput(ip);
    end_op();
    return -1;
  }
  ip->mode = mode;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_chown(void)
{
  char *path;
  int uid;
  struct inode *ip;
  if(argstr(0, &path) < 0 || argint(1, &uid) < 0)
    return -1;
  // only admin can chown
  if(myproc()->uid != UID_ADMIN)
    return -1;
  begin_op();
  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  ip->uid = uid;
  ip->gid = uid;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_audit_read(void)
{
  if(myproc()->uid != UID_ADMIN){
    cprintf("Permission denied: admin only!\n");
    return -1;
  }
  audit_print();
  return 0;
}
