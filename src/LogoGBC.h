#pragma once
#pragma once
#ifndef __LOGOGBC__
#define __LOGOGBC__
#include "DisplayObject.h"

class Logo final : public DisplayObject
{
public:

	Logo();
	~Logo();

	void draw() override;
	void update() override;
	void clean() override;
};


#endif	/* defined (__LOGOGBC__) */
