#include "Selector.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

#include "Defines.h"

#define SELECTOR_LINE_WIDTH     (4)
#define ANIMATION_STEPS		(4)

Selector::Selector(SDL_Surface* pScreen, Config* pConfig)
: m_pScreen(pScreen), m_pConfig(pConfig)
#ifdef ANIMATE_SELECTOR_MOVEMENT
, m_nPreviousX(0), m_nPreviousY(0), m_nAnimateStep(0)
#endif
, m_nCurrentX(0), m_nCurrentY(0)
{
}

void Selector::SetDimensions(int nWidth, int nHeight)
{
   m_nWidth = nWidth;
   m_nHeight = nHeight;
}

int Difference(int a, int b)
{
   return Hit_Max(a-b, b-a);
}

void Selector::DrawSelector(int nLeft, int nTop, int nPieceSize)
{
   int r=255, g=0, b=0, a=200;
#ifdef SELECTOR_COLOR_ANIMATES
   m_ColorIntensity.AdjustColor(r,g,b,a);
   //printf("r:%d, g:%d, b:%d, a:%d\n", r, g, b, a);
#endif

#ifdef ANIMATE_SELECTOR_MOVEMENT
   if( m_nPreviousX != m_nCurrentX || m_nPreviousY != m_nCurrentY ) {
      m_nAnimateStep++;
      if( m_nAnimateStep >= ANIMATION_STEPS ) {
         m_nPreviousX = m_nCurrentX;
         m_nPreviousY = m_nCurrentY;
         m_nAnimateStep = 0;
      }
   }
#endif

   int nX = 
#ifdef ANIMATE_SELECTOR_MOVEMENT
	m_pConfig->GetSelectorMovementAnimate() ? m_nPreviousX : m_nCurrentX;
#else
	m_nCurrentX;
#endif
   int nY = 
#ifdef ANIMATE_SELECTOR_MOVEMENT
	m_pConfig->GetSelectorMovementAnimate() ? m_nPreviousY : m_nCurrentY;
#else
	m_nCurrentY;
#endif

   int nExtraX =
#ifdef ANIMATE_SELECTOR_MOVEMENT
	m_pConfig->GetSelectorMovementAnimate() ? m_nPreviousX != m_nCurrentX ? Difference(m_nPreviousX, m_nCurrentX)*nPieceSize*m_nAnimateStep/ANIMATION_STEPS : 0 : 0;
   if( m_nPreviousX > m_nCurrentX ) nExtraX *= -1;
#else
	0;
#endif
	int nExtraY =
#ifdef ANIMATE_SELECTOR_MOVEMENT
        m_pConfig->GetSelectorMovementAnimate() ? m_nPreviousY != m_nCurrentY ? Difference(m_nPreviousY, m_nCurrentY)*nPieceSize*m_nAnimateStep/ANIMATION_STEPS : 0 : 0;
   if( m_nPreviousY > m_nCurrentY ) nExtraY *= -1;
#else
        0;
#endif


   //Top
   thickLineRGBA(m_pScreen, nLeft + nX*nPieceSize + nExtraX, nTop + nY*nPieceSize + nExtraY, nLeft + nX*nPieceSize + nPieceSize + nExtraX, nTop + nY*nPieceSize + nExtraY, SELECTOR_LINE_WIDTH, r, g, b, a);
   //Left
   thickLineRGBA(m_pScreen, nLeft + nX*nPieceSize + nExtraX, nTop + nY*nPieceSize + nExtraY, nLeft + nX*nPieceSize + nExtraX, nTop + nY*nPieceSize + nPieceSize + nExtraY, SELECTOR_LINE_WIDTH, r, g, b, a);
   //Bottom
   thickLineRGBA(m_pScreen, nLeft + nX*nPieceSize + nExtraX, nTop + nY*nPieceSize + nPieceSize + nExtraY, nLeft + nX*nPieceSize + nPieceSize + nExtraX, nTop + nY*nPieceSize + nPieceSize + nExtraY, SELECTOR_LINE_WIDTH, r, g, b, a);
   //Right
   thickLineRGBA(m_pScreen, nLeft + nX*nPieceSize + nPieceSize + nExtraX, nTop + nY*nPieceSize + nExtraY, nLeft + nX*nPieceSize + nPieceSize + nExtraX, nTop + nY*nPieceSize + nPieceSize + nExtraY, SELECTOR_LINE_WIDTH, r, g, b, a);
   //roundedRectangleRGBA(m_pScreen, nLeft + nX*nPieceSize, nTop + nY*nPieceSize, nLeft + nX*nPieceSize + nPieceSize, nTop + nY*nPieceSize + nPieceSize, 8, 0, 0, 255, 127);
}

void Selector::Move(Direction eDirection)
{
#ifdef ANIMATE_SELECTOR_MOVEMENT
   m_nPreviousX = m_nCurrentX;
   m_nPreviousY = m_nCurrentY;
   m_nAnimateStep = 0;
#endif

   if( eDirection == Up ) {
      if( m_nCurrentY > 0 ) m_nCurrentY--;
#ifdef MOVEMENT_WRAPS
      else if( m_nCurrentY == 0 ) m_nCurrentY = m_nHeight-1;
#endif
   }
   else if( eDirection == Left ) {
      if( m_nCurrentX > 0 ) m_nCurrentX--;
#ifdef MOVEMENT_WRAPS
      else if( m_nCurrentX == 0 ) m_nCurrentX = m_nWidth-1;
#endif
   }
   else if( eDirection == Right ) {
      if( m_nCurrentX < (m_nWidth-1) ) m_nCurrentX++;
#ifdef MOVEMENT_WRAPS
      else if( m_nCurrentX == (m_nWidth-1) ) m_nCurrentX = 0;
#endif
   }
   else if( eDirection == Down ) {
      if( m_nCurrentY < (m_nHeight-1) ) m_nCurrentY++;
#ifdef MOVEMENT_WRAPS
      else if( m_nCurrentY == (m_nHeight-1) ) m_nCurrentY = 0;
#endif
   }
}

int Selector::GetCurrentX() const
{
   return m_nCurrentX;
}

int Selector::GetCurrentY() const
{
   return m_nCurrentY;
}
