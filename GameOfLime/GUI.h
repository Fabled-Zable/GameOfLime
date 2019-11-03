//#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


struct BoxStyle
{
	olc::Pixel defaultForeGroundColor, defaultBackGroundColor;
	olc::Pixel currentForeGroundColor, currentBackGroundColor;
	olc::Pixel heldForegroundColor, heldBackgroundColor;
	olc::Pixel hoverForeGroundColor, hoverBackgroundColor;
	bool fill = true;
	bool border = true;
	int borderThickness = 1;
};

struct ButtonStyle
{
	BoxStyle boxStyle;
	olc::Pixel defaultTextColor;
	olc::Pixel currentTextColor;
	olc::Pixel hoverTextColor;
	int textScale = 1;
	bool XCenter = true;
	bool YCenter = true;
};

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
	//olc::Pixel defaultForeGroundColor, defaultBackGroundColor;
	//olc::Pixel currentForeGroundColor, currentBackGroundColor;

	//olc::Pixel heldForegroundColor, heldBackgroundColor;

	//olc::Pixel hoverForeGroundColor, hoverBackgroundColor;

	//bool fill = true;
	//bool border = true;

	//int borderThickness = 1;

	BoxStyle boxStyle;


	Box(olc::PixelGameEngine* engine, int x = 0, int y = 0, int width = 0, int height = 0, olc::Pixel foreGroundColor = olc::WHITE, olc::Pixel backGroundColor = olc::BLACK)
		: Element(engine, x ,y, width, height)
	{
		this->boxStyle.defaultForeGroundColor = foreGroundColor;
		this->boxStyle.defaultBackGroundColor = backGroundColor;

		this->boxStyle.currentForeGroundColor = foreGroundColor;
		this->boxStyle.currentBackGroundColor = backGroundColor;

		this->boxStyle.heldForegroundColor = foreGroundColor;
		this->boxStyle.heldBackgroundColor = backGroundColor;

		this->boxStyle.hoverForeGroundColor = foreGroundColor;
		this->boxStyle.hoverBackgroundColor = backGroundColor;


	}

	Box(olc::PixelGameEngine* engine, BoxStyle boxStyle, int x = 0, int y = 0, int width = 0, int height = 0)
		: Element(engine, x, y, width, height)
	{
		this->boxStyle = boxStyle;
	}

	void _OnPress(int mouse) override {}
	void _OnRelease(int mouse) override {}

	void _OnHeld(int mouse) override
	{
		boxStyle.currentBackGroundColor = boxStyle.heldBackgroundColor;
		boxStyle.currentForeGroundColor = boxStyle.heldForegroundColor;
	}

	void _OnHover() override
	{
		boxStyle.currentBackGroundColor = boxStyle.hoverBackgroundColor;
		boxStyle.currentForeGroundColor = boxStyle.hoverForeGroundColor;
	}

	virtual void Render()
	{ 
		renderFill();

		renderBorder();

		boxStyle.currentBackGroundColor = boxStyle.defaultBackGroundColor;
		boxStyle.currentForeGroundColor = boxStyle.defaultForeGroundColor;
	}

private:
	virtual void renderFill()
	{
		if (boxStyle.fill)
			engine->FillRect(x, y, width, height, boxStyle.currentBackGroundColor);
	}
	virtual void renderBorder()
	{
		if (boxStyle.border)
			for (int i = boxStyle.borderThickness; i != 0; i += i < 0 ? 1 : -1)
				engine->DrawRect(x - i + 1, y - i + 1, width + i * 2 - 2, height, boxStyle.currentForeGroundColor);//trial and error ftw
	}
};

class Button : public Box
{
public:
	std::string text;

	ButtonStyle buttonStyle;

	Button(olc::PixelGameEngine* engine, std::string text,int scale = 1, int x = 0, int y = 0, int width = 0, int height = 0, olc::Pixel defaultTextColor = olc::WHITE, olc::Pixel foreGroundColor = olc::WHITE, olc::Pixel backGroundColor = olc::BLACK)
		: Box(engine, x, y, width, height, foreGroundColor, backGroundColor)
	{
		this->text = text;
		this->buttonStyle.textScale = scale;
		this->buttonStyle.defaultTextColor = defaultTextColor;
		this->buttonStyle.hoverTextColor = defaultTextColor;
	}

	Button(olc::PixelGameEngine* engine, ButtonStyle buttonStyle, std::string text = "", int x = 0, int y = 0, int width = 0, int height = 0)
		: Box(engine, buttonStyle.boxStyle, x, y, width, height)
	{
		this->text = text;
		this->buttonStyle = buttonStyle;
	}

	void _OnHover() override
	{
		buttonStyle.currentTextColor = buttonStyle.hoverTextColor;
	}

	void Render() override
	{
		const int charSize = 8;

		Box::Render();

		int drawX = x;
		if(buttonStyle.XCenter)
			drawX = x + width / 2 - (int)(charSize / 2 * text.length() * buttonStyle.textScale);
		int drawY = y;
		if (buttonStyle.YCenter)
			drawY = y + height / 2 - (charSize * buttonStyle.textScale) / 2;

		engine->DrawString(drawX, drawY, text, buttonStyle.currentTextColor, buttonStyle.textScale);

		buttonStyle.currentTextColor = buttonStyle.defaultTextColor;
	}
};

class CheckBox : public Box
{
public:
	bool checked = true;
	int innerWidth = 4;
	int innerHeight = 4;
	olc::Pixel unCheckedColor = olc::BLACK;

	CheckBox(olc::PixelGameEngine* engine, BoxStyle boxStyle, bool checked = true, int x = 0, int y = 0, int width = 0, int height = 0)
		: Box(engine, boxStyle,x,y,width,height) 
	{
		this->checked = checked;
	}

	void renderFill() override
	{
		if (boxStyle.fill)
			engine->FillRect(x + innerWidth, y + innerHeight, width - innerWidth * 2, height - innerHeight * 2, checked ? boxStyle.currentBackGroundColor : unCheckedColor);
	}

	void _OnPress(int mouse) override
	{
		checked = !checked;
	}

};

