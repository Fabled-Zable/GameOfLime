#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "GOL.h"
#include <chrono>
#include <thread>

double range_lerp(double value, double min1, double max1, double min2, double max2)
{
	return ((value - min1) / (max1 - min1)) * (max2 - min2) + min2;
}

class Game : public olc::PixelGameEngine
{
	const int gps = 10;
public:
	enum class EGameState
	{
		START,
		RUNNING,
		MENU
	};

	enum EFps : u_int
	{
		PARTIAL = 15,
		FULL = 60,
		UNCAPPED = UINT_MAX
	};
	

	u_int fps = EFps::FULL;

	GOL grid;
	bool started = false;
	u_int lastMouseX = 0, lastMouseY = 0;
	Game()
	{
		 sAppName = "Game Of Lime";
	}
	EGameState GameState;
public:
	bool OnUserCreate() override
	{
		GameState = EGameState::START;
		grid = GOL(ScreenWidth() / gps, ScreenHeight() / gps);
		return true;
	}

	void DrawLife(u_int x, u_int y)
	{
		switch (grid.getCell(x, y))
		{
		case GOL::EState::ALIVE:
			FillRect(x*gps, y*gps,gps,gps, olc::GREEN);

			break;

		case GOL::EState::DEAD:
			FillRect(x * gps, y * gps, gps, gps, olc::BLACK);
			break;

		case GOL::EState::WALL:
			FillRect(x * gps, y * gps, gps, gps, olc::WHITE);
			break;

		case GOL::EState::ROOT:
			FillRect(x * gps, y * gps, gps, gps, olc::DARK_GREEN);
			break;
		case GOL::EState::CREEP_ONE:
			FillRect(x * gps, y * gps, gps, gps, olc::DARK_RED);
			break;
		case GOL::EState::CREEP_TWO:
			FillRect(x * gps, y * gps, gps, gps, olc::DARK_BLUE);
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

	bool OnUserUpdate(float fElapsedTime) override
	{
		if(fps != EFps::UNCAPPED)
			std::this_thread::sleep_for(std::chrono::milliseconds((int)((1000.0f / fps) - fElapsedTime)));// don't melt cpu

		switch (GameState)
		{
		case EGameState::START:
			fps = PARTIAL;
			DrawString(0, 0, "m1    step\nspace steps\ne     erase\nesc   genocide\nm     menu\nm2    gol\nc     creep1\nx     creep2\nw     wall\nr     root\nspace start", olc::GREEN,5);
			if (GetKey(olc::SPACE).bReleased == true)
			{
				FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
				GameState = EGameState::RUNNING;
			}
			return true;
			break;

		case EGameState::MENU:
			fps = EFps::PARTIAL;
			break;

		case EGameState::RUNNING:
			fps = EFps::FULL;
			int mouseGridPosX = GetMouseX()/gps;
			int mouseGridPosY = GetMouseY()/gps;

			if (mouseGridPosX != lastMouseX || mouseGridPosY != lastMouseY)
			{
				DrawLife(lastMouseX, lastMouseY);
			}

			FillRect(mouseGridPosX*gps,mouseGridPosY*gps,gps,gps, olc::GREY);
			if (GetMouse(0).bPressed || GetKey(olc::SPACE).bHeld)
			{
				grid.step([this](u_int x, u_int y) -> void {DrawLife(x, y); });
				//DrawAllLife();
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

			for (int i = 0; i < ScreenWidth(); i += gps)
			{
				DrawLine(i, 0, i, ScreenHeight(), olc::VERY_DARK_GREY);
			}
			for (int i = 0; i < ScreenHeight(); i += gps)
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

int main()
{
	Game game;
	if (game.Construct(1280, 720, 1, 1))
		game.Start();

	return 0;
}

/*
TODO:
* GUI
* Save/Load "creatures"
* make cell class?
* fix creep (spreads to left faster than right for some reason)
* fix grid (bigger than you can see?)
*/