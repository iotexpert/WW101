NAME := App_WW101KEY_06_08_secure_server

$(NAME)_SOURCES := 08_secure_server.c

#GLOBAL_DEFINES     += RX_PACKET_POOL_SIZE=6
#GLOBAL_DEFINES     += TX_PACKET_POOL_SIZE=6

WIFI_CONFIG_DCT_H := wifi_config_dct.h

CERTIFICATE := $(SOURCE_ROOT)resources/certificates/certificate.pem
PRIVATE_KEY := $(SOURCE_ROOT)resources/certificates/key.pem
