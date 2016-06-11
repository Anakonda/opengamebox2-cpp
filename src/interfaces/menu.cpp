#include "menu.h"
#include "../widgets/button.h"


Menu::Menu(Vector2 screenSize, void (*callback)()) : Interface()
{
	this->activeTextBox = nullptr;
	urlTextbox = new Textbox(Vector2(-100, -25), Vector2(200, 25), std::pair<Widget::halign, Widget::valign>(Widget::halign::CENTER, Widget::valign::CENTER), Vector2(), std::string("localhost:8000"));
	this->widgets.push_back(urlTextbox);
	nameTextbox = new Textbox(Vector2(-100, 5), Vector2(200, 25), std::pair<Widget::halign, Widget::valign>(Widget::halign::CENTER, Widget::valign::CENTER), Vector2(), std::string(""));
	this->widgets.push_back(nameTextbox);

	this->widgets.push_back(new Button(Vector2(-50, 35), Vector2(100, 25), std::pair<Widget::halign, Widget::valign>(Widget::halign::CENTER, Widget::valign::CENTER), Vector2(),
										&Renderer::grey, &Renderer::white, "Connect", callback));

	for(auto &widget : this->widgets)
	{
		widget->screenResize(screenSize);
	}
}

Menu::~Menu()
{

}

bool Menu::handleEvent(ALLEGRO_EVENT &event)
{
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
	{
        Vector2 location(event.mouse.x, event.mouse.y);
		if(this->activeTextBox != nullptr)
		{
			this->activeTextBox->focused = false;
			this->activeTextBox = nullptr;
		}
		for(auto &widget : this->widgets)
		{
			if (widget->checkHit(location))
			{
				if(widget->getType() == Widget::type::TEXTBOX)
				{
					this->activeTextBox = (Textbox*)widget;
					this->activeTextBox->focused = true;
				}
				return true;
			}
		}
		return false;
	}
	else if (event.type == ALLEGRO_EVENT_KEY_CHAR)
	{
		if (activeTextBox != nullptr)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				activeTextBox->focused = false;
				activeTextBox = nullptr;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
			{
				std::string text = activeTextBox->getText();
				if (text.size() > 0)
				{
					activeTextBox->setText(text.substr(0, text.size() - 1));
				}
			}
			else if (event.keyboard.keycode == 22)
			{
				char* clipboard = al_get_clipboard_text(Renderer::getDisplay());
				activeTextBox->setText(activeTextBox->getText() + clipboard);
				al_free(clipboard);
			}
			else
			{
				activeTextBox->setText(activeTextBox->getText() + (char)event.keyboard.unichar);
			}
			return true;
		}
		return false;
	}
}
