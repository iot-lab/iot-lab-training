APPLICATION = posix_server

BOARD ?= iotlab-m3

RIOTBASE ?= $(CURDIR)/../../../RIOT

SERVER_PORT ?= 8888

# Import ztimer module
USEMODULE += ztimer_msec

# Import radio driver module
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Import network modules
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp

# Import posix sockets


DEVELHELP ?= 1

CFLAGS += -DSERVER_PORT=$(SERVER_PORT)

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
