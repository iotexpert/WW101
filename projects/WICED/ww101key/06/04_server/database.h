#ifndef DATABASE_H
#define DATABASE_H
#include "wiced.h"
// the dbEntry is the structure that is stored in the linked list.
typedef struct dbEntry {
    uint32_t deviceId;
    uint32_t regId;
    uint32_t value;
} dbEntry_t;

void dbStart(void);
dbEntry_t *dbFind(dbEntry_t *find);
void dbSetValue(dbEntry_t *newValue);
uint32_t dbGetCount();

#define dbMax (400)
inline uint32_t dbGetMax()
{ return dbMax ;
}

#endif
