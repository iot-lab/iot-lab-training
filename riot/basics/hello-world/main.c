#include <stdio.h>
#include "ztimer.h"

int main(void)
{
    ztimer_sleep(ZTIMER_SEC, 5);
    puts("Hello from RIOT!");
    return 0;
}
