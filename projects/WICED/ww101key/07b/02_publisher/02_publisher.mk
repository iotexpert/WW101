NAME := App_WW101KEY_07b_02_publisher

$(NAME)_SOURCES := 02_publisher.c

$(NAME)_COMPONENTS := protocols/MQTT

WIFI_CONFIG_DCT_H := wifi_config_dct.h

$(NAME)_RESOURCES  := apps/aws_iot/rootca.cer \
                      apps/aws_iot/client.cer \
                      apps/aws_iot/privkey.cer

VALID_PLATFORMS := BCM943362WCD4 \
                   BCM943362WCD6 \
                   BCM943362WCD8 \
                   BCM943364WCD1 \
                   BCM94343WWCD1 \
                   BCM943438WCD1 \
                   BCM94343WWCD2 \
                   BCM94343W_AVN
