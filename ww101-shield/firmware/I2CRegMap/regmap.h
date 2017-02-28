#include <project.h>

#ifndef REGMAP_H
#define REGMAP_H
    
CY_PACKED typedef struct {
    float32  dacVal;
    uint8    ledVal;
    uint8    ledControl;
    uint8    buttonVal;
    float32  temperature;
    float32  humidity;
    float32  illuminance;
    float32  potVal;
} CY_PACKED_ATTR dataSet_t;

#endif