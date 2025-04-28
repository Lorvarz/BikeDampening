#ifndef ALARM_H
#define ALARM_H
#include <stdbool.h>

void setupTIM6();

bool  isConnStable();

bool  SDStable();
bool  mpuStable();
void  setupDAC();

#endif //ALARM_H