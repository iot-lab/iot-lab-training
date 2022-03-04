APPLICATION = Projet IOT ISENP18M

BOARD ?= iotlab-m3

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ztimer
USEMODULE += ztimer_msec

# Add the required modules here
USEMODULE += lps331ap
USEMODULE += lsm303dlhc
USEMODULE += isl29020
USEMODULE += l3g4200d
CFLAGS=-DAUTO_INIT_LED0


RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include