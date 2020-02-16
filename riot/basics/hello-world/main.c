#include <stdio.h>
#include "xtimer.h"

int main(void)
{
    xtimer_sleep(5);
    puts("Hello from RIOT!");
    return 0;
}
