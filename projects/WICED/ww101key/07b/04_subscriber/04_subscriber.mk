NAME := App_WW101KEY_07b_04_subcriber

$(NAME)_SOURCES := 04_subscriber.c

$(NAME)_COMPONENTS := protocols/MQTT

WIFI_CONFIG_DCT_H := wifi_config_dct.h

$(NAME)_RESOURCES  := apps/aws_iot/rootca.cer \
                      apps/aws_iot/client.cer \
                      apps/aws_iot/privkey.cer
