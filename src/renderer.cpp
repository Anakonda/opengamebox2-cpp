#include "renderer.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro.h>
#include <curl/curl.h>

#include <map>
#include <unordered_set>
#include <fstream>
#include <thread>

ALLEGRO_DISPLAY* display;
ALLEGRO_FONT* font;

ALLEGRO_COLOR Renderer::white;
ALLEGRO_COLOR Renderer::grey;
ALLEGRO_COLOR Renderer::green;

ALLEGRO_TRANSFORM Renderer::uiTransform;
ALLEGRO_TRANSFORM Renderer::gameTransform;

std::map<std::string, ALLEGRO_BITMAP*> images;
std::map<std::string, bool> imagesLoading;

Vector2 Renderer::screenPos;
double Renderer::screenZoom;
Vector2 screenSize;

void Renderer::init()
{
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
/*	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 2, ALLEGRO_SUGGEST);*/

	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
	display = al_create_display(1200, 800);

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();

	al_set_target_backbuffer(display);
	font = al_load_ttf_font("res/LiberationSans-Regular.ttf", 14, 0);

	white = al_map_rgba(255, 255, 255, 255);
	grey = al_map_rgba(40, 40, 40, 255);
	green = al_map_rgba(0, 255, 0, 255);

	al_identity_transform(&uiTransform);
	al_identity_transform(&gameTransform);
	Renderer::screenPos = Vector2(0x4000000000000, 0x4000000000000) + Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay()));
	screenSize = Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay()));
	Renderer::screenZoom = 1.0;

	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_VIDEO_BITMAP);
	//al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE | ALLEGRO_NO_PREMULTIPLIED_ALPHA);

	curl_global_init(CURL_GLOBAL_SSL);
}
void Renderer::deinit()
{
	al_destroy_display(display);
	al_destroy_font(font);
	al_shutdown_image_addon();
	for(auto &image : images)
	{
		al_destroy_bitmap(image.second);
	}
}

ALLEGRO_DISPLAY* Renderer::getDisplay()
{
	return display;
}

void Renderer::drawText(const Vector2 coordinates, const ALLEGRO_COLOR* color, std::string text)
{
	al_draw_text(font, *color, coordinates.x, coordinates.y, 0, text.c_str());
}

ALLEGRO_FONT* Renderer::getFont()
{
	return font;
}

std::ofstream cache;
size_t filehandler(void* buf, size_t size, size_t nmemb, void* userp)
{
	for (size_t c = 0; c<size*nmemb; c++)
	{
		cache.put(((char*)buf)[c]);
	}
	return size*nmemb;
}

void Renderer::loadImage(std::string hash)
{
	if (images.find(hash) != images.end())
	{
		return;
	}
	std::ifstream test("cache/" + hash);
	if (!test.good())
	{
		test.close();
		cache = std::ofstream("cache/" + hash, std::ofstream::binary);
		//std::ofstream cache("cache/" + hash, std::ofstream::binary);

		CURL* curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, (std::string("http://gateway.ipfs.io/ipfs/") + hash).c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &filehandler);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

		curl_easy_perform(curl);
		cache.close();
	}
	else
	{
		test.close();
	}
	(*imagesLoading.find(hash)).second = true;
}

void Renderer::drawImage(std::string hash, Vector2 location, ALLEGRO_COLOR tint)
{
	if(hash == "")
	{
		return;
	}
	if(images.find(hash) == images.end())
	{
		if(imagesLoading.find(hash) == imagesLoading.end())
		{
			imagesLoading.insert(std::pair<std::string, bool>(hash, false));
			std::thread thread(&Renderer::loadImage, hash);
			thread.detach();
			return;
		}
		else if(imagesLoading.at(hash) == true)
		{
			images.insert(std::pair<std::string, ALLEGRO_BITMAP*>(hash, al_load_bitmap((std::string("cache/") + hash).c_str())));
		}
		else if(imagesLoading.at(hash) == false)
		{
			return;
		}
	}
	if(images.at(hash) == nullptr)
	{
		return;
	}
	auto drawpos = gameCoordinatesToScreen(location);
	float scale = 1 / Renderer::screenZoom;
	float angle = 0;
	//Vector2 imageSize = getImageSize(hash);
	al_draw_tinted_scaled_rotated_bitmap(images.at(hash), tint, 0, 0, drawpos.x, drawpos.y, scale, scale, angle, 0);
}

Vector2 Renderer::getImageSize(std::string hash)
{
	if (images.find(hash) == images.end() || images.at(hash) == nullptr)
	{
		return Vector2();
	}
	return Vector2(al_get_bitmap_width(images.at(hash)), al_get_bitmap_height(images.at(hash)));
}

Vector2 Renderer::gameCoordinatesToScreen(Vector2 coordinates)
{
	return (coordinates - Renderer::screenPos + screenSize/2) / Renderer::screenZoom + (screenSize/2 - (screenSize/2 / Renderer::screenZoom));
}

Vector2 Renderer::screenCoordinatesToGame(Vector2 coordinates)
{
	return (coordinates * Renderer::screenZoom) + Renderer::screenPos - (screenSize/2 * Renderer::screenZoom);
}

void Renderer::resize()
{
	screenSize = Vector2(al_get_display_width(Renderer::getDisplay()), al_get_display_height(Renderer::getDisplay()));
}
