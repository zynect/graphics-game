#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bitmap.h>
#include <image.h>

#include <vector>

enum SpriteId {
    BigMario, LittleMario
};

class Sprite {
protected:
    int width;
    int height;
    std::vector<std::vector<glm::vec2>> frames;
    Image img;

public:
    Sprite(const std::string& fileName, int w, int h):
        width(w),
        height(h),
        frames(),
        img()
        {
            std::string bmp = fileName + ".bmp";
	        assert(readBMP(bmp.c_str(), img));
	        // TODO: read in UVs too
            frames.push_back({{0, 0}, {1, 0}, {0, 1}, {1, 1}});

            GLuint textureID;
            glGenTextures(1, &textureID);
            
            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            // Give the image to OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img.bytes[0]);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    
};
