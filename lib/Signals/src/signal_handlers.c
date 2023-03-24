#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "../include/signal_handlers.h"

//  Global atomic boolean flag to terminate.
atomic_bool terminate_shell = ATOMIC_VAR_INIT(false);


// Make all individual signal handler functions static
static void handleSIGINT(int signal);
static void handleSIGQUIT(int signal);
static void handleSIGTSTP(int signal);
static void handleSIGPIPE(int signal);
static void handleSIGHUP(int signal);

static void
handleSIGINT(int signal)
{
    (void)signal;
    printf("\nReceived SIGINT (Ctrl+C)\n");
    exit(0);
}

static void
handleSIGQUIT(int signal)
{
    (void)signal;
    printf("\nReceived SIGQUIT (Ctrl+\\)\n");
    exit(0);
}

static void
handleSIGTSTP(int signal)
{
    (void)signal;
    printf("\nReceived SIGTSTP (Ctrl+Z)\n");
}

static void
handleSIGPIPE(int signal)
{
    (void)signal;
    printf("\nReceived SIGPIPE\n");
    exit(0);
}

static void
handleSIGHUP(int signal)
{
    (void)signal;
    printf("\nReceived SIGHUP\n");
    exit(0);
}

SignalHandler
registerSignalHandler(int signal, SignalHandler handler)
{
    struct sigaction newAction, oldAction;
    newAction.sa_handler = handler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_RESTART;

    if (sigaction(signal, &newAction, &oldAction) < 0)
    {
        return SIG_ERR;
    }

    return oldAction.sa_handler;
}

void* keyboard_listener(void * arg) 
{
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (!atomic_load(&terminate_shell)) {
        int ch = getchar();

        // Check for Esc key (27 is the ASCII code for Esc)
        if (ch == 27) {
            atomic_store(&terminate_shell, true);
        }
        (void)arg;
        // if (ch == (*(char*)arg))
        // {
        //     atomic_store(&terminate_shell, true);
        // }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return NULL;
}

void install_default_keyboard_listener(void * arg) {
    pthread_t input_tid;

    // Start the input thread
    pthread_create(&input_tid, NULL, keyboard_listener, arg);

    // Detach the input thread so that it can be cleaned up automatically when it exits
    pthread_detach(input_tid);
}


void
install_default_signal_handlers()
{
    registerSignalHandler(SIGINT, handleSIGINT);
    registerSignalHandler(SIGQUIT, handleSIGQUIT);
    registerSignalHandler(SIGTSTP, handleSIGTSTP);
    registerSignalHandler(SIGPIPE, handleSIGPIPE);
    registerSignalHandler(SIGHUP, handleSIGHUP);
}

void
installSignalHandler(int signal, SignalHandler handler)
{

    registerSignalHandler(signal, handler);
}

#ifdef _MAIN_EXCLUDED
int
main()
{
    static int default_handlers_installed = 0;
    if (!default_handlers_installed)
    {
        install_default_signal_handlers();
        install_default_keyboard_listener(NULL);
        default_handlers_installed = 1;
    }
}
#endif
