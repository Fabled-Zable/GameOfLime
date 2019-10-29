//#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Element
{
private:
	virtual void _OnPress(int mouse) {}
	virtual void _OnRelease(int mouse) {}
	virtual void _OnHeld(int mouse) {}
	virtual void _OnHover() {}


public:
	int x, y, width, height;

	std::function<void(int mouse)> onPress;
	std::function<void(int mouse)> onRelease;
	std::function<void(int mouse)> onHeld;
	std::function<void()> onHover;

	olc::PixelGameEngine* engine;


	Element(olc::PixelGameEngine* engine, int x = 0, int y = 0, int width = 0, int height = 0 )
	{
		onPress = [this](int mouse) -> void {};
		onRelease = [this](int mouse) -> void {};
		onHeld = [this](int mouse) -> void {};
		onHover = [this]() -> void {};


		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->engine = engine;
	}

	bool mouseHovered()
	{
		int32_t mouseX = engine->GetMouseX();
		int32_t mouseY = engine->GetMouseY();
		return mouseX >= x && mouseY >= y && mouseX <= x + width && mouseY <= y + height;
	}

	void Poll()
	{
		if (mouseHovered())
		{
			olc::HWButton mouse0 = engine->GetMouse(0);
			olc::HWButton mouse1 = engine->GetMouse(1);

			_OnHover();
			onHover();
			if (mouse0.bPressed)
			{
				_OnPress(0);
				onPress(0);
			}
			if (mouse1.bPressed)
			{
				_OnPress(1);
				onPress(1);
			}

			if (mouse0.bReleased)
			{
				_OnRelease(0);
				onRelease(0);
			}
			if (mouse1.bReleased)
			{
				_OnRelease(1);
				onRelease(1);
			}

			if (mouse0.bHeld)
			{
				_OnHeld(0);
				onHeld(0);
			}
			if (mouse1.bHeld)
			{
				_OnHeld(1);
				onHeld(1);
			}
		}
	}
};

class Box : public Element
{
public:
	olc::Pixel defaultForeGroundColor, defaultBackGroundColor;
	olc::Pixel currentForeGroundColor, currentBackGroundColor;

	olc::Pixel heldForegroundColor, heldBackgroundColor;

	olc::Pixel hoverForeGroundColor, hoverBackgroundColor;

	bool fill = true;
	bool border = true;

	int borderThickness = 1;


	Box(olc::PixelGameEngine* engine, int x = 0, int y = 0, int width = 0, int height = 0, olc::Pixel foreGroundColor = olc::WHITE, olc::Pixel backGroundColor = olc::BLACK)
		: Element(engine, x ,y, width, height)
	{
		this->defaultForeGroundColor = foreGroundColor;
		this->defaultBackGroundColor = backGroundColor;

		this->currentForeGroundColor = foreGroundColor;
		this->currentBackGroundColor = backGroundColor;

		this->heldForegroundColor = foreGroundColor;
		this->heldBackgroundColor = backGroundColor;

		this->hoverForeGroundColor = foreGroundColor;
		this->hoverBackgroundColor = backGroundColor;


	}

	void _OnPress(int mouse) override {}
	void _OnRelease(int mouse) override {}

	void _OnHeld(int mouse) override
	{
		currentBackGroundColor = heldBackgroundColor;
		currentForeGroundColor = heldForegroundColor;
	}

	void _OnHover() override
	{
		currentBackGroundColor = hoverBackgroundColor;
		currentForeGroundColor = hoverForeGroundColor;
	}

	virtual void Render()
	{ 
		if(fill)
			engine->FillRect(x, y, width, height, currentBackGroundColor);

		if(border)
			for (int i = borderThickness; i != 0; i += i < 0 ? 1 : -1)
				engine->DrawRect(x - i + 1, y - i + 1, width + i*2 - 2, height, currentForeGroundColor);//trial and error ftw

		currentBackGroundColor = defaultBackGroundColor;
		currentForeGroundColor = defaultForeGroundColor;
	}
};

class Button : public Box
{
public:
	olc::Pixel defaultTextColor;
	olc::Pixel currentTextColor;
	olc::Pixel onHoverTextColor;
	std::string text;
	int scale;
	bool XCenter = true;
	bool YCenter = true;

	Button(olc::PixelGameEngine* engine, std::string text,int scale = 1, int x = 0, int y = 0, int width = 0, int height = 0, olc::Pixel defaultTextColor = olc::WHITE, olc::Pixel foreGroundColor = olc::WHITE, olc::Pixel backGroundColor = olc::BLACK)
		: Box(engine, x, y, width, height, foreGroundColor, backGroundColor)
	{
		this->text = text;
		this->scale = scale;
		this->defaultTextColor = defaultTextColor;
		this->onHoverTextColor = defaultTextColor;
	}

	void _OnHover() override
	{
		currentTextColor = onHoverTextColor;
	}

	void Render() override
	{
		const int charSize = 8;

		Box::Render();

		int drawX = x;
		if(XCenter)
			drawX = x + width / 2 - (int)(charSize / 2 * text.length() * scale);
		int drawY = y;
		if (YCenter)
			drawY = y + height / 2 - (charSize * scale) / 2;

		engine->DrawString(drawX, drawY, text, currentTextColor,scale);

		currentTextColor = defaultTextColor;
	}
};