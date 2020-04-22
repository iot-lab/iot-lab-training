# name of the application
APPLICATION = ttn-mqtt

# The default board is ST B-L072Z-LRWAN1 LoRa discovery board
BOARD ?= b-l072z-lrwan1

RIOTBASE ?= $(CURDIR)/../../RIOT

#
# Here we add the modules that are needed
#
USEMODULE += xtimer
USEMODULE += hts221


#
# The application needs LoRaWAN related modules and variables:
#

# The Semtech LoRa radio device (SX1276)
USEMODULE += sx1276

# The Semtech Loramac package
USEPKG += semtech-loramac

# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# This must be the last line of the Makefile
include $(RIOTBASE)/Makefile.include
