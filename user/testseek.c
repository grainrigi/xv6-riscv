#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int check_panic(int result, const char *msg) {
  if (result == -1) {
    fprintf(2, "%s\n", msg);
    exit(-1);
    return -1;
  }
  return 0;
}

int main(void) {
  int fd;
  char buf = 100;

  check_panic((fd = open("callsign.txt", O_RDWR|O_CREATE|O_TRUNC)), "open failed for write");

  // first, write down JA1YAD
  check_panic(write(fd, "JA1YAD", 6), "write JA1YAD failed");

  // next, change the area number to 6 (JA1YAD -> JA6YAD)
  check_panic(lseek(fd, 2, SEEK_SET), "seek to area number failed");
  check_panic(write(fd, "6", 1), "change area number failed");

  // next, change the suffix to xZx (JA6YAD -> JA6YZD)
  check_panic(lseek(fd, 1, SEEK_CUR), "seek to suffix failed");
  check_panic(write(fd, "Z", 1), "change suffix failed");

  // next, add portable area number (JA6YZD -> JA6YZD'\0'1)
  check_panic(lseek(fd, 1, SEEK_END), "seek beyond the file end failed");
  check_panic(write(fd, "1", 1), "add portable area number failed");

  // check if the file is extended correctly
  check_panic(lseek(fd, -2, SEEK_CUR), "seek to the extended part failed");
  check_panic(read(fd, &buf, 1), "read extended part failed");
  if (buf != 0)
    check_panic(-1, "extended part contains invalid data");

  // finally, add portable (JA6YZD'\0'1 -> JA6YZD/1)
  check_panic(lseek(fd, -2, SEEK_END), "seek to the extended part for write failed");
  check_panic(write(fd, "/", 1), "add portable failed");

  close(fd);

  // dump the result
  char *argv[3] = { "cat", "callsign.txt", 0 };
  exec("cat", argv);

  return 0;
}