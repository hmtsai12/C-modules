#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

void signalHandle(int sig)
{
	printf("caught signal %d\n", sig);
	switch (sig)
	{
		case SIGUSR1:
			break;
		case SIGALRM:
			break;
		case SIGINT:
			exit(0);
			break;
		case SIGTERM:
			break;
		default:
			break;
	}
}

void signalTimerStart(int msec)
{
	struct itimerval itv;
	itv.it_interval.tv_sec = msec/1000;
	itv.it_interval.tv_usec = (msec%1000)*1000;
	itv.it_value = itv.it_interval;
	setitimer(ITIMER_REAL, &itv, NULL);
	return ;
}

void signalTimerStop(void)
{
	struct itimerval itv;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value = itv.it_interval;
	setitimer(ITIMER_REAL, &itv, NULL);
	return ;
}

void registerSignal(sigset_t *sigsToCatch)
{
	sigemptyset(sigsToCatch);
	sigaddset(sigsToCatch, SIGINT);
	sigaddset(sigsToCatch, SIGUSR1);
	sigaddset(sigsToCatch, SIGALRM);
	sigaddset(sigsToCatch, SIGTERM);
	sigprocmask(SIG_BLOCK, sigsToCatch, NULL);
}

int waitSignal(sigset_t *sigsToCatch)
{
	int signum;
	sigwait(sigsToCatch, &signum);
	return signum;
}


int main()
{
	sigset_t sig;
	int retSig;
	registerSignal(&sig);
	signalTimerStart(2000);
	while(1)
	{
		retSig = waitSignal(&sig);
		signalHandle(retSig);	
	}
	return 0;
}
