#include "game.h"

#include "../widgets/panel.h"
#include "../widgets/button.h"
#include "../widgets/label.h"
#include "../renderer.h"
#include "../player.h"

#include <allegro5/allegro_color.h>
#include <sio_client.h>

#include <map>
#include <fstream>
#include <sstream>
#include <cstdio>

extern std::map<std::string, Player> players;
extern sio::client connection;

Panel* mainPanel;
Panel* playerListPanel;
Panel* entityPanel;

Textbox* entityName;
Textbox* scriptName;

void toggleMainPanel()
{
	mainPanel->focused = !mainPanel->focused;
}

void togglePlayerPanel()
{
	entityPanel->focused = false;
	playerListPanel->focused = !playerListPanel->focused;
}

void toggleEntityPanel()
{
	playerListPanel->focused = false;
	entityPanel->focused = !entityPanel->focused;
}

void createEntity()
{
	sio::message::ptr message = sio::array_message::create();
	sio::message::ptr entity = sio::object_message::create();
	sio::message::ptr pos = sio::object_message::create();
	pos->get_map().insert(std::pair<std::string, sio::message::ptr>("x", sio::double_message::create(Renderer::screenPos.x)));
	pos->get_map().insert(std::pair<std::string, sio::message::ptr>("y", sio::double_message::create(Renderer::screenPos.y)));
	entity->get_map().insert(std::pair<std::string, sio::message::ptr>("imgHash", sio::string_message::create(entityName->getText())));
	entity->get_map().insert(std::pair<std::string, sio::message::ptr>("selectedClientId", sio::null_message::create()));
	entity->get_map().insert(std::pair<std::string, sio::message::ptr>("pos", pos));

	message->get_vector().push_back(entity);
	connection.socket()->emit("ENTITY_CREATE_REQUEST", message);
}

void loadScript()
{
	std::string strscript = "scripts/";
	strscript += scriptName->getText() + ".txt";
	sio::message::ptr message = sio::array_message::create();
	std::ifstream script(strscript);
	if(script.bad())
	{
		return;
	}
	for(std::string line; std::getline(script, line); )
	{
		Vector2 vecpos = Renderer::screenPos;
		std::string hash;

		std::string key;
		std::string value;
		std::istringstream temp(line);
		for(std::string arg; std::getline(temp, arg, ' ');)
		{
			key = arg.substr(0, arg.find(':'));
			value = arg.substr(arg.find(':')+1);
			if(key == "hash")
			{
				hash = value;
			}
			else if(key == "relativePosition")
			{
				float x;
				float y;
				sscanf (value.c_str(), "%f,%f", &x, &y);
				vecpos += Vector2(x,y);
			}
		}
		sio::message::ptr entity = sio::object_message::create();
		sio::message::ptr pos = sio::object_message::create();
		pos->get_map().insert(std::pair<std::string, sio::message::ptr>("x", sio::double_message::create(vecpos.x)));
		pos->get_map().insert(std::pair<std::string, sio::message::ptr>("y", sio::double_message::create(vecpos.y)));
		entity->get_map().insert(std::pair<std::string, sio::message::ptr>("imgHash", sio::string_message::create(hash)));
		entity->get_map().insert(std::pair<std::string, sio::message::ptr>("selectedClientId", sio::null_message::create()));
		entity->get_map().insert(std::pair<std::string, sio::message::ptr>("pos", pos));
		message->get_vector().push_back(entity);
	}
	connection.socket()->emit("ENTITY_CREATE_REQUEST", message);
}

GameInterface::GameInterface(Vector2 screenSize) : Interface()
{
	this->activeTextBox = nullptr;
	this->widgets.push_back(new Button(Vector2(-50, 0), Vector2(49, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::RIGHT, Widget::valign::TOP),
										screenSize, &Renderer::grey, &Renderer::white, std::string("Menu"), &toggleMainPanel));
	mainPanel = new Panel(Vector2(-200, 0), Vector2(200, screenSize.y),
									std::pair<Widget::halign, Widget::valign>(Widget::halign::RIGHT, Widget::valign::TOP), screenSize, &Renderer::grey);
	this->widgets.push_back(mainPanel);

	playerListPanel = new Panel(Vector2(0, 20), mainPanel->size - Vector2(0, 21),
								std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP), mainPanel->size, &Renderer::grey);
	mainPanel->widgets.push_back(playerListPanel);
	entityPanel = new Panel(Vector2(0, 20), mainPanel->size - Vector2(0, 21),
								std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP), mainPanel->size, &Renderer::grey);
	mainPanel->widgets.push_back(entityPanel);

	mainPanel->widgets.push_back(new Button(Vector2(1, 1), Vector2(75, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP),
											mainPanel->size, &Renderer::grey, &Renderer::white, std::string("Players"), &togglePlayerPanel));
	mainPanel->widgets.push_back(new Button(Vector2(75, 1), Vector2(75, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP),
											mainPanel->size, &Renderer::grey, &Renderer::white, std::string("entities"), &toggleEntityPanel));

	entityName = new Textbox(Vector2(2, 20), Vector2(196, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP),
								screenSize, "");
	entityPanel->widgets.push_back(new Button(Vector2(-25, 45), Vector2(50, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::CENTER, Widget::valign::TOP),
											mainPanel->size, &Renderer::grey, &Renderer::white, std::string("create"), &createEntity));

	entityPanel->widgets.push_back(entityName);

	scriptName = new Textbox(Vector2(2, 80), Vector2(196, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP),
								screenSize, "");
	entityPanel->widgets.push_back(new Button(Vector2(-25, 105), Vector2(50, 20), std::pair<Widget::halign, Widget::valign>(Widget::halign::CENTER, Widget::valign::TOP),
											mainPanel->size, &Renderer::grey, &Renderer::white, std::string("load"), &loadScript));

	entityPanel->widgets.push_back(scriptName);


	mainPanel->screenResize(screenSize);
}

GameInterface::~GameInterface()
{

}

bool GameInterface::handleEvent(ALLEGRO_EVENT &event)
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
			if(widget->checkHit(location))
			{
				if(widget->getType() == Widget::type::TEXTBOX)
				{
					this->activeTextBox = (Textbox*)widget;
					this->activeTextBox->focused = true;
				}
				return true;
			}
		}
	}
	else if (event.type == ALLEGRO_EVENT_KEY_CHAR)
	{
		if (activeTextBox != nullptr)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
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
	return false;
}

void GameInterface::draw()
{
	for(auto &widget : playerListPanel->widgets)
	{
		delete widget;
	}
	playerListPanel->widgets.clear();
	size_t i = 0;
	for(auto &player : players)
	{
		playerListPanel->widgets.push_back(new Label(Vector2(2, i * 20 + 50), std::pair<Widget::halign, Widget::valign>(Widget::halign::LEFT, Widget::valign::TOP),
															playerListPanel->size, al_color_hsl(player.second.color, 1, 0.5), player.second.name));
	}
	mainPanel->screenResize(Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay())));
	Interface::draw();
}
