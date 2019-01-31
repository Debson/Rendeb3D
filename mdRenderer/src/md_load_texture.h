#pragma once

#ifndef MDLOAD_TEXTURE_H
#define MDLOAD_TEXTURE_H


#include <glad/glad.h>
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <sstream>

#include "../external/stb_image.h"

#include "types.h"


GLuint mdLoadTexture(std::string &path);


#endif // !MDLOAD_TEXUTRE_H

