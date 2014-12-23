This is use signal wait function, it isn't callback function.
1. sigprocmask has three mode(SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK)
2. SIG_BLOCK can read old setting.
3. pthread_sigmask like sigprocmask
4. pthread_sigmaks for thread, sigprocmask for process.

alarmmain.c
1. use POSIX timer or system alarm function
2. because POSIX timer function, it compiler need -lrt
