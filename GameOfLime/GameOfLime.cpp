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

	void DrawLife(u_int x, u_int y)
	{
		switch (grid.getCell(x, y))
		{
		case GOL::EState::ALIVE:
			FillRect(x*gridPixelSize, y*gridPixelSize,gridPixelSize,gridPixelSize, olc::GREEN);

			break;

		case GOL::EState::DEAD:
			FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, olc::BLACK);
			break;

		case GOL::EState::WALL:
			FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, olc::WHITE);
			break;

		case GOL::EState::ROOT:
			FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, olc::DARK_GREEN);
			break;
		case GOL::EState::CREEP_ONE:
			FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, olc::DARK_RED);
			break;
		case GOL::EState::CREEP_TWO:
			FillRect(x * gridPixelSize, y * gridPixelSize, gridPixelSize, gridPixelSize, olc::DARK_BLUE);
			break;
		}
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
	Button* startButton;

	bool OnUserCreate() override
	{
		GameState = GameState::START;
		grid = GOL(ScreenWidth() / gridPixelSize, ScreenHeight() / gridPixelSize - 10);
		grid.randomFill();

		std::vector<std::string> infos = { " M1        STEP", " SPACE    STEPS", " E        ERASE"," ESC   GENOCIDE"," M         MENU"," M2         GOL"," C       CREEP1"," X       CREEP2"," W         WALL"," R         ROOT"};

		for (u_int i = 0; i < infos.size(); i++)
		{
			Button* button = new Button(this, infos[i], 2, 20, 20 * (i + 1) + (i * 50), 250, 50, olc::YELLOW, olc::GREY, olc::BLACK);
			button->borderThickness = 3;
			button->XCenter = false;
			infoBoxes.push_back(button);
		}

		startButton = new Button(this,"START!",4,ScreenWidth()/2,ScreenHeight()/2, 6 * 8 * 4 + 20, 64, olc::YELLOW,olc::GREY,olc::GREEN);
		startButton->heldBackgroundColor = olc::DARK_GREEN;
		startButton->heldForegroundColor = olc::DARK_GREY;
		startButton->onHoverTextColor = olc::WHITE;
		startButton->borderThickness = 5;
		startButton->onPress = [this](int mouse) -> void {
			GameState = GameState::RUNNING; 
			FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK); 
			DrawAllLife();
		};

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
			startButton->defaultBackGroundColor.g = range_lerp(sin(ticks),-1,1,127,200);
			startButton->hoverBackgroundColor = startButton->defaultBackGroundColor;
			startButton->Poll();
			startButton->Render();

			/*DrawString(0, 0, "\n\n\n\n\n\n\n\n\n\nspace start", olc::GREEN,5);
			if (GetKey(olc::SPACE).bReleased == true)
			{
				FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
				GameState = GameState::RUNNING;
			}*/
			return true;
			break;

		case GameState::MENU:
			fps = Fps::PARTIAL;
			break;

		case GameState::RUNNING:
			fps = Fps::FULL;
			int mouseGridPosX = GetMouseX()/gridPixelSize;
			int mouseGridPosY = GetMouseY()/gridPixelSize;

			if (mouseGridPosX != lastMouseX || mouseGridPosY != lastMouseY)
			{
				DrawLife(lastMouseX, lastMouseY);
			}

			FillRect(mouseGridPosX*gridPixelSize,mouseGridPosY*gridPixelSize,gridPixelSize,gridPixelSize, olc::GREY);
			if (GetMouse(0).bPressed || GetKey(olc::SPACE).bHeld)
			{
				if(ticks%5 == 0 || GetMouse(0).bPressed)
					grid.step([this](u_int x, u_int y) -> void {DrawLife(x, y); });
			}
			if (GetMouse(1).bHeld)
			{
				grid.setCell(mouseGridPosX, mouseGridPosY, GOL::EState::ALIVE);
				DrawAllLife();

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
				for (u_int x = 0; x <= grid.width; x++)
					for (u_int y = 0; y <= grid.height; y++)
					{
						grid.setCell(x, y, GOL::EState::DEAD);
					}
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
			break;
		}

		return true;
	}
};

class Test : public olc::PixelGameEngine
{
public:
	Button* button;

	Test()
	{
		sAppName = "Testing stuff";
	}

	bool OnUserCreate() override
	{
		button = new Button(this,"Hello World!",1,ScreenWidth()/2 - 100,ScreenHeight()/2-100,200,100,olc::WHITE,olc::WHITE,olc::DARK_GREEN);
		button->hoverBackgroundColor = olc::BLUE;
		button->heldBackgroundColor = olc::DARK_BLUE;
		button->borderThickness = 1;

		return true;

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		FillRect(0, 0, ScreenWidth(), ScreenHeight(),olc::VERY_DARK_MAGENTA);
		Draw(GetMouseX(), GetMouseY());

		//button->Poll();
		//button->Render();
		return true;
	}
};


int main()
{
	Game game;
	if (game.Construct(1280, 720, 1, 1))
		game.Start();;

	return 0;
}