#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bitmap.h>
#include <image.h>
#include <fstream>
#include <sstream>
#include "level.h"

void readInLevel (const std::string& level){
  std::ifstream infile(level);
  std::string line;
  char name;
  int x, y, w, h;

  while(getline(infile, line))
  {
      std::stringstream stream(line);

      if (name == 0){
          stream >> name;
          continue;
        }

      if (line.empty())
      {
          name = 0;
          continue;
      }

      if (name != 0)
      {
          if(name == 'P'){
            stream >> x >> y >> w >> h;
            objects.push_back(std::make_shared<Player>(glm::vec2(x, y), 0, glm::vec2(w, h), 0));
          }
          if(name == 'E'){
            stream >> x >> y >> w;
            objects.push_back(std::make_shared<Enemy>(glm::vec2(x, y), 0, glm::vec2(w, w), 1));
          }
      }
  }
}
