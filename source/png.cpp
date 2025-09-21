#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "graphics.h"
#include "png.h"

PNG::PNG(const char *imagePath)
{
	this->img = (uint32_t *)stbi_load(imagePath, &this->width, &this->height, &this->channels, STBI_rgb_alpha);

 	if (this->img == NULL)
	{
		return;
	}
}

PNG::~PNG()
{
	if(this->img != NULL)
		stbi_image_free(this->img);
}

void PNG::Draw(Scene2D* scene, int startX, int startY)
{
    if (this->img == NULL)
        return;

    for (int yPos = 0; yPos < this->height; yPos++)
    {
        for (int xPos = 0; xPos < this->width; xPos++)
        {
            uint32_t encodedColor = this->img[(yPos * this->width) + xPos];

            int x = startX + xPos;
            int y = startY + yPos;

            uint8_t r = (uint8_t)((encodedColor >> 0) & 0xFF);
            uint8_t g = (uint8_t)((encodedColor >> 8) & 0xFF);
            uint8_t b = (uint8_t)((encodedColor >> 16) & 0xFF);

            Color color = { r, g, b };

            if (x < 0 || y < 0 || x >= 1920 || y >= 1080)
                continue;

            scene->DrawPixel(x, y, color);
        }
    }
}


