#include "utils.h"



// For some reason I have to include here or it wont compile
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <orbis/libkernel.h>
#include <orbis/UserService.h>



void sceKernelNotificationRequestf(char* fmt, ...) {
	if (strlen(fmt) >= 1024) { // Bufferoverflow protection
		bufferoverflow_protect:

		sceKernelNotificationRequestf((char*)"Bufferoverflow protection triggered!");
		return;
	}

	char text[1024];
	
	va_list args;
	va_start(args, fmt);
	vsnprintf(text, sizeof(text), fmt, args);
	va_end(args);


	if (strlen(text) >= 1024) // Bufferoverflow protection
		goto bufferoverflow_protect;



	OrbisNotificationRequest Buffer;

	strcpy(Buffer.message, text);

	Buffer.type = OrbisNotificationRequestType::NotificationRequest;
	Buffer.unk3 = 0;
	Buffer.useIconImageUri = 1;
	Buffer.targetId = -1;
	strcpy(Buffer.iconUri, "cxml://psnotification/tex_icon_system");

	sceKernelSendNotificationRequest(0, &Buffer, 3120, 0);

	return;
}


void getFirmwareVersionStr(char* out) {
	OrbisKernelSwVersion KernelSwVersion;
	sceKernelGetSystemSwVersion(&KernelSwVersion);
	
	if (KernelSwVersion.s_version[1] == '.') // Firmware version is something like X.XX
		strncpy(out, KernelSwVersion.s_version, 4);

	else // Firmware version is something like XX.XX
		strncpy(out, KernelSwVersion.s_version, 5);

	return;
}

void getCurrentUserId(int* out) { // UserService must be initialized successfully to run this
	if (sceUserServiceGetInitialUser(out) != 0)
		*out = -1;

	return;
}

void getUsernameFromUserId(int userId, char* out, size_t outSize) { // UserService must be initialized successfully to run this
	if (sceUserServiceGetUserName(userId, out, outSize) < 0)
		out[0] = -1;

	return;
}

void utils_SceKernelNotificationRequest(const char* text) {
	if (strlen(text) >= 1024) { // Bufferoverflow protection
		char bufferoverflow_warning[1024];
		snprintf(bufferoverflow_warning, sizeof(bufferoverflow_warning), "Bufferoverflow: Expected MAX. 1024 bytes got %zu bytes", strlen(text));

		utils_SceKernelNotificationRequest(bufferoverflow_warning);
		return;
	}

	OrbisNotificationRequest Buffer;

	strcpy(Buffer.message, text);

	Buffer.type = OrbisNotificationRequestType::NotificationRequest;
	Buffer.unk3 = 0;
	Buffer.useIconImageUri = 1;
	Buffer.targetId = -1;
	strcpy(Buffer.iconUri, "cxml://psnotification/tex_icon_system");

	sceKernelSendNotificationRequest(0, &Buffer, 3120, 0);
}