#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bitmap.h>
#include <image.h>
#include <fstream>
#include <sstream>

#include <vector>

enum SpriteId {
    BigMario, LittleMario
};

struct Sprite {
    const int width;
    const int height;
    std::vector<unsigned int> frames;

    Sprite(int w, int h):
        width(w),
        height(h),
        frames()
        {
        }

    unsigned int& operator[](unsigned int i){ return frames[i];}
	const unsigned int& operator[](unsigned int i) const { return frames[i];}
};

void readSprites(std::vector<Sprite>& sprites, std::vector<glm::vec2>& frames, int width, int height)
{
    float left, top, right, bottom;
    std::ifstream infile("../assets/sprites.txt");
    std::string line;
    Sprite* sprite = nullptr;

    while(getline(infile, line))
    {
        if (line[0] == '#')
            continue;
        
        if (line.empty())
        {
            sprite = nullptr;
            continue;
        }

        std::stringstream stream(line);

        if (sprite == nullptr)
        {
            int w, h;
            stream >> w >> h;
            sprites.push_back({w, h});
            sprite = &sprites.back();
            continue;
        }
        
        stream >> left >> top;

        right = left + sprite->width - .1;
        bottom = top + sprite->height - .1;
        
        left += .1;
        top += .1;

        top = height - top;
        bottom = height - bottom;

        left /= static_cast<float>(width);
        top /= static_cast<float>(height);
        right /= static_cast<float>(width);
        bottom /= static_cast<float>(height);

        sprite->frames.push_back(frames.size());
        frames.push_back({left, top});
        frames.push_back({right, top});
        frames.push_back({left, bottom});
        frames.push_back({right, bottom});
    }
}
