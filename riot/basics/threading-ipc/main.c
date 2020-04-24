#include <stdio.h>
#include <string.h>

#include "shell.h"

/* TODO: add thread and message includes here */



/* TODO: add the thread pid variable and the thread stack array here */



/* TODO: implement the receiver thread here */



static int send_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }

    /* TODO: send the message to the thread here */


    return 0;
}

static const shell_command_t commands[] = {
    { "send", "send a message via ipc", send_command },
    { NULL, NULL, NULL}
};

int main(void)
{
    puts("RIOT application with thread IPC");

    /* TODO: create the listener thread here */

 

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
