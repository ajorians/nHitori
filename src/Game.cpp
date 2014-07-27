#include "Game.h"
#include "GameBackground.h"
#ifdef USE_GRAPHIC_YOU_WIN
#include "YouWinGraphic.h"
#endif

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, const char* pstrFile, int nLevelNumber, Config* pConfig)
: m_pScreen(pScreen)
#ifdef USE_GRAPHIC_YOU_WIN
, m_pWinGraphic(NULL)
#else
, m_YouWinMessage(pScreen)
#endif
, m_nLevelNumber(nLevelNumber), m_pConfig(pConfig), m_Selector(pScreen, pConfig), m_bGameOver(false)
{
	HitoriLibCreate(&m_Hitori, pstrFile);
	m_Selector.SetDimensions(GetHitoriBoardWidth(m_Hitori), GetHitoriBoardHeight(m_Hitori));
	m_pBackground = nSDL_LoadImage(image_HitoriBoard);
	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);
}

Game::~Game()
{
	HitoriLibFree(&m_Hitori);
	SDL_FreeSurface(m_pBackground);
#ifdef USE_GRAPHIC_YOU_WIN
	if( m_pWinGraphic != NULL )
		SDL_FreeSurface(m_pWinGraphic);
#endif
	nSDL_FreeFont(m_pFont);
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	//Update screen
	UpdateDisplay();
	
	SDL_Delay(30);
	
	return true;
}

bool Game::PollEvents()
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
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                        case SDLK_RCTRL:
                                                ToggleMarking();
                                        break;
					
					case SDLK_UP:
                                        case SDLK_8:
                                                Move(Up);
        	                                break;
                                        
                                        case SDLK_DOWN:
                                        case SDLK_2:
                                                Move(Down);
	                                        break;

					case SDLK_RIGHT:
					case SDLK_6:
						Move(Right);
						break;
					
					case SDLK_LEFT:
					case SDLK_4:
						Move(Left);
						break;

					case SDLK_PLUS:
						if( !m_bGameOver ) HitoriRedo(m_Hitori);
						break;

					case SDLK_MINUS:
						if( !m_bGameOver ) HitoriUndo(m_Hitori);
						break;

					case SDLK_TAB:
						if( !m_bGameOver ) SetHitoriToSolved(m_Hitori);
						if( IsHitoriSolved(m_Hitori) == HITORILIB_SOLVED )
							m_bGameOver = true;
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

bool Game::IsAdjacentMarked(int nX, int nY)
{
   if( nX != 0 ) {
      if( IsHitoriSpotMarked(m_Hitori, nX-1, nY) )
         return true;
   }

   if( nY != 0 ) {
      if( IsHitoriSpotMarked(m_Hitori, nX, nY-1) )
         return true;
   }

   if( nX != (GetHitoriBoardWidth(m_Hitori)-1) ) {
      if( IsHitoriSpotMarked(m_Hitori, nX+1, nY) )
         return true;
   }

   if( nY != (GetHitoriBoardHeight(m_Hitori)-1) ) {
      if( IsHitoriSpotMarked(m_Hitori, nX, nY+1) )
         return true;
   }

   return false;
}

bool Game::IsMaybeMarked(int nX, int nY)
{
   for(int nX2=0; nX2<GetHitoriBoardWidth(m_Hitori); nX2++) {
      if( (nX2 != nX) && (!IsHitoriSpotMarked(m_Hitori, nX2, nY)) && (GetHitoriSpotValue(m_Hitori, nX, nY) == GetHitoriSpotValue(m_Hitori, nX2, nY)) ) {
         return true;
      }
   }

   for(int nY2=0; nY2<GetHitoriBoardHeight(m_Hitori); nY2++) {
      if( (nY2 != nY) && (!IsHitoriSpotMarked(m_Hitori, nX, nY2)) && (GetHitoriSpotValue(m_Hitori, nX, nY) == GetHitoriSpotValue(m_Hitori, nX, nY2)) ) {
         return true;
      }
   }

   return false;
}

bool Game::IsIsolatedSpot(int nX, int nY) {
   return IsHitoriIsolatedSpot(m_Hitori, nX, nY) == HITORILIB_ISOLATED;
}

void Game::DrawMaybeMarkedCell(SDL_Rect& rect)
{
   if( is_classic ) {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 192, 192, 192));
    } else {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 127, 127, 127));
    }
}

void Game::DrawIsolatedSpot(SDL_Rect& rect)
{
   if( is_classic ) {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 96, 96, 96));
   } else {
      int r=255, g=255, b=0, a=255;
      m_ColorIntensity.AdjustColor(r,g,b,a);
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, r, g, b));
   }
}

void Game::DrawAdjacentMarkedSpot(SDL_Rect& rect)
{
   if( is_classic ) {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 96, 96, 96));
   } else {
      int r=255, g=255, b=0, a=255;
      m_ColorIntensity.AdjustColor(r,g,b,a);
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, r, g, b));
   }
}

void Game::DrawMarkedSpot(SDL_Rect& rect)
{
   if( is_classic ) {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 160, 160, 160));
   } else {
      SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 0, 0, 255));
   }
}

void Game::DrawCell(int nX, int nY, int nLeft, int nTop, int nPieceSize)
{
   //if( nX == m_nCurrentX && nY == m_nCurrentY )
      //return;

   SDL_Rect rect;
   rect.x = nLeft + nX*nPieceSize;
   rect.y = nTop + nY*nPieceSize;
   rect.w = nPieceSize;
   rect.h = nPieceSize;
   if( IsHitoriSpotMarked(m_Hitori, nX, nY) ) {
      if( IsAdjacentMarked(nX, nY) && m_pConfig->GetSolutionHelper() ) {
         DrawAdjacentMarkedSpot(rect);
      } else {
         DrawMarkedSpot(rect);
      }
   } else{
      if( m_pConfig->GetSolutionHelper() ) {
         if( IsIsolatedSpot(nX, nY) ) {
             DrawIsolatedSpot(rect);
         } else if( IsMaybeMarked(nX, nY) ) {
             DrawMaybeMarkedCell(rect);
         }
      }
   }

   int nValue = GetHitoriSpotValue(m_Hitori, nX, nY);
   char buffer[16];
   Hit_itoa(nValue, buffer, 16);
   int nIndent = (nPieceSize - nSDL_GetStringWidth(m_pFont, buffer))/2;
   nSDL_DrawString(m_pScreen, m_pFont, nLeft + nX*nPieceSize + nIndent, nTop + nY*nPieceSize + nPieceSize/2, buffer );
}

void Game::DrawSelector(int nLeft, int nTop, int nPieceSize)
{
   m_Selector.DrawSelector(nLeft, nTop, nPieceSize);
}

void Game::UpdateDisplay()
{
	m_ColorIntensity.Animate();

	//Draw background
	if( !is_classic ) {
		SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	} else {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
	}

	int nWidth = GetHitoriBoardWidth(m_Hitori);
	int nHeight = GetHitoriBoardHeight(m_Hitori);

	int nPieceWidth = (SCREEN_WIDTH-1)/nWidth;//The -1 is just so range of pixels is 0->Dimension-1; instead of 0->Dimension
	int nPieceHeight = (SCREEN_HEIGHT-1)/nHeight;

        int nPieceSize = nPieceWidth < nPieceHeight ? nPieceWidth : nPieceHeight;

	int nBoardWidth = nPieceSize * nWidth;
	int nBoardHeight = nPieceSize * nHeight;

	int nTop = (SCREEN_HEIGHT - nBoardHeight) / 2;
	int nLeft = (SCREEN_WIDTH - nBoardWidth) / 2;

	/*SDL_Rect rectBoard;
	rectBoard.x = nLeft;
	rectBoard.y = nTop;
	rectBoard.w = nBoardWidth;
	rectBoard.h = nBoardHeight;
	SDL_FillRect(m_pScreen, &rectBoard, SDL_MapRGB(m_pScreen->format, GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B));*/
	boxRGBA(m_pScreen, nLeft, nTop, nLeft + nBoardWidth, nTop + nBoardHeight,  GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B, 180);

	for(int nX=0; nX<=nWidth; nX++) {
		vlineRGBA(m_pScreen, nLeft + nX*nPieceSize, nTop, nTop + nBoardHeight, 0, 0, 0, 255);
	}

	for(int nY=0; nY<=nHeight; nY++) {
		hlineRGBA(m_pScreen, nLeft, nLeft + nBoardWidth, nTop + nY*nPieceSize, 0, 0, 0, 255);
	}

	for(int nX=0; nX<nWidth; nX++) {
                for(int nY=0; nY<nHeight; nY++) {
			DrawCell(nX, nY, nLeft, nTop, nPieceSize);
                }
        }

        DrawSelector(nLeft, nTop, nPieceSize);

#ifdef USE_GRAPHIC_YOU_WIN
	if( m_bGameOver ) {
		if( m_pWinGraphic == NULL ) {
		m_pWinGraphic = nSDL_LoadImage(image_HitoriYouWin);
                SDL_SetColorKey(m_pWinGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pWinGraphic->format, 255, 255, 255));
		}

		SDL_Rect rectWin;
		rectWin.x = (SCREEN_WIDTH - m_pWinGraphic->w)/2;
		rectWin.y = (SCREEN_HEIGHT - m_pWinGraphic->h)/2;
		rectWin.w = m_pWinGraphic->w;
		rectWin.h = m_pWinGraphic->h;
	
		SDL_BlitSurface(m_pWinGraphic, NULL, m_pScreen, &rectWin);	
	}
#else
	if( !m_YouWinMessage.Animate() )
#endif
        {
                SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
}

void Game::Move(Direction eDirection)
{
	if( m_bGameOver )
		return;

	m_Selector.Move(eDirection);	
}

void Game::ToggleMarking()
{
	if( m_bGameOver )
                return;

	MarkHitoriSpot(m_Hitori, m_Selector.GetCurrentX(), m_Selector.GetCurrentY());

	if( IsHitoriSolved(m_Hitori) == HITORILIB_SOLVED ) {
		m_bGameOver = true;
#ifndef USE_GRAPHIC_YOU_WIN
		m_YouWinMessage.CreateMessage("You Win!!!\n******");
#endif
		if( m_nLevelNumber >= 0 ) {
			m_pConfig->SetBeatLevel(m_nLevelNumber);
		}
	}
}

