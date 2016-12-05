NAME := App_WW101KEY_05_04_network_switch

$(NAME)_SOURCES := 04_network_switch.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h

# These do not have any LEDs/Buttons
INVALID_PLATFORMS := BCM943909* BCM943907*
