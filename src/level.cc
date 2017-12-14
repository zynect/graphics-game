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
      if (line[0] == '#')
        continue;

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
            objects.push_back(std::make_shared<Player>(glm::vec2(x, y), 4, glm::vec2(w, h), 1));
          }
          else if(name == 'E'){
            stream >> x >> y >> w;
            objects.push_back(std::make_shared<Enemy>(glm::vec2(x, y), 3, glm::vec2(w, w), 2));
          }
          else if(name == 'F'){
            stream >> x >> y >> w;
            objects.push_back(std::make_shared<Platform>(glm::vec2(x, y), 1, glm::vec2(w, w), 3));
          }
          else if(name == 'C'){
            stream >> x >> y >> w >> h;
            objects.push_back(std::make_shared<Coin>(glm::vec2(x + 2, y), 2, glm::vec2(w, h), 4));
          }
          else if(name == 'M'){
            stream >> x >> y >> w;
            objects.push_back(std::make_shared<Mushroom>(glm::vec2(x, y), 2, glm::vec2(w, h), 5));
          }
          else if(name == 'B'){
            stream >> x >> y >> w >> h;
            objects.push_back(std::make_shared<Background>(glm::vec2(x, y), 0, glm::vec2(w, h), 6));
          }
      }
  }
}
