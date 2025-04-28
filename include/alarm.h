#ifndef ALARM_H
#define ALARM_H
#include <stdbool.h>

void setupTIM6();
void setupDAC();

bool inline isConnStable();

bool inline SDStable();
bool inline mpuStable();

#endif //ALARM_H