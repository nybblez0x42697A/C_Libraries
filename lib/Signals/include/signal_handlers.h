#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (
    *SignalHandler)(
    int);
void
install_default_signal_handlers();
void
install_default_keyboard_listener(void * arg);
void
install_signal_handler(
    int signal,
    SignalHandler);

#endif
