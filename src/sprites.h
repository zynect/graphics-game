#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum SpriteNames {
    BigMario, LittleMario
};

class Sprite{
protected:
    int height;
    int width;
    vector<glm::vec2> frames;

public:
    Sprite(int h, int w, vector<glm::vec2> v):
        height(h),
        width(w),
        frames(v)
        {}

}

unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }

    return data;
}