#ifndef SIG_H
#define SIG_H

  typedef int sig_t;
  #define SIG_CONT 0
  #define SIG_TERM 1
  #define SIG_STOP 2
  #define SIG_KILL 9

  #define NSIG 32
  typedef void (*sighandler_t)(int);
  

#endif // SIG_H