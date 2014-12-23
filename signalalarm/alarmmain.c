#include <stdio.h>
#include <string.h>
#include <linux/if_ether.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define SIGTIMER (SIGRTMAX)
#define pinfo(fmt, args...)		printf(fmt, ## args)

int timeup = 0, lost_count = 0;
static timer_t tid = (timer_t)-1;
void SignalHandler(int signo, siginfo_t * info, void* context)
{
	if (signo == SIGALRM)
	{
		alarm(0);
		pinfo("time up\n");
		timeup = 1;
	}
	else if(signo == SIGINT)
	{
		pinfo("Ctrl+c cached!\n");
		exit(1);
	}
}
void SignalHandler1(int signo) 
{
	if (signo == SIGALRM) 
	{
		pinfo("time up\n");
		timeup = 1;
	}
	else if (signo == SIGINT) 
	{
		timer_delete(tid);
		pinfo("Crtl+c cached!");
		exit(1);  // exit if CRTL/C is issued
	}
	else if (signo == SIGTIMER)
	{
		pinfo("POSIX time up\n");
		timeup = 1;
	}
	else
	{
		pinfo("signal=%d\n", signo);
	}
}

timer_t SetTimer(int signo, int sec, int mode)
{
	static struct sigevent sigev;
	static timer_t tid;
	static struct itimerspec itval;
	static struct itimerspec oitval;
 
	// Create the POSIX timer to generate signo
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = signo;
	sigev.sigev_value.sival_ptr = &tid;
 
	if (timer_create(CLOCK_REALTIME, &sigev, &tid) == 0) 
	{
		itval.it_value.tv_sec = sec / 1000;
		itval.it_value.tv_nsec = (long)(sec % 1000) * (1000000L);
 
		if (mode == 1) {
			itval.it_interval.tv_sec = itval.it_value.tv_sec;
			itval.it_interval.tv_nsec = itval.it_value.tv_nsec;
		} else {
			itval.it_interval.tv_sec = 0;
			itval.it_interval.tv_nsec = 0;
		}
 
		if (timer_settime(tid, 0, &itval, &oitval) != 0) {
			pinfo("time_settime error!");
		}
	} 
	else 
	{
		pinfo("timer_create error!");
		return (timer_t)-1;
	}
	return tid;
}


int main (void)
{
	struct sigaction sigact;
	
	/*set timer and CTRL+c signal*/
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	sigact.sa_sigaction = SignalHandler;
	if ((sigaction(SIGALRM, &sigact, NULL) == -1) || (sigaction(SIGINT, &sigact, NULL) == -1))
	{
		pinfo("sigaction failed");
		return -1;
	}
	/*set alarm signal*/	
	alarm(5);
	/*set POSIX timer signal*/
	/*signal(SIGTIMER, SignalHandler1);
	if(tid == (timer_t)-1)
		tid = SetTimer(SIGTIMER, 3000, 1);*/
		
	while(1)
	{
		if(timeup == 1)
		{
			/*if(tid == (timer_t)-1)
				tid = SetTimer(SIGTIMER, 5000, 1);*/
				
			alarm(5);
			timeup = 0;
		}
	}
	return 0;
}
