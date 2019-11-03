#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "GOL.h"
#include "GUI.h"
#include <chrono>
#include <thread>

double range_lerp(double value, double min1, double max1, double min2, double max2)
{
	return ((value - min1) / (max1 - min1)) * (max2 - min2) + min2;
}

class Game : public olc::PixelGameEngine
{
	const int gridPixelSize = 10;
public:
	enum class GameState
	{
		START,
		RUNNING,
		MENU
	};

	enum Fps : u_int
	{
		PARTIAL = 15,
		FULL = 60,
		UNCAPPED = UINT_MAX
	};
	

	u_int fps = Fps::FULL;

	GOL grid;
	bool started = false;
	u_int lastMouseX = 0, lastMouseY = 0;
	Game()
	{
		 sAppName = "Game Of Lime";
	}
	GameState GameState;

public:
	void genocide()
	{
		for (u_int x = 0; x <= grid.width; x++)
			for (u_int y = 0; y <= grid.height; y++)
			{
				grid.setCell(x, y, GOL::EState::DEAD);
			}
		DrawAllLife();
	}

	void openHelpMenu()
	{
		SetPixelMode(olc::Pixel::Mode::ALPHA);
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::Pixel(32, 64, 32, 127));
		GameState = GameState::START;
		SetPixelMode(olc::Pixel::Mode::NORMAL);
	}

	void DrawLife(u_int x, u_int y)
	{
		olc::Pixel color;
		switch (grid.getCell(x, y))
		{
		case GOL::EState::ALIVE:
			color = olc::GREEN;
			break;
		case GOL::EState::DEAD:
			color = olc::BLACK;
			break;
		case GOL::EState::WALL:
			color = olc::WHITE;
			break;
		case GOL::EState::ROOT:
			color = olc::DARK_GREEN;
			break;
		case GOL::EState::CREEP_ONE:
			color = olc::DARK_RED;
			break;
		case GOL::EState::CREEP_TWO:
			color = olc::DARK_BLUE;
			break;
		}

		FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, color);
	}

	void DrawAllLife()
	{
		for (u_int x = 0; x < grid.width; x++)
			for (u_int y = 0; y < grid.height; y++)
			{
				DrawLife(x, y);
			}
	}

	std::vector<Button*> infoBoxes;
	std::vector<Button*> inGameGui;
	Button* startButton;
	CheckBox* autoStep;

	bool OnUserCreate() override
	{
		GameState = GameState::START;
		grid = GOL(ScreenWidth() / gridPixelSize, ScreenHeight() / gridPixelSize - 10);
		grid.randomFill();

		std::vector<std::string> infos = { " M1        STEP", " SPACE    STEPS", " E        ERASE"," ESC   GENOCIDE"," H         HELP"," M2         GOL"," C       CREEP1"," X       CREEP2"," W         WALL"," R         ROOT"};

		ButtonStyle infoBoxStyle;
		infoBoxStyle.boxStyle.borderThickness = 3;
		infoBoxStyle.XCenter = false;
		infoBoxStyle.textScale = 2;
		infoBoxStyle.boxStyle.borderThickness = 3;
		infoBoxStyle.defaultTextColor = olc::YELLOW;
		infoBoxStyle.boxStyle.defaultForeGroundColor = olc::GREY;
		infoBoxStyle.boxStyle.defaultBackGroundColor = olc::BLACK;


		for (u_int i = 0; i < infos.size(); i++)
		{
			Button* button = new Button(this, infoBoxStyle, infos[i], 20, 20 * (i + 1) + (i * 50), 250, 50);// , olc::YELLOW, olc::GREY, olc::BLACK);
			infoBoxes.push_back(button);
		}

		ButtonStyle startButtonStyle;
		startButtonStyle.boxStyle.heldBackgroundColor = olc::DARK_GREEN;
		startButtonStyle.boxStyle.heldForegroundColor = olc::DARK_GREY;
		startButtonStyle.boxStyle.borderThickness = 5;
		startButtonStyle.boxStyle.defaultForeGroundColor = olc::GREY;
		startButtonStyle.boxStyle.defaultBackGroundColor = olc::GREEN;
		startButtonStyle.defaultTextColor = olc::YELLOW;
		startButtonStyle.hoverTextColor = olc::WHITE;
		startButtonStyle.textScale = 4;

		startButton = new Button(this,startButtonStyle,"START!",ScreenWidth()/2,ScreenHeight()/2, 6 * 8 * 4 + 20, 64);
		startButton->boxStyle.borderThickness = 5;
		startButton->onPress = [this](int mouse) -> void {
			GameState = GameState::RUNNING; 
			FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK); 
			DrawAllLife();
		};

		ButtonStyle inGameGuiStyle;
		inGameGuiStyle.defaultTextColor = olc::YELLOW;
		inGameGuiStyle.hoverTextColor = olc::WHITE;
		inGameGuiStyle.textScale = 2;

		inGameGuiStyle.boxStyle.defaultBackGroundColor = olc::DARK_GREEN;
		inGameGuiStyle.boxStyle.defaultForeGroundColor = olc::GREY;

		inGameGuiStyle.boxStyle.heldBackgroundColor = olc::VERY_DARK_MAGENTA;
		inGameGuiStyle.boxStyle.heldForegroundColor = olc::GREY;
		
		{
			Button* button = new Button(this, inGameGuiStyle, "Clear");
			button->onRelease = [this](int mouse) -> void {genocide();};
			inGameGui.push_back(button);
		}
		{
			Button* button = new Button(this, inGameGuiStyle, "Help");
			button->onRelease = [this](int mouse) -> void {openHelpMenu();};
			inGameGui.push_back(button);
		}
		{
			Button* button = new Button(this, inGameGuiStyle, "Repopulate");
			button->onRelease = [this](int mouse) -> void {grid.randomFill(); DrawAllLife(); };
			inGameGui.push_back(button);
		}

		int lastButtonX = 0;

		for (u_int i = 0; i < inGameGui.size(); i++)
		{
			Button* button = inGameGui[i];

			button->width = 200;
			button->height = 30;

			button->y = ScreenHeight() - (5 * gridPixelSize) - (button->height/2);
			button->x = button->width * i + (20 * i) + 20;
			lastButtonX = button->x + button->width;
		}

		BoxStyle autoStepStyle;
		autoStepStyle.defaultForeGroundColor = olc::WHITE;
		autoStepStyle.defaultBackGroundColor = olc::GREEN;
		autoStepStyle.hoverBackgroundColor = autoStepStyle.defaultBackGroundColor;
		autoStepStyle.hoverForeGroundColor = autoStepStyle.defaultForeGroundColor;
		autoStepStyle.heldForegroundColor = autoStepStyle.defaultForeGroundColor;
		autoStepStyle.heldBackgroundColor = autoStepStyle.defaultBackGroundColor;

		autoStep = new CheckBox(this, autoStepStyle, true, 20 + lastButtonX, ScreenHeight() - (5 * gridPixelSize) - 15, 30, 30);

		return true;
	}
	u_int ticks = 0;
	bool OnUserUpdate(float fElapsedTime) override
	{
		
		sAppName = "Game Of Lime - FPS " + std::to_string(nFrameCount) + " : " + std::to_string(fps);

		if(fps != Fps::UNCAPPED)
			std::this_thread::sleep_for(std::chrono::milliseconds((int)((1000.0f / fps) - fElapsedTime)));// don't melt cpu

		ticks++;

		switch (GameState)
		{
		case GameState::START:
			fps = PARTIAL;

			for (u_int i = 0; i < infoBoxes.size(); i++)
			{
				infoBoxes[i]->Render();
			}
			startButton->boxStyle.defaultBackGroundColor.g = (uint8_t)range_lerp(sin(ticks),-1,1,127,200);
			startButton->boxStyle.hoverBackgroundColor = startButton->boxStyle.defaultBackGroundColor;
			startButton->Render();
			startButton->Poll();

			return true;
			break;

		case GameState::MENU:
			fps = Fps::PARTIAL;
			break;

		case GameState::RUNNING:
			if (IsFocused())
				fps = Fps::FULL;
			else
				fps = Fps::PARTIAL;
			int mouseGridPosX = GetMouseX()/gridPixelSize;
			int mouseGridPosY = GetMouseY()/gridPixelSize;

			if (mouseGridPosX != lastMouseX || mouseGridPosY != lastMouseY)
			{
				DrawLife(lastMouseX, lastMouseY);
			}

			FillRect(mouseGridPosX*gridPixelSize,mouseGridPosY*gridPixelSize,gridPixelSize,gridPixelSize, olc::GREY);
			if (GetMouseY() < ScreenHeight() - (10 * gridPixelSize) || autoStep->checked)
			{
				if (GetMouse(0).bPressed || GetKey(olc::SPACE).bHeld || autoStep->checked)
				{
					if (ticks % 5 == 0 || GetMouse(0).bPressed)
						grid.step([this](u_int x, u_int y) -> void {DrawLife(x, y); });
				}
				if (GetMouse(1).bHeld)
				{
					grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::ALIVE);
					DrawAllLife();

				}
			}
			if (GetKey(olc::H).bReleased)
			{
				openHelpMenu();
			}
			if (GetKey(olc::W).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::WALL);
				DrawAllLife();

			}
			if (GetKey(olc::R).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::ROOT);
				DrawAllLife();

			}
			if (GetKey(olc::E).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::DEAD);
				DrawAllLife();

			}
			if (GetKey(olc::C).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::CREEP_ONE);
				DrawAllLife();

			}
			if (GetKey(olc::X).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::CREEP_TWO);
				DrawAllLife();

			}
			if (GetKey(olc::ESCAPE).bHeld)
			{
				genocide();
			}

			if (GetKey(olc::UP).bHeld)
			{
				grid.shift(0, -1);
				DrawAllLife();
			}
			if (GetKey(olc::DOWN).bHeld)
			{
				grid.shift(0, 1);
				DrawAllLife();
			}
			if (GetKey(olc::LEFT).bHeld)
			{
				grid.shift(-1, 0);
				DrawAllLife();
			}
			if (GetKey(olc::RIGHT).bHeld)
			{
				grid.shift(1, 0);
				DrawAllLife();
			}

			for (int i = 0; i < ScreenWidth(); i += gridPixelSize)
			{
				DrawLine(i, 0, i, ScreenHeight() - (10 * gridPixelSize), olc::VERY_DARK_GREY);
			}
			for (int i = 0; i < ScreenHeight() - (9 * gridPixelSize); i += gridPixelSize)
			{
				DrawLine(0, i, ScreenWidth(), i, olc::VERY_DARK_GREY);
			}
			lastMouseX = mouseGridPosX;
			lastMouseY = mouseGridPosY;

			for (u_int i = 0; i < inGameGui.size(); i++)
			{
				inGameGui[i]->Render();
				inGameGui[i]->Poll();
			}

			autoStep->Render();
			autoStep->Poll();
			break;
		}

		return true;
	}
};

class Test : public olc::PixelGameEngine
{
public:
	CheckBox* checkBox;

	Test()
	{
		sAppName = "Testing stuff";
	}

	bool OnUserCreate() override
	{
		BoxStyle boxStyle;
		boxStyle.defaultForeGroundColor = olc::WHITE;
		boxStyle.defaultBackGroundColor = olc::GREEN;
		boxStyle.hoverBackgroundColor = boxStyle.defaultBackGroundColor;
		boxStyle.hoverForeGroundColor = boxStyle.defaultForeGroundColor;
		boxStyle.heldForegroundColor = boxStyle.defaultForeGroundColor;
		boxStyle.heldBackgroundColor = boxStyle.defaultBackGroundColor;

		checkBox = new CheckBox(this, boxStyle,true,20,20,200,200);
		return true;

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		sAppName = "Testing stuff - " + std::to_string(nFrameCount);

		FillRect(0, 0, ScreenWidth(), ScreenHeight(),olc::VERY_DARK_MAGENTA);
		checkBox->Render();
		checkBox->Poll();

		


		return true;
	}
};


int main()
{
	Game game;
	if (game.Construct(1280, 720, 1, 1))
		game.Start();

	//Test test;
	//if (test.Construct(1280, 720, 1, 1))
	//	test.Start();

	return 0;
}