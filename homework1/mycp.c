#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#define BUFSIZ 20

int
main(int argc, char *argv[])
{
  int fd_read, fd_write, bytes_read, reason;
  char buf[BUFSIZ], input[2];

  /*dealing with program parameters*/
  if (argc != 3) {
    fprintf(stderr, "mycp: Format error. Usage: mycp <src_file> <dest_file>\n\n");
    exit(1);
  }
 
  /*try to open the source file*/
  if ((fd_read = open(argv[1], O_RDONLY, 0)) == -1) {
    fprintf(stderr, "mycp: Cannot open file %s\n", argv[1]);
    reason = access(argv[1], R_OK);
    if (errno == EACCES) 
      fprintf(stderr, "mycp: Permission denied for reading file %s\n", argv[1]);
    exit(1);
  }

  /*try to open the dest file*/
  if ((fd_write = open(argv[2], O_WRONLY, 0)) != -1 || errno == EACCES) {
    while(1) {
      printf("mycp: There is already a file named %s in this directory! \n  Overwrite? <Y/N>: ", argv[2]);
      gets(input);
      if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0) {
        /*start to overwrite!*/
        while((bytes_read = read(fd_read, buf, BUFSIZ)) > 0 ) {
          if (write(fd_write, buf, bytes_read) != bytes_read) {
            fprintf(stderr, "mycp: Write error into file %s!\n", argv[2]);
            reason = access(argv[2], W_OK);
            if (errno == EACCES) {
              fprintf(stderr, "mycp: Permission denied when writing to file %s!\n", argv[2]);
              reason = close(fd_read);
              reason = close(fd_write);
              exit(1);
            }
	    if (errno == EIO) {
              fprintf(stderr, "mycp: I/O error.\n");
              reason = close(fd_read);
              reason = close(fd_write);
              exit(1);
            }
          }
        }
        struct stat stat_buf;
        reason = fstat(fd_read, &stat_buf);
        reason = fchmod (fd_write, stat_buf.st_mode);
        reason = close(fd_read);
        reason = close(fd_write);
        return 0;
      }
      else if (strcmp(input, "n") == 0 || strcmp (input, "N") == 0) {
        reason = close(fd_read);
        reason = close(fd_write);
        return 0;
      }
      else {
        puts("Invalid input!\n");
      }
    }  
  }

  /*dest file doesn't exist, creating new one*/
  else {
    if ((fd_write = creat(argv[2], 0664)) == -1) {
      fprintf(stderr, "mycp: Cannot create new file %s!\n", argv[2]);
      reason = access(argv[2], W_OK);
      if (errno == EACCES)
        fprintf(stderr, "mycp: Permission denied when writing to file %s!\n", argv[2]);
      reason = close(fd_read);
      reason = close(fd_write);
      exit(1);
    }
    while((bytes_read = read(fd_read, buf, BUFSIZ)) > 0) {
      if (write(fd_write, buf, bytes_read) != bytes_read) {
        fprintf(stderr, "mycp: Write error into file %s!\n", argv[2]);
        reason = close(fd_read);
        reason = close(fd_write);
        exit(1);
      }
    }
    struct stat stat_buf;
    reason = fstat(fd_read, &stat_buf);
    reason = fchmod (fd_write, stat_buf.st_mode);
    reason = close(fd_read);
    reason = close(fd_write);
    return 0;
  }
}
