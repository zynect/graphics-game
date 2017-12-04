#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bitmap.h>
#include <image.h>
#include <fstream>

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
            std::string uvs = fileName + ".uv";
	        assert(readBMP(bmp.c_str(), img));
	        readUVs(uvs);
            //frames.push_back({{0, 1}, {1, 1}, {0, 0}, {1, 0}});

            GLuint textureID;
            glGenTextures(1, &textureID);
            
            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            // Give the image to OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img.bytes[0]);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    
    const std::vector<glm::vec2>& getFrameUVs(unsigned int frame) const
    {
        return frames[frame];
    };

    void readUVs(const std::string& fileName)
    {
        float left, top, right, bottom;
        std::ifstream infile(fileName);
        while (infile >> left >> top)
        {
            std::vector<glm::vec2> frame;

            right = left + width - .1;
            bottom = top + height - .1;
            
            left += .1;
            top += .1;

            top = img.height - top;
            bottom = img.height - bottom;

            left /= static_cast<float>(img.width);
            top /= static_cast<float>(img.height);
            right /= static_cast<float>(img.width);
            bottom /= static_cast<float>(img.height);

            frame.push_back({left, top});
            frame.push_back({right, top});
            frame.push_back({left, bottom});
            frame.push_back({right, bottom});
            frames.push_back(frame);
        }
    };
};
