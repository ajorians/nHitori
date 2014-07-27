#ifndef GAME_H
#define GAME_H

//#define USE_GRAPHIC_YOU_WIN//Use a static "You Win!!!" image instead of the animated one

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HitoriLib/HitoriLib.h"
}

#ifndef USE_GRAPHIC_YOU_WIN
#include "YouWinMessage.h"
#endif

#include "Config.h"
#include "ColorIntensity.h"
#include "Selector.h"
#include "Defines.h"

class Game
{
public:
	Game(SDL_Surface* pScreen, const char* pstrFile, int nLevelNumber, Config* pConfig);
	~Game();

	bool Loop();
	
protected:
	bool PollEvents();
	void DrawMaybeMarkedCell(SDL_Rect& rect);
	void DrawIsolatedSpot(SDL_Rect& rect);
	void DrawAdjacentMarkedSpot(SDL_Rect& rect);
	void DrawMarkedSpot(SDL_Rect& rect);
	void DrawCell(int nX, int nY, int nLeft, int nTop, int nPieceSize);
	void DrawSelector(int nLeft, int nTop, int nPieceSize);
	void UpdateDisplay();
	void Move(Direction eDirection);
	void ToggleMarking();
	bool IsAdjacentMarked(int nX, int nY);
	bool IsMaybeMarked(int nX, int nY);
	bool IsIsolatedSpot(int nX, int nY);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pBackground;
#ifdef USE_GRAPHIC_YOU_WIN
	SDL_Surface 	*m_pWinGraphic;
#else
	YouWinMessage    m_YouWinMessage;
#endif
	nSDL_Font 	*m_pFont;
	HitoriLib	m_Hitori;
	int		m_nLevelNumber;
	Config		*m_pConfig;//Does not own
	Selector	m_Selector;
	bool		m_bGameOver;
        ColorIntensity	m_ColorIntensity;
};

#endif
