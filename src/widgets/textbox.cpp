#include "textbox.h"

#include "../renderer.h"

Textbox::Textbox(Vector2 pos, Vector2 size, std::pair<halign, valign> align, Vector2 screenSize, std::string text)
			: Widget(pos, size, align, screenSize),
			text(text)
{

}

Textbox::~Textbox()
{

}

void Textbox::draw()
{
	Vector2 otherCorner = this->realLocation + this->size;
	ALLEGRO_COLOR *color = &Renderer::white;
	std::string temptext = this->text;
	if(this->focused)
	{
		color = &Renderer::green;
		temptext = temptext + '|';
	}
	al_draw_rectangle(this->realLocation.x, this->realLocation.y, otherCorner.x, otherCorner.y, *color, 2);
	if(this->text != "")
	{
		Renderer::drawText(this->realLocation + Vector2(5, 5), &Renderer::white, temptext);
	}
}

void Textbox::setText(std::string text)
{
	this->text = text;
}

std::string Textbox::getText()
{
	return this->text;
}

void Textbox::move(Vector2 location)
{
	this->location = location;
}

Vector2 Textbox::getLocation()
{
	return this->location;
}

Widget::type Textbox::getType()
{
	return Widget::type::TEXTBOX;
}
