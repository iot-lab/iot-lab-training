#include <stdio.h>

#include "shell.h"

/* Include threads header and define required macros here */

/* Implement thread functions here */

int main(void)
{
    /* Start new threads here */
    
    /* Start the shell here */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
