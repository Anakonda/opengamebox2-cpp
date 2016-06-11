#include "main.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "interfaces/menu.h"
#include "interfaces/game.h"

void onConnection(std::string something)
{
	interfaceMutex.lock();
	delete interface;
	interface = new GameInterface(Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay())));
	interfaceMutex.unlock();
	sio::message::ptr message = sio::object_message::create();

	std::string authToken;
	const char* cstr = al_get_config_value(config, "player", "authToken");
	if (cstr == nullptr)
	{
		std::stringstream stream;
		stream << std::hex << rand();
		authToken =  stream.str();
		al_set_config_value(config, "player", "authToken", authToken.c_str());
	}
	else
	{
		authToken = cstr;
	}

	message->get_map().insert(std::pair<std::string, sio::message::ptr>("authToken", sio::string_message::create(authToken)));
	connection.socket()->emit(std::string("HANDSHAKE"), message);
	message = sio::object_message::create();
	message->get_map().insert(std::pair<std::string, sio::message::ptr>("name", sio::string_message::create(username)));
	connection.socket()->emit("PLAYER_UPDATE_REQUEST", message);
}

void connect(void)
{
	Menu* menu = (Menu*)interface;
	connection.connect(std::string("http://") + menu->urlTextbox->getText());
	username = menu->nameTextbox->getText();
}

void onHandshake(sio::event& event)
{
	myID = event.get_message()->get_map().at("id")->get_string();
}

void playerlist(sio::event &event)
{
	for(size_t i = 0; i < event.get_message()->get_vector().size(); i++)
	{
		Player player;
		sio::message::ptr message = event.get_message()->get_vector().at(i);
		player.id = message->get_map().at("id")->get_string();
		player.color = message->get_map().at("color")->get_int();
		if (players.find(player.id) != players.end())
		{
			players.erase(player.id);
		}
		players.insert(std::pair<std::string, Player>(player.id, player));
	}
}

void playerNick(sio::event &event)
{
	std::string id = event.get_message()->get_map().at("id")->get_string();
	std::string name = event.get_message()->get_map().at("name")->get_string();
	players.at(id).name = name;
}

void playerleave(sio::event &event)
{
	auto &list = event.get_message()->get_vector();
	for(auto &player : list)
	{
		std::string id = player->get_map().at("id")->get_string();
		if(players.find(id) != players.end())
		{
			players.erase(id);
		}
	}
}

void entitylist(sio::event &event)
{
	std::map<size_t, size_t> order;
	for(size_t i = 0; i < event.get_message()->get_vector().size(); i++)
	{
		sio::message::ptr message = event.get_message()->get_vector().at(i);
		std::string hash = message->get_map().at("imgHash")->get_string();
		int id = message->get_map().at("id")->get_int();
		int depth = message->get_map().at("depth")->get_int();
		std::string owner = message->get_map().at("selectedClientId")->get_string();
		double x = message->get_map().at("pos")->get_map().at("x")->get_double();
		double y = message->get_map().at("pos")->get_map().at("y")->get_double();
		if (entities.find(id) != entities.end())
		{
			entities.erase(id);
			entityOrder.erase(std::find(entityOrder.begin(), entityOrder.end(), id));
		}
		entities.insert(std::pair<size_t, Entity>(id, Entity(hash, Vector2(x, y), owner, id)));
		entities.at(id).depth = depth;
		order.insert(std::pair<size_t, size_t>(depth, id));
	}
	for(auto &temp : order)
	{
		entityOrder.push_back(temp.second);
	}
}

void entitymove(sio::event &event)
{
	for(size_t i = 0; i < event.get_message()->get_vector().size(); i++)
	{
		sio::message::ptr message = event.get_message()->get_vector().at(i);
		int id = message->get_map().at("id")->get_int();
		size_t depth = message->get_map().at("depth")->get_int();
		double x = message->get_map().at("pos")->get_map().at("x")->get_double();
		double y = message->get_map().at("pos")->get_map().at("y")->get_double();
		if (entities.find(id) != entities.end())
		{
			entities.at(id).move(Vector2(x, y));
			entities.at(id).depth = depth;
			entityOrder.erase(std::find(entityOrder.begin(), entityOrder.end(), id));
			if(lastMovedEntities.find(id) != lastMovedEntities.end())
			{
				entities.at(id).location = Vector2(x, y);
				lastMovedEntities.erase(id);
			}
		}
		entityOrder.push_back(id);
	}
}

void entityselect(sio::event &event)
{
	for(size_t i = 0; i < event.get_message()->get_vector().size(); i++)
	{
		sio::message::ptr message = event.get_message()->get_vector().at(i);
		int id = message->get_map().at("id")->get_int();
		std::string owner = message->get_map().at("selectedClientId")->get_string();
		size_t depth = message->get_map().at("depth")->get_int();
		if (entities.find(id) != entities.end())
		{
			entities.at(id).ownerID = owner;
			entities.at(id).depth = depth;
		}
	}
}

void entitydelete(sio::event &event)
{
	for(size_t i = 0; i < event.get_message()->get_vector().size(); i++)
	{
		sio::message::ptr message = event.get_message()->get_vector().at(i);
		size_t id = message->get_map().at("id")->get_int();
		entities.erase(id);
		std::vector<size_t>::iterator it = std::find(entityOrder.begin(), entityOrder.end(), id);
		entityOrder.erase(it);
		it = std::find(selectedEntities.begin(), selectedEntities.end(), id);
		if(it != selectedEntities.end())
		{
			selectedEntities.erase(it);
		}
	}
}

std::map<size_t, size_t> getEntityStack(size_t entityID, std::unordered_set<size_t> &visited)
{
	bool thisFound = false;
	std::map<size_t, size_t> ret;
	ret.insert(std::pair<size_t, size_t>(entities.at(entityID).depth, entityID));
	for(auto id : entityOrder)
	{
		if(!thisFound)
		{
			if(id == entityID)
			{
				thisFound = true;
			}
			continue;
		}
		if(visited.find(id) != visited.end())
		{
			continue;
		}
		if(entities.at(entityID).checkCollision(entities.at(id)))
		{
			auto temp = getEntityStack(id, visited);
			ret.insert(temp.begin(), temp.end());
			visited.insert(id);
		}
	}
	return ret;
}

int main(int argc, char **argv)
{
	al_init();
	al_install_mouse();
	al_install_keyboard();

	Renderer::init();

	config = al_load_config_file("cofig.ini");
	if(config == nullptr)
	{
		config = al_create_config();
	}

	ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();

	al_register_event_source(eventQueue, al_get_display_event_source(Renderer::getDisplay()));
	al_register_event_source(eventQueue, al_get_mouse_event_source());
	al_register_event_source(eventQueue, al_get_keyboard_event_source());

	interface = new Menu(Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay())), &connect);

	connection.set_socket_open_listener(&onConnection);

	connection.socket()->on("HANDSHAKE_REPLY", &onHandshake);
	connection.socket()->on("PLAYER_JOIN", &playerlist);
	connection.socket()->on("PLAYER_LEAVE", &playerleave);
	connection.socket()->on("PLAYER_UPDATE", &playerNick);
	connection.socket()->on("ENTITY_CREATE", &entitylist);
	connection.socket()->on("ENTITY_MOVE", &entitymove);
	connection.socket()->on("ENTITY_SELECT", &entityselect);
	connection.socket()->on("ENTITY_DELETE", &entitydelete);

	while (running)
	{
		ALLEGRO_EVENT event;
		while(al_get_next_event(eventQueue, &event))
		{
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				running = false;
				break;
			}
			else if(event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
			{
				al_acknowledge_resize(event.display.source);
				Renderer::resize();
				if (interface != nullptr)
				{
					interfaceMutex.lock();
					interface->screenResize(Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay())));
					interfaceMutex.unlock();
				}
			}
			else
			{
				bool eventHandled = false;
				if (interface != nullptr)
				{
					interfaceMutex.lock();
					eventHandled = interface->handleEvent(event);
					interfaceMutex.unlock();
				}
				if(!eventHandled)
				{
					if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
					{
						if(event.mouse.button == 1)
						{
							Vector2 clickPos(event.mouse.x, event.mouse.y);
							Vector2 gameClickPos = Renderer::screenCoordinatesToGame(clickPos);
							selectedEntities.clear();
							Entity* clickedEntity = nullptr;
							for(std::vector<size_t>::reverse_iterator it = entityOrder.rbegin(); it != entityOrder.rend(); ++it)
							{
								if(entities.at(*it).checkHit(gameClickPos))
								{
									if(entities.at(*it).ownerID != myID && players.find(entities.at(*it).ownerID) != players.end())
									{
										break;
									}
									clickedEntity = &entities.at(*it);

									std::unordered_set<size_t> visited;
									for(auto &temp : getEntityStack(*it, visited))
									{
										selectedEntities.push_back(temp.second);
									}

									movingObjects = true;
									moveObjectsStart = std::pair<Vector2, Vector2>(clickPos, gameClickPos);
									sio::message::ptr message = sio::array_message::create();
									for(auto &selectedEntity : selectedEntities)
									{
										sio::message::ptr entity = sio::object_message::create();
										entity->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("id"), sio::int_message::create(selectedEntity)));
										message->get_vector().push_back(entity);
									}
									connection.socket()->emit("ENTITY_SELECT_REQUEST", message);
									break;
								}
							}
							if(clickedEntity==nullptr)
							{
								connection.socket()->emit("ENTITY_SELECT_REQUEST", sio::array_message::create());
								boxSelect = true;
								boxSelectStart = gameClickPos;
							}
						}
						else if(event.mouse.button == 3)
						{
							movingScreen = true;
							moveScreenStart = std::pair<Vector2, Vector2>(Vector2(event.mouse.x, event.mouse.y), Renderer::screenPos);
						}
					}
					else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
					{
						if(event.mouse.button == 1)
						{
							if(movingObjects)
							{
								movingObjects = false;
								sio::message::ptr message = sio::array_message::create();
								for(auto &id : selectedEntities)
								{
									sio::message::ptr entity = sio::object_message::create();
									sio::message::ptr pos = sio::object_message::create();
									pos->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("x"), sio::int_message::create(entities.at(id).dragPos.x)));
									pos->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("y"), sio::int_message::create(entities.at(id).dragPos.y)));
									entity->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("pos"), pos));
									entity->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("id"), sio::int_message::create(id)));
									message->get_vector().push_back(entity);
									lastMovedEntities.insert(id);
								}
								connection.socket()->emit("ENTITY_MOVE_REQUEST", message);
							}
							selectedEntities.clear();
							if(boxSelect)
							{
								boxSelect = false;
								sio::message::ptr message = sio::array_message::create();
								Vector2 mousePos = Renderer::screenCoordinatesToGame(Vector2(event.mouse.x, event.mouse.y));
								Vector2 lower;
								Vector2 higer;
								if(mousePos.x > boxSelectStart.x)
								{
									lower.x = boxSelectStart.x;
									higer.x = mousePos.x;
								}
								else
								{
									lower.x = mousePos.x;
									higer.x = boxSelectStart.x;
								}
								if(mousePos.y > boxSelectStart.y)
								{
									lower.y = boxSelectStart.y;
									higer.y = mousePos.y;
								}
								else
								{
									lower.y = mousePos.y;
									higer.y = boxSelectStart.y;
								}
								for(size_t id : entityOrder)
								{
									Entity &entity = entities.at(id);
									if(lower.x <= entity.location.x && higer.x >= entity.location.x
										&& lower.y <= entity.location.y && higer.y >= entity.location.y)
									{

										sio::message::ptr entity = sio::object_message::create();
										entity->get_map().insert(std::pair<std::string, sio::message::ptr>(std::string("id"), sio::int_message::create(id)));
										message->get_vector().push_back(entity);
									}
								}
								connection.socket()->emit("ENTITY_SELECT_REQUEST", message);
							}
						}
						else if(event.mouse.button == 3)
						{
							movingScreen = false;
						}
					}
					else if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
					{
						if(event.mouse.dz == 0)
						{
							if(movingScreen)
							{
								Vector2 currentPos(event.mouse.x, event.mouse.y);
								Vector2 delta = (moveScreenStart.first - currentPos) * Renderer::screenZoom;
								Renderer::screenPos = moveScreenStart.second + delta;
							}
							if(movingObjects)
							{
								Vector2 currentPos(event.mouse.x, event.mouse.y);
								Vector2 delta = (currentPos - moveObjectsStart.first) * Renderer::screenZoom;
								for(size_t id : selectedEntities)
								{
									entities.at(id).dragPos = entities.at(id).location + delta;
								}
							}
						}
						else
						{
							Renderer::screenZoom *= 1 - float(event.mouse.dz)/20.0;
							if(Renderer::screenZoom <= 0)
							{
								Renderer::screenZoom = 1.0/20.0;
							}
						}
					}
					else if(event.type == ALLEGRO_EVENT_KEY_CHAR)
					{
						if(event.keyboard.keycode == ALLEGRO_KEY_DELETE)
						{
							sio::message::ptr message = sio::array_message::create();
							for(auto &it : entities)
							{
								if(it.second.ownerID == myID)
								{
									size_t id = it.first;
									sio::message::ptr msgentity = sio::object_message::create();
									msgentity->get_map().insert(std::pair<std::string, sio::message::ptr>("id", sio::int_message::create(id)));
									message->get_vector().push_back(msgentity);
								}
							}
							connection.socket()->emit("ENTITY_DELETE_REQUEST", message);
						}
					}
				}
			}
		}

		al_clear_to_color(Renderer::grey);
		// Begin render

		al_hold_bitmap_drawing(true);

		//al_use_transform(&Renderer::gameTransform);
		for(auto &id : entityOrder)
		{
			entities.at(id).draw();
		}
		if (interface != nullptr)
		{
			al_use_transform(&Renderer::uiTransform);
			interface->draw();
		}
		if(boxSelect)
		{
			Vector2 startPos = Renderer::gameCoordinatesToScreen(boxSelectStart);
			ALLEGRO_MOUSE_STATE state;
			al_get_mouse_state(&state);
			al_draw_rectangle(startPos.x, startPos.y, state.x, state.y, Renderer::white, 2);
		}
		al_hold_bitmap_drawing(false);

		// Draw the rendering
		al_flip_display();
	}

	if(interface != nullptr)
	{
		delete interface;
	}

	al_save_config_file("config.ini", config);
	al_destroy_config(config);

	connection.sync_close();
	al_destroy_event_queue(eventQueue);

	Renderer::deinit();
	return 0;
}
