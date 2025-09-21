#include <stdio.h>
#include <signal.h>
#include <stdexcept>
#include <thread>

#include "controller.h"
#include "debug.h"
#include "graphics.h"
#include "png.h"

// Dimensions
#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#define FRAME_DEPTH        4

#define FONT_SIZE_LARGE  32
#define FONT_SIZE_SMALL  16


#define DEBUG_LOG

#ifdef DEBUG_LOG
 #define DEBUG_IP   "192.168.0.48"
 #define DEBUG_PORT 3333
#endif



FT_Face fontLarge;
FT_Face fontSmall;

Color black;
Color white;
Color yellow;
Color green;

int frameID = 0;

int main(void) {

    char hostname[64];
    if (gethostname(hostname, sizeof(hostname)) != 0)
        strcpy(hostname, "PS4");

#ifdef DEBUG_LOG
    if (Debug::open(DEBUG_IP, DEBUG_PORT)) {
        signal(SIGSEGV, Debug::crash_handler);
        signal(SIGILL, Debug::crash_handler);
        signal(SIGFPE, Debug::crash_handler);
        signal(SIGABRT, Debug::crash_handler);
        
        // Greet debug server
        Debug::log("[OrbisForge] Hello from ");
        Debug::log(hostname);
        Debug::log("\n");
    }
    else
        utils_SceKernelNotificationRequest("Proceeding with no debug log.");
#endif

    // Initialize controller
    auto controller = new Controller();

    if (!controller->Init(-1)) {
#ifdef DEBUG_LOG
        Debug::log("[OrbisForge] Failed to initialize controller, proceeding to endless loop\n");
#endif
        for (;;);
    }

#ifdef DEBUG_LOG
    Debug::log("[OrbisForge] Successfuly initialized controller\n");
#endif



    // Initialize 2D scene
    auto scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
    if (!scene->Init(0xC000000, 2)) {
#ifdef DEBUG_LOG
        Debug::log("[OrbisForge] Failed to initialize 2D scene, proceeding to endless loop\n");
#endif
        for (;;);
    }

#ifdef DEBUG_LOG
    Debug::log("[OrbisForge] Successfully initialized 2D scene\n");
#endif

    black  = {   0,   0,   0 };
    white  = { 255, 255, 255 };
    yellow = { 255, 255,   0 };
    green  = {   0, 255,   0 };

    const char* font = "/app0/assets/fonts/Gontserrat-Regular.ttf";

    Debug::log("[OrbisForge] Initializing fonts ...\n");

    if (!scene->InitFont(&fontLarge, font, FONT_SIZE_LARGE)) {
#ifdef DEBUG_LOG
        Debug::log("[OrbisForge] Failed to initialized large font, proceeding to endless loop\n");
#endif
        for (;;);
    }
#ifdef DEBUG_LOG
    Debug::log("[OrbisForge] Successfully initialized large font.\n");
#endif

    if (!scene->InitFont(&fontSmall, font, FONT_SIZE_SMALL)) {
#ifdef DEBUG_LOG
        Debug::log("[OrbisForge] Failed to initialize small font, proceeding to endless loop\n");
#endif
        for (;;);
    }

#ifdef DEBUG_LOG
    Debug::log("[OrbisForge] Successfully initialized small font.\n");
#endif




    auto anvil_png = new PNG("/app0/assets/images/anvil.png");
#ifdef DEBUG_LOG
    Debug::log("[OrbisForge] Loaded /app0/assets/images/anvil.png\n");
#endif


    // Menu rendering etc

    int selectedIndex = 0;
    int lastPixel;

    const char* menuItems[] = {
        "Disable system updates",
        "Enable system updates",
        "Tuff option 3",
        "Tuff option 4",
        "Tuff option 5",
        "Tuff option 6"
    };

    int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);

    int downHoldCounter = 0;
    int upHoldCounter = 0;

    const int INITIAL_HOLD_DELAY = 15;  // Frames till first repeat (~150ms)
    const int REPEAT_HOLD_DELAY = 3;   // Frames till next repeats (~30ms)










    OrbisKernelSwVersion SwVersion;
    sceKernelGetSystemSwVersion(&SwVersion);


    // E.g. PS4 11.00
    char info_text[128];
    snprintf(info_text, sizeof(info_text), "%s ", hostname);

    if (SwVersion.s_version[1] == '.')
        strncat(info_text, SwVersion.s_version, 4);
    else
        strncat(info_text, SwVersion.s_version, 5);

    for (;;) {
        lastPixel = 100;

        scene->SetActiveFrameBuffer((frameID + 1) % 2);

        scene->DrawText((char*)"Orbis Forge", fontLarge, 75, 75, black, white);
        scene->DrawText(info_text, fontSmall, 75, 1005, black, white);
        scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);
        anvil_png->Draw(scene, 1475, 580);

        for (int i = 0; i < numMenuItems; i++) {
            if (i == selectedIndex)
                scene->DrawText((char*)menuItems[i], fontSmall, 75, lastPixel + 25, black, yellow);
            else
                scene->DrawText((char*)menuItems[i], fontSmall, 75, lastPixel + 25, black, white);

            lastPixel += 25;
        }

        bool downPressed = controller->DpadDownPressed();
        bool upPressed = controller->DpadUpPressed();

        // DOWN Scroll
        if (downPressed) {
            downHoldCounter++;
            if (downHoldCounter == 1 ||
                (downHoldCounter > INITIAL_HOLD_DELAY && (downHoldCounter - INITIAL_HOLD_DELAY) % REPEAT_HOLD_DELAY == 0)) {
                if (selectedIndex < numMenuItems - 1) selectedIndex++;
            }
        }
        else {
            downHoldCounter = 0;
        }

        // UP scroll
        if (upPressed) {
            upHoldCounter++;
            if (upHoldCounter == 1 ||
                (upHoldCounter > INITIAL_HOLD_DELAY && (upHoldCounter - INITIAL_HOLD_DELAY) % REPEAT_HOLD_DELAY == 0)) {
                if (selectedIndex > 0) selectedIndex--;
            }
        }
        else {
            upHoldCounter = 0;
        }
        
        if (controller->XPressed()) {

            // Disable system updates
            if (strcmp(menuItems[selectedIndex], "Disable system updates") == 0) {
                scene->SetActiveFrameBuffer((frameID + 1) % 2);
                scene->FrameBufferFill(black);

                scene->DrawText((char*)"Orbis Forge", fontLarge, 75, 75, black, white);
                scene->DrawText(info_text, fontSmall, 75, 1005, black, white);
                scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);
                anvil_png->Draw(scene, 1475, 580);

                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 125, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Unlinking /update/PS4UPDATE.PUP.net.temp ...\n");
#endif
                sceKernelUnlink("/update/PS4UPDATE.PUP.net.temp");


                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 150, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Removing directory /update/PS4UPDATE.PUP.net.temp ...\n");
#endif
                sceKernelRmdir("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Creating directory /update/PS4UPDATE.PUP.net.temp with OrbisKernelMode 777 ...", fontSmall, 75, 175, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Creating directory /update/PS4UPDATE.PUP.net.temp with OrbisKernelMode 777 ...\n");
#endif
                sceKernelMkdir("/update/PS4UPDATE.PUP.net.temp", 777);



                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP ...", fontSmall, 75, 200, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Unlinking /update/PS4UPDATE.PUP ...\n");
#endif
                sceKernelUnlink("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP ...", fontSmall, 75, 225, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Removing directory /update/PS4UPDATE.PUP ...\n");
#endif
                sceKernelRmdir("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Creating directory /update/PS4UPDATE.PUP with OrbisKernelMode 777 ...", fontSmall, 75, 250, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Creating directory /update/PS4UPDATE.PUP with OrbisKernelMode 777 ...\n");
#endif
                sceKernelMkdir("/update/PS4UPDATE.PUP", 777);


                scene->DrawText((char*)"Finished! Press O to go back to the main menu.", fontSmall, 75, 300, black, green);
                utils_SceKernelNotificationRequest("Disabled system updates!");

#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Disabled system updates\n");
#endif

                scene->SubmitFlip(frameID);
                scene->FrameWait(frameID);
                scene->FrameBufferSwap();
               
                for (;;) {
                    if (controller->CirclePressed())
                        break;

                    sceKernelUsleep(10000); // 10ms Sleep
                }

                scene->SetActiveFrameBuffer((frameID + 1) % 2);
                scene->FrameBufferFill(black);

                scene->SubmitFlip(frameID);
                scene->FrameWait(frameID);
                scene->FrameBufferSwap();
                frameID++;

                continue;
            }

            // Enable system updates
            else if (strcmp(menuItems[selectedIndex], "Enable system updates") == 0) {
                scene->SetActiveFrameBuffer((frameID + 1) % 2);
                scene->FrameBufferFill(black);

                scene->DrawText((char*)"Orbis Forge", fontLarge, 75, 75, black, white);
                scene->DrawText(info_text, fontSmall, 75, 1005, black, white);
                scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);
                anvil_png->Draw(scene, 1475, 580);

                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 125, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Unlinking /update/PS4UPDATE.PUP.net.temp ...\n");
#endif
                sceKernelUnlink("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 150, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Removing directory /update/PS4UPDATE.PUP.net.temp ...\n");
#endif
                sceKernelRmdir("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP ...", fontSmall, 75, 175, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Unlinking /update/PS4UPDATE.PUP ...\n");
#endif
                sceKernelUnlink("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP ...", fontSmall, 75, 200, black, white);
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Removing directory /update/PS4UPDATE.PUP ...\n");
#endif
                sceKernelRmdir("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Finished! Press O to go back to the main menu.", fontSmall, 75, 250, black, green);
                utils_SceKernelNotificationRequest("Enabled system updates!");
#ifdef DEBUG_LOG
                Debug::log("[OrbisForge] Enabled system updates\n");
#endif

                scene->SubmitFlip(frameID);
                scene->FrameWait(frameID);
                scene->FrameBufferSwap();

                for (;;) {
                    if (controller->CirclePressed())
                        break;

                    sceKernelUsleep(10000); // 10ms Sleep
                }

                scene->SetActiveFrameBuffer((frameID + 1) % 2);
                scene->FrameBufferFill(black);

                scene->SubmitFlip(frameID);
                scene->FrameWait(frameID);
                scene->FrameBufferSwap();
                frameID++;

                continue;
            }
        }

        // Frame Render + Swap
        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();
        frameID++;

        sceKernelUsleep(10000); // 10ms Sleep
    }

    Debug::log("[OrbisForge] Execution finished, proceeding to endless loop.");
    for (;;);
}