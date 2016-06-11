#include "entity.h"
#include "renderer.h"

#include <allegro5/allegro_color.h>

#include <vector>
#include <unordered_set>
#include <algorithm>

extern std::string myID;
extern std::vector<size_t> selectedEntities;
extern std::unordered_set<size_t> lastMovedEntities;

#define MOVE_SPEED 20

Entity::Entity(std::string hash, Vector2 location, std::string ownerID, size_t id)
{
	this->hash = hash;
	this->location = location;
	this->dragPos = location;
	this->ownerID = ownerID;
	this->destination = location;
	this->id = id;
}

bool Entity::checkHit(Vector2 location)
{
	const Vector2 temp = this->location + this->getSize();
	return this->location.x <= location.x && temp.x >= location.x && this->location.y <= location.y && temp.y >= location.y;
}

bool Entity::checkCollision(Entity other)
{
	const Vector2 mySize = this->getSize();
	const Vector2 otherSize = other.getSize();
	const Vector2 distance = (this->location - other.location).abs();
	return distance.x < mySize.x/2 + otherSize.x/2 && distance.y < mySize.y/2 + otherSize.y/2;
}

void Entity::draw()
{
	Vector2 drawPos;
	if(std::find(selectedEntities.begin(), selectedEntities.end(), this->id) == selectedEntities.end()
		&& lastMovedEntities.find(this->id) == lastMovedEntities.end())
	{
		if(this->location != this->destination)
		{
			Vector2 newPos;
			Vector2 distance = this->destination - this->location;
			if(distance.len2() < MOVE_SPEED)
			{
				this->location = this->destination;
			}
			else
			{
				this->location += distance / distance.len() * MOVE_SPEED;
			}
		}
		drawPos = this->location;
	}
	else
	{
		drawPos = this->dragPos;
	}

	Renderer::drawImage(this->hash, drawPos);
	if(players.find(ownerID) != players.end())
	{
		Vector2 pos = Renderer::gameCoordinatesToScreen(drawPos);
		Vector2 size = this->getSize() / Renderer::screenZoom;
		al_draw_rectangle(pos.x, pos.y, (pos + size).x, (pos + size).y, al_color_hsl(players.at(ownerID).color, 1, 0.5), 3);
	}
}

Vector2 Entity::getSize()
{
	return Renderer::getImageSize(this->hash);
}

void Entity::move(Vector2 newPos)
{
	this->destination = newPos;
}
