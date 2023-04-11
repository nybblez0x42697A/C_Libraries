#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include "../include/signal_handlers.h"

void
handleSIGINT(int signal)
{
    (void)signal;
    const char msg[] = "Received SIGINT (Ctrl+C)\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    // clean.cleanup_f(signal);
}

void
handleSIGQUIT(int signal)
{
    (void)signal;
    const char msg[] = "\nReceived SIGQUIT (Ctrl+\\)\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    clean.cleanup_f(signal);
    // exit(0);
}

void
handleSIGTSTP(int signal)
{
    (void)signal;
    printf("\nReceived SIGTSTP (Ctrl+Z)\n");
}

SignalHandler
registerSignalHandler(int signal, SignalHandler handler)
{
    struct sigaction newAction;
    memset(&newAction, 0, sizeof(struct sigaction));
    struct sigaction oldAction;
    memset(&oldAction, 0, sizeof(struct sigaction));
    newAction.sa_handler = handler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_RESTART;

    if (sigaction(signal, &newAction, &oldAction) < 0)
    {
        return SIG_ERR;
    }

    return oldAction.sa_handler;
}

void
install_default_signal_handlers()
{
    registerSignalHandler(SIGINT, handleSIGINT);
    registerSignalHandler(SIGQUIT, handleSIGQUIT);
    registerSignalHandler(SIGTSTP, handleSIGTSTP);
    // registerSignalHandler(SIGPIPE, handleSIGPIPE);
    // registerSignalHandler(SIGHUP, handleSIGHUP);
}

void
install_signal_handler(int signal, SignalHandler handler)
{
    registerSignalHandler(signal, handler);
}

#ifndef _MAIN_EXCLUDED
int
main()
{

    int default_handlers_installed = 0;
    if (!default_handlers_installed)
    {
        install_default_signal_handlers();
        default_handlers_installed = 1;
    }
}
#endif
