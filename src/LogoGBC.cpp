#include "LogoGBC.h"

#include "TextureManager.h"

Logo::Logo()
{
	TextureManager::Instance()->load("../Assets/textures/gbcLogo.png", "logo");

	auto size = TextureManager::Instance()->getTextureSize("logo");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position = glm::vec2(200.0f, 200.0f);

	setType(LOGO);
	getRigidBody()->isColliding = false;

}

Logo::~Logo()
= default;

void Logo::draw()
{
	TextureManager::Instance()->draw("logo",
		getTransform()->position.x, getTransform()->position.y, 0, 255, true);
}

void Logo::update()
{
}

void Logo::clean()
{
}
