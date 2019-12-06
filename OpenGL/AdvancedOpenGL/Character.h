#pragma once
#include "glad/glad.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "glm/glm.hpp"

class Character
{
public:
	Character();
	virtual ~Character();

public:
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小,像素
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离

private:
	FT_Library ft;
	FT_Face face;
};