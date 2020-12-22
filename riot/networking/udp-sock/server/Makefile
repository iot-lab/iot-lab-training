APPLICATION = sock_server

BOARD ?= iotlab-m3

RIOTBASE ?= $(CURDIR)/../../../RIOT

SERVER_PORT ?= 8888

# include this for printing IP addresses
USEMODULE += shell_commands
USEMODULE += xtimer

# Import radio driver module
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

# Import network modules here


DEVELHELP ?= 1

CFLAGS += -DSERVER_PORT=$(SERVER_PORT)

include $(RIOTBASE)/Makefile.include