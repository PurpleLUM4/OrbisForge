#pragma once

#include <stdlib.h> // For size_t

// Other includes in utils.cpp

void sceKernelNotificationRequestf(char* fmt, ...);

void getFirmwareVersionStr(char* out);
void getCurrentUserId(int* out);
void getUsernameFromUserId(int userId, char* out, size_t outSize);