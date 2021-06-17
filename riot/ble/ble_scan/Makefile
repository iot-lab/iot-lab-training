# name of your application
APPLICATION = ble_scan_rss

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# We use the shell in this example
USEMODULE += shell

# ble rss json scan module
EXTERNAL_MODULE_DIRS += $(CURDIR)/ble_rss_scan
USEMODULE += ble_scan_rss

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1


include $(RIOTBASE)/Makefile.include
