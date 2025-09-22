
#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#define FRAME_DEPTH        4

#define FONT_SIZE_LARGE  32
#define FONT_SIZE_SMALL  16


#include <stdio.h>
#include <signal.h>

#include <string>
#include <vector>


// Openorbis
#include "controller.h"
#include "graphics.h"
#include "png.h"

// Custom
#include "utils.h"



// Fonts for the menu
FT_Face fontLarge;
FT_Face fontSmall;



int main(void) {
    int result;
    
    // Initialize UserService
    result = sceUserServiceInitialize(NULL);
    if (result != 0 && result != 0x80960003) { // Failed to initialize UserService
        sceKernelNotificationRequestf((char*)"Failed to initialize UserService!");
        for (;;);
    }


    // Initialize controller
    auto controller = new Controller();
    if (!controller->Init(-1)) { // Failed to initialize Controller
        sceKernelNotificationRequestf((char*)"Failed to initialize controller!");
        for (;;);
    }


    // Initialize 2D scene
    auto scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
    if (!scene->Init(0xC000000, 2)) { // Failed to initialize 2D scene
        sceKernelNotificationRequestf((char*)"Failed to initialize 2D scene!");
        for (;;);
    }


    // Initialize large font
    if (!scene->InitFont(&fontLarge, (const char*)"/app0/assets/fonts/Gontserrat-Regular.ttf", FONT_SIZE_LARGE)) { // Failed to initialize large font
        sceKernelNotificationRequestf((char*)"Failed to initialize large font!");
        for (;;);
    }

    // Initialize small font
    if (!scene->InitFont(&fontSmall, (const char*)"/app0/assets/fonts/Gontserrat-Regular.ttf", FONT_SIZE_SMALL)) { // Failed to initialize small font
        sceKernelNotificationRequestf((char*)"Failed to initialize small font!");
        for (;;);
    }

    // Load PNGs
    auto anvil = new PNG("/app0/assets/images/anvil.png");
    auto luma = new PNG("/app0/assets/images/luma.png");


    char firmwareVersion[8]; // Holds the current firmware version as an string (e.g. 11.00)
    int userId; // Holds the user ID of the current logged in user
    char username[32]; // Holds the username of the current logged in user

    
    // Menu vars
    int lastLine;
    int selectedMenuIndex = 0;

    const char* menuItems[] = {
        "Disable system updates",
        "Enable system updates",
        "Credits"
    };

    int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);

    int downHoldCounter = 0;
    int upHoldCounter = 0;

    const int INITIAL_HOLD_DELAY = 15;  // Frames till first repeat (~150ms)
    const int REPEAT_HOLD_DELAY = 3;   // Frames till next repeats (~30ms)


    int frameID = 0;
    for (;;) { // Main loop
        getFirmwareVersionStr(firmwareVersion); // Get current firmware version
        getCurrentUserId(&userId); // Get user ID
        getUsernameFromUserId(userId, username, sizeof(username) - 1); // Get username

        scene->SetActiveFrameBuffer((frameID + 1) % 2);


        scene->DrawText((char*)"Orbis Forge v1.02", fontLarge, 75, 75, black, white);
        scene->DrawTextf(fontSmall, 75, 1005, black, white, (char*)"PS4 %s  -  %s (User ID: 0x%x)", firmwareVersion, username, userId);
        scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);

        anvil->Draw(scene, 1475, 580);

        
        lastLine = 100;

        for (int i = 0; i < numMenuItems; i++) {
            lastLine += 25;

            if (i == selectedMenuIndex)
                scene->DrawText((char*)menuItems[i], fontSmall, 75, lastLine, black, yellow);
            else
                scene->DrawText((char*)menuItems[i], fontSmall, 75, lastLine, black, white);
        }


        bool downPressed = controller->DpadDownPressed();
        bool upPressed = controller->DpadUpPressed();


        // DOWN Scroll
        if (downPressed) {
            downHoldCounter++;
            if (downHoldCounter == 1 ||
                (downHoldCounter > INITIAL_HOLD_DELAY && (downHoldCounter - INITIAL_HOLD_DELAY) % REPEAT_HOLD_DELAY == 0)) {
                if (selectedMenuIndex < numMenuItems - 1)
                    selectedMenuIndex++;
            }
        }
        else
            downHoldCounter = 0;

        // UP scroll
        if (upPressed) {
            upHoldCounter++;
            if (upHoldCounter == 1 ||
                (upHoldCounter > INITIAL_HOLD_DELAY && (upHoldCounter - INITIAL_HOLD_DELAY) % REPEAT_HOLD_DELAY == 0)) {
                if (selectedMenuIndex > 0)
                    selectedMenuIndex--;
            }
        }
        else
            upHoldCounter = 0;



        if (controller->XPressed()) { // Something was selected
            if (strcmp(menuItems[selectedMenuIndex], "Disable system updates") == 0) { // Disable system updates (Scene-Collective's disable updates payload was used as reference)
                scene->FrameBufferFill(black);


                scene->DrawText((char*)"Orbis Forge v1.02", fontLarge, 75, 75, black, white);
                scene->DrawTextf(fontSmall, 75, 1005, black, white, (char*)"PS4 %s  -  %s (User ID: 0x%x)", firmwareVersion, username, userId);
                scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);

                anvil->Draw(scene, 1475, 580);


                // Disable system updates

                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 125, black, white);
                sceKernelUnlink("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 150, black, white);
                sceKernelRmdir("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Creating directory /update/PS4UPDATE.PUP.net.temp with OrbisKernelMode 777 ...", fontSmall, 75, 175, black, white);
                sceKernelMkdir("/update/PS4UPDATE.PUP.net.temp", 777); // Create dummy directory



                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP ...", fontSmall, 75, 200, black, white);
                sceKernelUnlink("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP ...", fontSmall, 75, 225, black, white);
                sceKernelRmdir("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Creating directory /update/PS4UPDATE.PUP with OrbisKernelMode 777 ...", fontSmall, 75, 250, black, white);
                sceKernelMkdir("/update/PS4UPDATE.PUP", 777); // Create dummy directory


                // System updates disabled

                scene->DrawText((char*)"Finished! Press O to go back to the main menu.", fontSmall, 75, 300, black, green);


                // Wait for user to press circle

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

            else if (strcmp(menuItems[selectedMenuIndex], "Enable system updates") == 0) { // Disable system updates (Scene-Collective's enable updates payload was used as reference)
                scene->FrameBufferFill(black);


                scene->DrawText((char*)"Orbis Forge v1.02", fontLarge, 75, 75, black, white);
                scene->DrawTextf(fontSmall, 75, 1005, black, white, (char*)"PS4 %s  -  %s (User ID: 0x%x)", firmwareVersion, username, userId);
                scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);

                anvil->Draw(scene, 1475, 580);


                // Enable system updates
                
                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 125, black, white);
                sceKernelUnlink("/update/PS4UPDATE.PUP.net.temp");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP.net.temp ...", fontSmall, 75, 150, black, white);
                sceKernelRmdir("/update/PS4UPDATE.PUP.net.temp"); // Remove dummy directory



                scene->DrawText((char*)"Unlinking /update/PS4UPDATE.PUP ...", fontSmall, 75, 175, black, white);
                sceKernelUnlink("/update/PS4UPDATE.PUP");

                scene->DrawText((char*)"Removing directory /update/PS4UPDATE.PUP ...", fontSmall, 75, 200, black, white);
                sceKernelRmdir("/update/PS4UPDATE.PUP");  // Remove dummy directory



                // System updates enabled

                scene->DrawText((char*)"Finished! Press O to go back to the main menu.", fontSmall, 75, 250, black, green);


                // Wait for user to press circle

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





            else if (strcmp(menuItems[selectedMenuIndex], "Credits") == 0) { // Display credits
                scene->SetActiveFrameBuffer((frameID + 1) % 2);
                scene->FrameBufferFill(black);


                scene->DrawText((char*)"Orbis Forge v1.02", fontLarge, 75, 75, black, white);
                scene->DrawTextf(fontSmall, 75, 1005, black, white, (char*)"PS4 %s  -  %s (User ID: 0x%x)", firmwareVersion, username, userId);
                scene->DrawText((char*)"https://github.com/PurpleLUM4", fontSmall, 1600, 1005, black, white);



                luma->Draw(scene, 741, 221); // Draw purple luma
                scene->DrawText((char*)"OrbisForge by @PurpleLUM4 using OpenOrbis", fontSmall, 780, 650, black, white);



                // Wait for user to press circle

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
    }

    for (;;);
}