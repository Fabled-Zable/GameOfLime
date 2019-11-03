#pragma once
#include <vector>

class GOL
{

public:
	enum class EState
	{
		DEAD,
		ALIVE,
		WALL,
		ROOT,
		CREEP_ONE,
		CREEP_TWO
	};
private:
	std::vector<  std::vector<EState> > grid;

public:
	u_int height, width;
	GOL()
	{
		height = 0;
		width = 0;
	}
	GOL(u_int width, u_int height)
	{
		this->width = width;
		this->height = height;
		grid.resize(width);
		for (unsigned int i = 0; i < grid.size(); i++)
		{
			grid[i].resize(height);
		}
		
	}

	bool setCell(u_int x, u_int y, EState value)
	{
		if (x >= width || x < 0 || y >= height || y < 0)
			//std::cout << "Set cell out of range x:" << x << " y:" << y << "\n";
			return false;
		grid[x][y] = value;
		return true;
	}

	void shift(int shift_x, int shift_y)
	{
		std::vector< std::vector< EState > > newGrid;
		newGrid.resize(grid.size());
		for (unsigned int x = 0; x < grid.size(); x++)
		{
			newGrid[x].resize(grid[x].size());;
			for (unsigned int y = 0; y < grid[x].size(); y++)
			{
				newGrid[x][y] = grid[x][y];
			}
		}
		
		for(u_int x = 0; x < grid.size(); x++)
			for (u_int y = 0; y < grid[x].size(); y++)
			{
				long int newX = x + shift_x;
				long int newY = y + shift_y;
				if (newX >= 0 && newX < (long int)grid.size() && newY >= 0 && newY < (long int)grid[x].size())
				{
					newGrid[newX][newY] = grid[x][y];
				}
			}

		grid = newGrid;
	}

	void randomFill()
	{
		for(u_int x = 0; x < grid.size(); x++)
			for (u_int y = 0; y < grid[x].size(); y++)
			{
				if (rand() % 5 == 0)
					grid[x][y] = EState::ALIVE;
				else
				{
					grid[x][y] = EState::DEAD;
				}
			}
	}

	EState getCell(u_int x, u_int y)
	{
		if (x >= width || x < 0 || y >= height || y < 0)
		{
			//std::cout << "Get cell out of range x:" << x << " y:" << y << "\n";
			return EState::DEAD;
		}
		return grid[x][y];
	}

	bool isAlive(u_int x, u_int y)
	{
		EState cell = getCell(x, y);
		return cell == EState::ALIVE || cell == EState::ROOT || cell == EState::CREEP_ONE || cell == EState::CREEP_TWO;
	}

	short unsigned int getNeighborCount(u_int x, u_int y)
	{
		short unsigned int neighborCount = 0;

		if (isAlive(x, y + 1))//above
		{
			neighborCount++;
		}
		if (isAlive(x + 1, y + 1))//top right
		{
			neighborCount++;
		}
		if (isAlive(x - 1, y + 1))//top left
		{
			neighborCount++;
		}
		if (isAlive(x - 1, y))//left
		{
			neighborCount++;
		}
		if (isAlive(x + 1, y))//right
		{
			neighborCount++;
		}
		if (isAlive(x, y - 1))//below
		{
			neighborCount++;
		}
		if (isAlive(x + 1, y - 1))//bottom right
		{
			neighborCount++;
		}
		if (isAlive(x - 1, y - 1))//bottom left
		{
			neighborCount++;
		}
		return neighborCount;
	}

	void step ()
	{
		step([](u_int x, u_int y) -> void {});
	}

	void step(std::function<void(u_int x, u_int y)> cellChanged)
	{
		std::vector< std::vector< EState > > newGrid;
		newGrid.resize(grid.size());
		for (unsigned int x = 0; x < grid.size(); x++)
		{

			newGrid[x].resize(grid[x].size());;

			for (unsigned int y = 0; y < grid[x].size(); y++)
			{
				newGrid[x][y] = grid[x][y];
			}
		}
		for (u_int x = 0; x < width; x++)
			for (u_int y = 0; y < height; y++)
			{
				switch (grid[x][y])
				{
				case EState::DEAD:
				case EState::ALIVE:
				{
					short unsigned int neighborCount = getNeighborCount(x, y);
					if (neighborCount < 2)
					{
						newGrid[x][y] = EState::DEAD;
					}
					if (neighborCount > 3)
					{
						newGrid[x][y] = EState::DEAD;
					}
					if (neighborCount == 3)
					{
						newGrid[x][y] = EState::ALIVE;
					}
				}
				break;

				case EState::CREEP_ONE:
				case EState::CREEP_TWO:
				{
					
					if ((rand() % 2) == 0)
					{
						u_int new_x = x + rand() % 3 - 1;// -1 to 1
						u_int new_y = y + rand() % 3 - 1;// -1 to 1

						if(new_x >= 0 && new_y >= 0 && new_x < newGrid.size() && new_y < newGrid[new_x].size() && grid[new_x][new_y] != EState::WALL)
							newGrid[new_x][new_y] = grid[x][y] == EState::CREEP_ONE ? EState::CREEP_ONE : EState::CREEP_TWO;
					}
				}

				break;
				default:
					break;
				}
			}

		std::vector<u_int> changedCells;
		for (u_int x = 0; x < grid.size(); x++)
		{
			for (u_int y = 0; y < grid[x].size(); y++)
			{
				if (grid[x][y] != newGrid[x][y])
				{
					changedCells.push_back(x);
					changedCells.push_back(y);
					grid[x][y] = newGrid[x][y];
				}
			}
		}
		for (u_int i = 1; i < changedCells.size(); i += 2)
		{
			u_int x = changedCells[i - 1],
				y = changedCells[i];
			cellChanged(x, y);
		}
	}
};



