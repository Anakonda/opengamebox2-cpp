#include "label.h"

Label::Label(Vector2 location, std::pair<halign, valign> alignment, Vector2 screenSize, ALLEGRO_COLOR textColor, std::string text)
	: Widget(location, Vector2(), alignment, screenSize)
{
	this->textColor = textColor;
	this->text = text;
}

Label::~Label()
{

}

void Label::draw()
{
	Renderer::drawText(this->realLocation, &this->textColor, this->text);
}

bool Label::checkHit(Vector2)
{
	return false;
}

Widget::type Label::getType()
{
	return Widget::type::LABEL;
}
