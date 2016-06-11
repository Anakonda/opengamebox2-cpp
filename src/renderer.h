#ifndef RENDERER_H
#define RENDERER_H

#include "vector2.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <string>

namespace Renderer
{
	extern ALLEGRO_COLOR white;
	extern ALLEGRO_COLOR grey;
	extern ALLEGRO_COLOR green;

	extern ALLEGRO_TRANSFORM uiTransform;
	extern ALLEGRO_TRANSFORM gameTransform;

	extern Vector2 screenPos;
	extern double screenZoom;

	void init();
	void deinit();
	ALLEGRO_DISPLAY* getDisplay();
	void drawTable();

	ALLEGRO_FONT* getFont();

	void drawCross(const Vector2 coordinates, const ALLEGRO_COLOR* color);

	void drawText(const Vector2 coordinates, const ALLEGRO_COLOR* color, const std::string text = std::string());

	void loadImage(std::string hash);
	void drawImage(std::string hash, Vector2 location, ALLEGRO_COLOR tint = white);
	Vector2 getImageSize(std::string hash);

	Vector2 gameCoordinatesToScreen(Vector2 coordinates);
	Vector2 screenCoordinatesToGame(Vector2 coordinates);

	void resize();
}

#endif // RENDERER_H
