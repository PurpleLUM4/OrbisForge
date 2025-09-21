#include "utils.h"

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