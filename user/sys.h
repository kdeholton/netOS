#ifndef _SYS_H_
#define _SYS_H_

extern long exit(long status);
extern long execv(int argc, char** args);
extern long open(char *name);
extern long getlen(long);
extern long close(long);
extern long read(long f, void* buf, long len);
extern long seek(long f, long pos);
extern long putchar(int c);
extern long getchar();
extern long semaphore(long n);
extern long up(long sem);
extern long down(long sem);
extern long fork();
extern long join(long proc);
extern long shutdown();
extern long clear();
extern long sleep(int amt);
extern long write(long f, void* buf, long len);
extern long moveToZero();
extern long putcolor(char c, long bg, long fg);
extern long getRow();
extern long getColumn();
extern void setCursor(int x, int y);
extern void decrementOnlyRow();

#endif
