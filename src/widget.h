#ifndef WIDGET_H
#define WIDGET_H

#include "vector2.h"

class Widget
{
public:
	enum class halign {LEFT, CENTER, RIGHT};
	enum class valign {TOP, CENTER, BOTTOM};
	enum class type
	{
		BUTTON,
		LABEL,
		PANEL,
		TEXTBOX
	};

	Widget(Vector2 pos, Vector2 size, std::pair<halign, valign> align, Vector2 screenSize);
	~Widget();
	virtual void draw() = 0;
	virtual bool checkHit(Vector2 clickPos);
	virtual void screenResize(Vector2 newSize, Vector2 offset = Vector2());
	virtual Widget::type getType() = 0;

	bool focused;
	Vector2 size;
protected:
	std::pair<halign, valign> alignment;
	Vector2 location;
	Vector2 realLocation;
};

#endif
