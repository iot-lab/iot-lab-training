# name of your application
APPLICATION = wakaama

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

# Import lps331ap driver module


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Specify the server IPv6 address (NOTE: Domain names not supported yet)
SERVER_ADDR ?= fd00:dead:beef::1

ifneq (,$(SERVER_ADDR))
  CFLAGS += -DLWM2M_SERVER_URI=\"coap://[$(SERVER_ADDR)]\"
endif

# Specify the device name
DEVICE_NAME ?= testRIOTDevice

ifneq (,$(DEVICE_NAME))
  CFLAGS += -DLWM2M_DEVICE_NAME=\"$(DEVICE_NAME)\"
endif

# Specify the device manufacturer
DEVICE_MANUFACTURER ?= Iot-LAB

ifneq (,$(DEVICE_NAME))
  CFLAGS += -DLWM2M_DEVICE_MANUFACTURER=\"$(DEVICE_MANUFACTURER)\"
endif

# NOTE: Add the package for wakaama
USEPKG += wakaama
# Uncomment to enable Wakaama debug log
#CFLAGS += -DLWM2M_WITH_LOGS

# Uncomment to indicate that the server is a LwM2M bootstrap server
# CFLAGS += -DLWM2M_BOOTSTRAP=1

# NOTE: Use wakaama in client mode
CFLAGS += -DLWM2M_CLIENT_MODE

include $(RIOTBASE)/Makefile.include
