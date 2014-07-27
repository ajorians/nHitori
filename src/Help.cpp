#include "Help.h"

HitoriHelp::HitoriHelp(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

HitoriHelp::~HitoriHelp()
{
	nSDL_FreeFont(m_pFont);
}

bool HitoriHelp::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool HitoriHelp::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
					case SDLK_RETURN:
					case SDLK_SPACE:
						return false;
					break;
					
					default:
						break;
				}

				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}
	return true;
}

void HitoriHelp::UpdateDisplay()
{
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));

	nSDL_DrawString(m_pScreen, m_pFont, 15, 20, 
"Hitori is a puzzle game.\n\
The object is to eliminate cells\n\
in the grid until there are no\n\
duplicates in any row or column.\n\
\n\
While blocking cells; just remember:\n\
1. No two blocks can be next to\n\
 another on a side (diagonal ok).\n\
2. No unblocked cell can be blocked\n\
 off from the rest of the cells.\n\
\n\
With solution helper:\n\
- Duplicates are gray until resolved\n\
- Adjacent blockings are yellow\n\
- Isolated cells are yellow\n\
\n\
And have fun!");		
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




