#ifndef LEVEL_H
#define LEVEL_H

#include <memory>
#include <vector>
#include <string>
#include "gameObject.h"

extern std::vector<std::shared_ptr<GameObject>> objects;

void readInLevel(const std::string& level);

#endif
