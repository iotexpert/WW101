NAME := App_WW101KEY_05_01_attach_open

$(NAME)_SOURCES := 01_attach_open.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h

# These do not have any LEDs/Buttons
INVALID_PLATFORMS := BCM943909* BCM943907*
