# name of your application
APPLICATION = Projet_IOT

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

# Default to using ethos for providing the uplink when not on native
UPLINK ?= ethos

# Check if the selected Uplink is valid
ifeq (,$(filter ethos slip cdc-ecm wifi ethernet,$(UPLINK)))
  $(error Supported uplinks are `ethos`, `slip`, `cdc-ecm`, `ethernet` and `wifi`)
endif

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Specify the mandatory networking module for a IPv6 routing node
USEMODULE += gnrc_ipv6_router_default
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_udp_cmd
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl


# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
# USEMODULE += sock_dns              # include DNS client
# USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Instead of simulating an Ethernet connection, we can also simulate
# an IEEE 802.15.4 radio using ZEP
USE_ZEP ?= 0

# set the ZEP port for native
ZEP_PORT_BASE ?= 17754
ifeq (1,$(USE_ZEP))
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
endif

# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
#IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
#CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

# Uncomment this to join RPL DODAGs even if DIOs do not contain
# DODAG Configuration Options (see the doc for more info)
# CFLAGS += -DCONFIG_GNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# When using a regular network uplink we should use DHCPv6
ifneq (,$(filter cdc-ecm wifi ethernet,$(UPLINK)))
  USE_DHCPV6 ?= 1
else
  USE_DHCPV6 ?= 0
endif

ifeq (1,$(USE_DHCPV6))
  # include DHCPv6 client for 6LoWPAN border router
  USEMODULE += gnrc_dhcpv6_client_6lbr
else
  # include UHCP client
  USEMODULE += gnrc_uhcpc
endif

# Ethos/native TAP interface and UHCP prefix can be configured from make command
TAP ?= tap0
ifeq (1,$(USE_DHCPV6))
# with DHCPv6 the 64-bit delegated prefixes are generated from a shorter
# configured prefix.
IPV6_PREFIX ?= 2001:db8::/32
else
# UHCP advertises the prefix as is.
IPV6_PREFIX ?= 2001:db8::/64
endif


# MODULE DEPENDENT CONFIGURATION IMPORT
# =====================================
# use ethos (ethernet over serial) or SLIP (serial-line IP) for network
# communication and stdio over UART, but not on native, as native has a tap
# interface towards the host.
ifeq (,$(filter native,$(BOARD)))
  ifeq (slip,$(UPLINK))
    # SLIP baudrate and UART device can be configured from make command
    SLIP_BAUDRATE ?= 115200
    include $(CURDIR)/../RIOT/examples/gnrc_border_router/Makefile.slip.conf
  else ifeq (ethos,$(UPLINK))
    # ethos baudrate can be configured from make command
    ETHOS_BAUDRATE ?= 115200
    include $(CURDIR)/../RIOT/examples/gnrc_border_router/Makefile.ethos.conf
  else ifeq (cdc-ecm,$(UPLINK))
    include $(CURDIR)/../RIOT/examples/gnrc_border_router/Makefile.cdc-ecm.conf
  else ifeq (wifi,$(UPLINK))
    # SSID and Password need to be configured
    include $(CURDIR)/../RIOT/examples/gnrc_border_router/Makefile.wifi.conf
  endif
else
  # The number of native (emulated) ZigBee/6LoWPAN devices
  ZEP_DEVICES ?= 1
  include $(CURDIR)/../RIOT/examples/gnrc_border_router/Makefile.native.conf
endif

# As there is an 'Kconfig' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=


RIOTBASE ?= $(CURDIR)/../RIOT

include $(RIOTBASE)/Makefile.include
include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab


# Compile-time configuration for DHCPv6 client (needs to come after
# Makefile.include as this might come from Kconfig)
ifeq (1,$(USE_DHCPV6))
  ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE
    ifeq (1,$(STATIC_ROUTES))
      CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1
      # CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1 requires one more address
      # for `fe80::2`.
      CFLAGS += -DCONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF=3
    endif
  endif
endif


.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

# define native specific targets to only run UHCP daemon when required
ifneq (,$(filter native,$(BOARD)))
ifneq (1,$(USE_DHCPV6))
.PHONY: uhcpd-daemon

uhcpd-daemon: host-tools
	$(RIOTTOOLS)/uhcpd/bin/uhcpd $(TAP) $(IPV6_PREFIX) &
endif
endif

ifeq (slip,$(UPLINK))
sliptty:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/sliptty
endif



# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk