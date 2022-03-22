APPLICATION = arroseur

BOARD ?= b-l072z-lrwan1

RIOTBASE ?= $(CURDIR)/../../RIOT

# Default LoRa region is Europe and default band is 868MHz

USEMODULE += ztimer_msec
USEMODULE += hts221
USEMODULE += sx1276

USEPKG += semtech-loramac
USEMODULE += semtech_loramac_rx
USEPKG += cayenne-lpp
LORA_REGION ?= EU868

include $(RIOTBASE)/Makefile.include

include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab