#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class GameOfLife : public olc::PixelGameEngine
{
public:
	GameOfLife()
	{
		sAppName = "Game of Life";
	}

	int tileWidth = 10;
	int tileHeight = 10;
	int roomWidth = 100;
	int roomHeight = 100;

	std::unique_ptr<bool[]> room;
	std::unique_ptr<bool[]> nextRoom;

	olc::vi2d vTilesOnScreen;
	olc::vi2d vTopLeftTile;
	olc::vi2d vHighlightPos;

	bool autoAdvance;
	float fTicRate;
	float fTimeSoFar;

public:
	void advance() {
		for (int i = 0; i < roomWidth * roomHeight; i++)
		{
			nextRoom[i] = getNextLifeStatus(i);
		}

		room.swap(nextRoom);
	}

	bool getNextLifeStatus(int i) {
		int n = getNeighborCount(i);

		if (room[i])
			return (n == 2 || n == 3);
		else
			return n == 3;
	}

	int getNeighborCount(int i) {
		int n = 0;

		int xStart = (i % roomWidth == 0) ? 0 : -1;
		int xEnd = (i % roomWidth == roomWidth - 1) ? 0 : 1;
		int yStart = (i < roomWidth) ? 0 : -1;
		int yEnd = (i > roomWidth * roomHeight - roomWidth) ? 0 : 1;
		for (int x = xStart; x <= xEnd; x++)
		{
			for (int y = yStart; y <= yEnd; y++)
			{
				int l = y * roomWidth + x + i;
				if (l != i)
					n += room[l];
			}
		}

		return n;
	}

	bool OnUserCreate() override
	{
		vTilesOnScreen = { ScreenWidth() / tileWidth, ScreenHeight() / tileHeight };
		vHighlightPos = { 0, 0 };
		vTopLeftTile = { 0, 0 };

		room = std::make_unique<bool[]>(roomWidth * roomHeight);
		nextRoom = std::make_unique<bool[]>(roomWidth * roomHeight);

		autoAdvance = false;
		fTicRate = 0.5f;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		auto GetTile = [&](olc::vi2d& v) {
			return room[v.y * roomWidth + v.x];
		};

		auto SetTile = [&](olc::vi2d& v, bool b) {
			room[v.y * roomWidth + v.x] = b;
		};

		// input
		if (GetKey(olc::LEFT).bPressed && vHighlightPos.x > 0)
			vHighlightPos.x--;
		if (GetKey(olc::UP).bPressed && vHighlightPos.y > 0)
			vHighlightPos.y--;
		if (GetKey(olc::RIGHT).bPressed)
			vHighlightPos.x++;
		if (GetKey(olc::DOWN).bPressed)
			vHighlightPos.y++;
		if (GetKey(olc::A).bPressed)
			SetTile(vHighlightPos, !GetTile(vHighlightPos));
		if (GetKey(olc::SPACE).bPressed)
		{
			advance();
			autoAdvance = false;
		}
		if (GetKey(olc::ENTER).bPressed)
		{
			autoAdvance = !autoAdvance;
			fTimeSoFar = fTicRate;
		}
		if (GetKey(olc::Q).bPressed)
		{
			for (int i = 0; i < roomWidth * roomHeight; i++)
			{
				room[i] = false;
			}
		}

		// Draw Screen
		Clear(olc::BLACK);
		for (int x = 0; x < vTilesOnScreen.x + 1; x++)
		{
			for (int y = 0; y < vTilesOnScreen.y + 1; y++)
			{
				FillRect(x * tileWidth, y * tileHeight, tileWidth - 1, tileHeight - 1,
					(room[y * roomWidth + x]) ? olc::WHITE : olc::DARK_GREEN);
			}
		}
		FillRect((vHighlightPos - vTopLeftTile) * tileWidth, olc::vi2d(tileWidth - 1, tileHeight - 1),
			GetTile(vHighlightPos) ? olc::Pixel(192, 192, 0) : olc::YELLOW);

		if (autoAdvance)
		{
			fTimeSoFar += fElapsedTime;
			if (fTimeSoFar > fTicRate)
			{
				fTimeSoFar -= fTicRate;
				advance();
			}
		}

		return true;
	}
};


int main()
{
	GameOfLife demo;
	if (demo.Construct(400, 300, 2, 2))
		demo.Start();

	return 0;
}
