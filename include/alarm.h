#ifndef ALARM_H
#define ALARM_H
#include <stdbool.h>

void setupTIM6();
void setupDAC();

bool  isConnStable();

bool  SDStable();
bool  mpuStable();

#endif //ALARM_H