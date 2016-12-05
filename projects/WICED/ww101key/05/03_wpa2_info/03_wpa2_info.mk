NAME := App_WW101KEY_05_03_wpa2_info

$(NAME)_SOURCES := 03_wpa2_info.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h

# These do not have any LEDs/Buttons
INVALID_PLATFORMS := BCM943909* BCM943907*
