NAME := App_WW101KEY_06_07_client_response_secure_stream

$(NAME)_SOURCES := 02_client_response_secure_stream.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h

CERTIFICATE := $(SOURCE_ROOT)resources/certificates/certificate.pem
PRIVATE_KEY := $(SOURCE_ROOT)resources/certificates/key.pem
