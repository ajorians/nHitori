#ifndef OPTIONS_H
#define OPTIONS_H

#define OPTIONS_USE_SOLID_COLOR

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

#include "Config.h"

enum Option_Direction{OP_Up, OP_Down};

class Options
{
public:
	Options(SDL_Surface* pScreen, Config* pConfig);
	~Options();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void ToggleCurrentOption();
	void Move(Option_Direction eDirection);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	int		m_nOptionsIndex;
#ifndef OPTIONS_USE_SOLID_COLOR
	SDL_Surface	*m_pBackground;
#endif
	nSDL_Font	*m_pFont;
	Config		*m_pConfig;
};

#endif
