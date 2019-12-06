#include "Character.h"
#include <iostream>

Character::Character()
	:TextureID{0},
	Size{-1, -1},
	Bearing{ -1, -1 }, Advance{-1}
{
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(face, 0, 48);
}

Character::~Character()
{
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}