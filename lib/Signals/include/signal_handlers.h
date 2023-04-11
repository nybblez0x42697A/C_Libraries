#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <signal.h>

typedef void (*func)(int signal);
typedef struct cleaner cleaner_t;
cleaner_t              clean;

typedef struct cleaner
{
    func    cleanup_f;
    void ** items;     // Array of cleanup functions
    int     num_items; // the number of cleanup functions
} cleaner_t;

typedef void (*SignalHandler)(int signum);

// Make all individual signal handler functions static
void handleSIGINT(int signal);
void handleSIGQUIT(int signal);
void handleSIGTSTP(int signal);
void handleSIGPIPE(int signal);
void handleSIGHUP(int signal);

void install_default_signal_handlers();
void install_signal_handler(int signal, SignalHandler handler);

atomic_bool terminate_shell;
int         terminate_key;

#endif
