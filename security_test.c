#include "types.h"
#include "user.h"
#include "fcntl.h"

int passed = 0;
int failed = 0;

void
test(char *name, int result, int expected)
{
  if(result == expected){
    printf(1, "[PASS] %s\n", name);
    passed++;
  } else {
    printf(1, "[FAIL] %s (got %d expected %d)\n", name, result, expected);
    failed++;
  }
}

int
main(void)
{
  int fd;

  printf(1, "\n========================================\n");
  printf(1, "  xv6 Medical Device Security Test Suite\n");
  printf(1, "  FDA / IEC 62443 Compliance Report\n");
  printf(1, "========================================\n\n");

  // ── PHASE 1 TESTS ──────────────────────────────
  printf(1, "--- Phase 1: Authentication Tests ---\n");

  // T1: valid admin login
  test("T01: admin login success",
       login("admin", "admin123"), 0);

  // T2: wrong password rejected
  test("T02: wrong password rejected",
       login("admin", "wrongpass"), -1);

  // T3: unknown user rejected
  test("T03: unknown user rejected",
       login("hacker", "hack123"), -1);

  // T4: patient login
  test("T04: patient login success",
       login("patient", "patient123"), 1);

  // T5: doctor login
  test("T05: doctor login success",
       login("doctor", "doctor123"), 2);

  // T6: whoami returns correct uid
  setuid(0);
  test("T06: whoami returns uid=0 after setuid",
       whoami(), 0);

  // ── PHASE 2 TESTS ──────────────────────────────
  printf(1, "\n--- Phase 2: File Permission Tests ---\n");

  // Setup as admin
  setuid(0);
  mkdir("patient");
  mkdir("dosage");
  mkdir("device");
  mkdir("audit");
  int pid = fork();
  if(pid == 0){
    char *argv[] = { "setup_medical", 0 };
    exec("setup_medical", argv);
    exit();
  }
  wait();
  // T7: admin reads patient/records
  fd = open("patient/records", O_RDONLY);
  test("T07: admin reads patient/records", fd >= 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T8: admin reads device/config
  fd = open("device/config", O_RDONLY);
  test("T08: admin reads device/config", fd >= 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T9: patient reads own records
  setuid(1);
  fd = open("patient/records", O_RDONLY);
  test("T09: patient reads patient/records", fd >= 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T10: patient blocked from device/config
  fd = open("device/config", O_RDONLY);
  test("T10: patient blocked from device/config", fd < 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T11: patient blocked from dosage/insulin.log
  fd = open("dosage/insulin.log", O_RDONLY);
  test("T11: patient blocked from dosage/insulin.log", fd < 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T12: doctor reads dosage/insulin.log
  setuid(2);
  fd = open("dosage/insulin.log", O_RDONLY);
  test("T12: doctor reads dosage/insulin.log", fd >= 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T13: doctor blocked from device/config
  fd = open("device/config", O_RDONLY);
  test("T13: doctor blocked from device/config", fd < 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // T14: doctor writes dosage/insulin.log
  setuid(2);
  fd = open("dosage/insulin.log", O_RDWR);
  test("T14: doctor writes dosage/insulin.log", fd >= 0 ? 0 : -1, 0);
  if(fd >= 0) close(fd);

  // ── PHASE 3 TESTS ──────────────────────────────
  printf(1, "\n--- Phase 3: Audit Log Tests ---\n");

  // T15: admin can read audit log
  setuid(0);
  test("T15: admin reads audit log",
       audit_read(), 0);

  // T16: patient cannot read audit log
  setuid(1);
  test("T16: patient blocked from audit log",
       audit_read(), -1);

  // T17: doctor cannot read audit log
  setuid(2);
  test("T17: doctor blocked from audit log",
       audit_read(), -1);

  // ── REPORT ─────────────────────────────────────
  printf(1, "\n========================================\n");
  printf(1, "  COMPLIANCE REPORT\n");
  printf(1, "========================================\n");
  printf(1, "  Total Tests : %d\n", passed + failed);
  printf(1, "  Passed      : %d\n", passed);
  printf(1, "  Failed      : %d\n", failed);
  printf(1, "  Status      : %s\n",
    failed == 0 ? "COMPLIANT - PASS" : "NON-COMPLIANT - FAIL");
  printf(1, "========================================\n\n");

  exit();
}
