// 7maxInstall.h

#ifndef __7MAXINSTALL_H
#define __7MAXINSTALL_H

#include "Common/String.h"

bool Start7maxService(CSysString &errorMessage);
void Check7maxConditions();
bool CheckPAE();
bool Check3GBMode();

#endif