This is use signal wait function, it isn't callback function.
1. sigprocmask has three mode(SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK)
2. SIG_BLOCK can read old setting.
3. pthread_sigmask like sigprocmask
4. pthread_sigmaks for thread, sigprocmask for process.


