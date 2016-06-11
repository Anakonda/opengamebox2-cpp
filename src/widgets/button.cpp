#include "button.h"

Button::Button(Vector2 location, Vector2 size, std::pair<Widget::halign, Widget::valign> alignment, Vector2 screenSize,
	ALLEGRO_COLOR* backgroundColor, ALLEGRO_COLOR* textColor, std::string text, void (*callback)())
	: Widget(location, size, alignment, screenSize)
{
	this->backgroundColor = backgroundColor;
	this->textColor = textColor;
	this->text = text;
	this->callback = callback;
}

Button::~Button()
{

}

void Button::draw()
{
	Vector2 drawLocation = this->realLocation;

	al_draw_filled_rectangle(drawLocation.x, drawLocation.y, drawLocation.x + size.x, drawLocation.y + size.y, *this->backgroundColor);
	al_draw_rectangle(drawLocation.x, drawLocation.y, drawLocation.x + size.x, drawLocation.y + size.y, Renderer::white, 1);
	Renderer::drawText(drawLocation + Vector2(2, 3), this->textColor, this->text);
}

bool Button::checkHit(Vector2 clickPos)
{
	if (Widget::checkHit(clickPos))
	{
		this->callback();
	}
}

Widget::type Button::getType()
{
	return Widget::type::BUTTON;
}
