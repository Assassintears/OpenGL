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
	GLuint     TextureID;  // ���������ID
	glm::ivec2 Size;       // ���δ�С,����
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���

private:
	FT_Library ft;
	FT_Face face;
};