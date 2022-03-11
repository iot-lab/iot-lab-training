APPLICATION = Sunline

BOARD ?= b-l072z-lrwan1

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += ztimer_msec
USEMODULE += hts221
USEMODULE += xtimer

USEMODULE += lps331ap
USEMODULE += lsm303dlhc

USEMODULE += sx1276

USEPKG += semtech-loramac

LORA_REGION ?= EU868

RIOTBASE ?= $(CURDIR)/../riot/RIOT

include $(RIOTBASE)/Makefile.include