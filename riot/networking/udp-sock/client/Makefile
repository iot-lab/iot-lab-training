APPLICATION = sock_client

BOARD ?= iotlab-m3

RIOTBASE ?= $(CURDIR)/../../../RIOT

SERVER_PORT ?= 8888
SERVER_ADDR ?= fe80::1

USEMODULE += ztimer_msec

# Import radio driver module
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

# Import network modules here


DEVELHELP ?= 1

CFLAGS += -DSERVER_PORT=$(SERVER_PORT)
CFLAGS += -DSERVER_ADDR=\"$(SERVER_ADDR)\"

include $(RIOTBASE)/Makefile.include