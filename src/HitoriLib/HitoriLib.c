//Public domain :)

#include <os.h>
#include "include/HitoriLib.h"
#include "Defines.h"

struct HitoriItem
{
   int m_nValue;
   int m_nMarked;
};

struct HitoriBoard
{
   int m_nWidth;
   int m_nHeight;
   struct HitoriItem* m_pItems;
};

struct HitoriAction
{
   int m_nX;
   int m_nY;
   int m_nMarked;
   struct HitoriAction* m_pNext;
};

struct HitoriItem* GetAt(struct HitoriBoard* pBoard, int nX, int nY)
{
   if( nX < 0 || nY < 0 || (nX >= pBoard->m_nWidth) || (nY >= pBoard->m_nHeight) ) {
      printf("Accessing non-existant element %d,%d\n", nX, nY);
      return NULL;
   }

   return pBoard->m_pItems + (pBoard->m_nWidth * nY + nX);
}

void* AllocateBoard(int nWidth, int nHeight)
{
   return malloc(nWidth*nHeight*sizeof(struct HitoriItem));
}

struct AdjacentItem
{
   int m_nX;
   int m_nY;
};

struct HitoriSolution
{
   int m_nX;
   int m_nY;
   struct HitoriSolution* m_pNext;
};

struct Hitori
{
   struct HitoriBoard* m_pBoard;
   struct AdjacentItem* m_pAdjacentSpots;
   struct HitoriAction* m_pUndoActions;
   struct HitoriAction* m_pRedoActions;
   struct HitoriSolution* m_pSolution;
   int m_nNumberMarked;
   int m_nLastError;
};

void AddSolutionItem(struct Hitori* pH, int nX, int nY)
{
   if( nX < 0 || nY < 0 ) return;
   struct HitoriSolution* pSol = malloc(sizeof(struct HitoriSolution));
   pSol->m_nX = nX;
   pSol->m_nY = nY;
   pSol->m_pNext = pH->m_pSolution;
   pH->m_pSolution = pSol;
}

void ClearSolution(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriSolution* pCurrent = pH->m_pSolution;
   while(pCurrent != NULL) {
      struct HitoriSolution* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pH->m_pSolution = pCurrent;
   }
}

int AddAdjacentSpots(HitoriLib api, int nX, int nY) 
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   int nSpotsAdded = 0;

   if( IsHitoriSpotMarked(api, nX, nY) )
      return nSpotsAdded;

   //See if this spot is already in the list and if so return
   int n=0;
   while(1) {
      struct AdjacentItem* pItem = pH->m_pAdjacentSpots + n;
      if( pItem->m_nX < 0 )
         break;

      n++;

      if( pItem->m_nX == nX && pItem->m_nY == nY )
         return nSpotsAdded;
   }

   struct AdjacentItem* pThisSpot = pH->m_pAdjacentSpots + n;
   pThisSpot->m_nX = nX;
   pThisSpot->m_nY = nY;
   nSpotsAdded++;

   struct AdjacentItem* pLast = pH->m_pAdjacentSpots + n + nSpotsAdded;
   pLast->m_nX = -1;
   pLast->m_nY = -1;

   //Add adjacent spots
   if( nX != 0 ) nSpotsAdded += AddAdjacentSpots(api, nX-1, nY);
   if( nY != 0 ) nSpotsAdded += AddAdjacentSpots(api, nX, nY-1);
   if( nX != (GetHitoriBoardWidth(api)-1) ) nSpotsAdded += AddAdjacentSpots(api, nX+1, nY);
   if( nY != (GetHitoriBoardHeight(api)-1) ) nSpotsAdded += AddAdjacentSpots(api, nX, nY+1);
   
   return nSpotsAdded;
}

void UpdateAdjacentSpots(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct AdjacentItem* pItemStart = pH->m_pAdjacentSpots;
   pItemStart->m_nX = -1;
   pItemStart->m_nY = -1;

   int nX;
   for(nX=0; nX<GetHitoriBoardWidth(api); nX++) {
      int nY;
      for(nY=0; nY<GetHitoriBoardHeight(api); nY++) {
         if( !IsHitoriSpotMarked(api, nX, nY) ) {
            int nSpotsAdded = AddAdjacentSpots(api, nX, nY);

            return;
         }
      }
   }
}

int HitoriLibCreate(HitoriLib* api, const char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = malloc(sizeof(struct Hitori));
   if( pH == NULL ){//Out of memory
      return HITORILIB_OUT_OF_MEMORY;
   }

   pH->m_pBoard = NULL;
   pH->m_pAdjacentSpots = NULL;

   pH->m_pBoard = malloc(sizeof(struct HitoriBoard));
   if( pH->m_pBoard == NULL ){//Out of memory
      free(pH);
      pH = NULL;
      return HITORILIB_OUT_OF_MEMORY;
   }
   pH->m_pBoard->m_pItems = NULL;

   if( strstr(pstrFile, "Hitori ") != pstrFile ) {//Hitori file version check
      free(pH->m_pBoard);
      pH->m_pBoard = NULL;
      free(pH);
      pH = NULL;
      return HITORILIB_OUT_OF_MEMORY;//TODO: Better return error code
   }

   if( strstr(pstrFile, "1 ") != (pstrFile + strlen("Hitori ")) ) {//Version check
      free(pH->m_pBoard);
      pH->m_pBoard = NULL;
      free(pH);
      pH = NULL;
      return HITORILIB_OUT_OF_MEMORY;//TODO: Better return error code
   }

   char* pstr = pstrFile + strlen("Hitori 1 ");
   char buffer[16];
   int nSpotInBuffer = 0;

   int nWidth = -1, nHeight = -1;
   int nX = 0, nY = 0;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( !isspace(ch) )
            break;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         int nValue = atoi(buffer);

         if( nWidth < 0 ) {
            nWidth = nValue;
            pH->m_pBoard->m_nWidth = nWidth;
         }
         else if( nHeight < 0 ) {
            nHeight = nValue;
            pH->m_pBoard->m_nHeight = nHeight;
            pH->m_pBoard->m_pItems = malloc(nWidth*nHeight*sizeof(struct HitoriItem));
            if( pH->m_pBoard->m_pItems == NULL ) {//Out of memory
               free(pH->m_pBoard);
               pH->m_pBoard = NULL;
               free(pH);
               pH = NULL;
               return HITORILIB_OUT_OF_MEMORY;
            }

            pH->m_pAdjacentSpots = malloc((nWidth*nHeight+1)*sizeof(struct AdjacentItem));
            if( pH->m_pAdjacentSpots == NULL ) {//Out of memory
               free(pH->m_pBoard->m_pItems);
               pH->m_pBoard->m_pItems = NULL;
               free(pH->m_pBoard);
               pH->m_pBoard = NULL;
               free(pH);
               pH = NULL;
               return HITORILIB_OUT_OF_MEMORY;
            }

         }
         else {
            struct HitoriItem* pItem = GetAt(pH->m_pBoard, nX, nY);
            pItem->m_nValue = nValue;
            pItem->m_nMarked = 0;
            nX++;
            if( nX >= nWidth ) {
               nX = 0;
               nY++;
            }

            //Maybe check if has more numbers than it should?
         }
      }
   }

   pH->m_pSolution = NULL;
   pstr = strstr(pstrFile, "Solution ");
   if( pstr == NULL ) {
      //Should never happen!
   }
   pstr = pstr + strlen("Solution ");

   nSpotInBuffer = 0;
   int nSolX = -1, nSolY = -1;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( !isspace(ch) || ch == '\r' || ch == '\n' || ch == '\0' )
            break;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         int nValue = atoi(buffer);

         if( nSolX < 0 ) {
            nSolX = nValue;
         } else {
            nSolY = nValue;
            AddSolutionItem(pH, nSolX, nSolY);
            nSolX = -1;
            nSolY = -1;
         }
      }
   }
   buffer[nSpotInBuffer] = '\0';
   int nValue = atoi(buffer);
   nSolY = nValue;
   AddSolutionItem(pH, nSolX, nSolY);

   pH->m_pUndoActions = NULL;
   pH->m_pRedoActions = NULL;
   pH->m_nNumberMarked = 0;//Note if opened level with marking; need to count for that!

   pH->m_nLastError = HITORILIB_OK;

   *api = pH;

   UpdateAdjacentSpots(*api);

   return HITORILIB_OK;
}

void ClearUndos(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pCurrent = pH->m_pUndoActions;
   while(pCurrent != NULL) {
      struct HitoriAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pH->m_pUndoActions = pCurrent;
   }

}

void ClearRedos(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pCurrent = pH->m_pRedoActions;
   while(pCurrent != NULL) {
      struct HitoriAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pH->m_pRedoActions = pCurrent;
   }

}

void AddUndo(HitoriLib api, int nX, int nY, int nMarked)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pAction = malloc(sizeof(struct HitoriAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddUndo\n");
   //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;
   pAction->m_nMarked = nMarked;

   struct HitoriAction* pRoot = pH->m_pUndoActions;
   pAction->m_pNext = pRoot;
   pH->m_pUndoActions = pAction;
}

void AddRedo(HitoriLib api, int nX, int nY, int nMarked)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pAction = malloc(sizeof(struct HitoriAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddRedo\n");
      //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;
   pAction->m_nMarked = nMarked;

   struct HitoriAction* pRoot = pH->m_pRedoActions;
   pAction->m_pNext = pRoot;
   pH->m_pRedoActions = pAction;
}

int HitoriLibFree(HitoriLib* api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = *api;

   ClearUndos(*api);
   ClearRedos(*api);
   ClearSolution(*api);

   free(pH->m_pAdjacentSpots);
   free(pH->m_pBoard->m_pItems);
   pH->m_pBoard->m_pItems = NULL;

   free(pH->m_pBoard);
   pH->m_pBoard = NULL;
   free(pH);
   pH = NULL;

   *api = NULL;
   return HITORILIB_OK;
}

int GetHitoriError(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;
   return pH->m_nLastError;
}

void ClearHitoriError(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;
   pH->m_nLastError = HITORILIB_OK;
}

//HitoriLib related functions
int GetHitoriBoardWidth(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   return pH->m_pBoard->m_nWidth;
}

int GetHitoriBoardHeight(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   return pH->m_pBoard->m_nHeight;
}

int GetHitoriSpotValue(HitoriLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   return GetAt(pH->m_pBoard, nX, nY)->m_nValue;
}

int MarkHitoriSpot(HitoriLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   int nMarked = GetAt(pH->m_pBoard, nX, nY)->m_nMarked;

   ClearRedos(api);

   AddUndo(api, nX, nY, nMarked);

   GetAt(pH->m_pBoard, nX, nY)->m_nMarked = !nMarked;
   if( nMarked ) {
      pH->m_nNumberMarked--;
   } else {
      pH->m_nNumberMarked++;
   }

   UpdateAdjacentSpots(api);

   return HITORILIB_OK;
}

int IsHitoriSpotMarked(HitoriLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   return GetAt(pH->m_pBoard, nX, nY)->m_nMarked;
}

int IsHitoriSolved(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   int nX = 0, nY = 0;

   //See if any isolated spots
   int nTotalSquares = pH->m_pBoard->m_nWidth * pH->m_pBoard->m_nHeight;
   int nAdjacent=0;
   while(1) {
      struct AdjacentItem* pItem = pH->m_pAdjacentSpots + nAdjacent;
      if( pItem->m_nX < 0 )
         break;
      nAdjacent++;
   }
   if( nTotalSquares != (nAdjacent + pH->m_nNumberMarked) )
      return HITORILIB_SOLVE_ADJACENT;

   //See if same number exists on same column
   for(nX = 0; nX < pH->m_pBoard->m_nWidth; nX++) {
      for(nY = 0; nY < pH->m_pBoard->m_nHeight; nY++) {
         if( !IsHitoriSpotMarked(api, nX, nY) ) {
            int nValue =  GetHitoriSpotValue(api, nX, nY);
            int nY2;
            for(nY2=0; nY2 < pH->m_pBoard->m_nHeight; nY2++) {
               if( nY != nY2 && !IsHitoriSpotMarked(api, nX, nY2) && GetHitoriSpotValue(api, nX, nY2) == nValue ) {
                  return HITORILIB_SOLVE_SAME_COLUMN;
               }
            }
         }
      }
   }

  //See if same number exists on same row
   for(nX = 0; nX < pH->m_pBoard->m_nWidth; nX++) {
      for(nY = 0; nY < pH->m_pBoard->m_nHeight; nY++) {
         if( !IsHitoriSpotMarked(api, nX, nY) ) {
            int nValue =  GetHitoriSpotValue(api, nX, nY);
            int nX2;
            for(nX2=0; nX2 < pH->m_pBoard->m_nWidth; nX2++) {
               if( nX != nX2 && !IsHitoriSpotMarked(api, nX2, nY) && GetHitoriSpotValue(api, nX2, nY) == nValue ) {
                  return HITORILIB_SOLVE_SAME_ROW;
               }
            }
         }
      }
   }

   //Filled in squares cannot be horizontally or vertically adjacent
   for(nX = 0; nX < pH->m_pBoard->m_nWidth; nX++) {
      for(nY = 0; nY < pH->m_pBoard->m_nHeight; nY++) {
         if( IsHitoriSpotMarked(api, nX, nY) ) {
            if( nX != 0 && IsHitoriSpotMarked(api, nX-1, nY) ) {
               return HITORILIB_SOLVE_ADJACENT;
            }
            if( nX != (GetHitoriBoardWidth(api)-1) && IsHitoriSpotMarked(api, nX+1, nY) ) {
               return HITORILIB_SOLVE_ADJACENT;
            }
            if( nY != 0 && IsHitoriSpotMarked(api, nX, nY-1) ) {
               return HITORILIB_SOLVE_ADJACENT;
            }
            if( nY != (GetHitoriBoardHeight(api)-1) && IsHitoriSpotMarked(api, nX, nY+1) ) {
               return HITORILIB_SOLVE_ADJACENT;
            }
         }
      }
   }

   return HITORILIB_SOLVED;
}

int IsHitoriIsolatedSpot(HitoriLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   int nAdjacent=0;
   while(1) {
      struct AdjacentItem* pItem = pH->m_pAdjacentSpots + nAdjacent;
      if( pItem->m_nX < 0 )
         break;
      if( pItem->m_nX == nX && pItem->m_nY == nY )
         return HITORILIB_NOT_ISOLATED;

      nAdjacent++;
   }
   return HITORILIB_ISOLATED;
}

int SetHitoriToSolved(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   int nX, nY;
   for(nX=0; nX<GetHitoriBoardWidth(api); nX++) {
      for(nY=0; nY<GetHitoriBoardHeight(api); nY++) {
         GetAt(pH->m_pBoard, nX, nY)->m_nMarked = 0;
      }
   }

   pH->m_nNumberMarked = 0;
   struct HitoriSolution* pCurrent = pH->m_pSolution;
   while(pCurrent != NULL) {
      GetAt(pH->m_pBoard, pCurrent->m_nX, pCurrent->m_nY)->m_nMarked = 1;
      pH->m_nNumberMarked++;
      pCurrent = pCurrent->m_pNext;
   }
   DEBUG_MSG("Number marked is: %d\n", pH->m_nNumberMarked);
   UpdateAdjacentSpots(api);

   return HITORILIB_OK;
}

int HitoriUndo(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pRoot = pH->m_pUndoActions;
   if( pRoot == NULL )
      return HITORILIB_CANNOT_UNDO;

   pH->m_pUndoActions = pRoot->m_pNext;
   AddRedo(api, pRoot->m_nX, pRoot->m_nY, !pRoot->m_nMarked);

   GetAt(pH->m_pBoard, pRoot->m_nX, pRoot->m_nY)->m_nMarked = pRoot->m_nMarked;
   if( pRoot->m_nMarked ) {
      pH->m_nNumberMarked++;
   } else {
      pH->m_nNumberMarked--;
   }

   free(pRoot);
   pRoot = NULL;

   UpdateAdjacentSpots(api);

   return HITORILIB_OK;
}

int HitoriRedo(HitoriLib api)
{
   DEBUG_FUNC_NAME;

   struct Hitori* pH = (struct Hitori*)api;

   struct HitoriAction* pRoot = pH->m_pRedoActions;
   if( pRoot == NULL )
      return HITORILIB_CANNOT_REDO;

   pH->m_pRedoActions = pRoot->m_pNext;
   AddUndo(api, pRoot->m_nX, pRoot->m_nY, !pRoot->m_nMarked);

   GetAt(pH->m_pBoard, pRoot->m_nX, pRoot->m_nY)->m_nMarked = pRoot->m_nMarked;
   if( pRoot->m_nMarked ) {
      pH->m_nNumberMarked++;
   } else {
      pH->m_nNumberMarked--;
   }

   free(pRoot);
   pRoot = NULL;

   UpdateAdjacentSpots(api);

   return HITORILIB_OK;
}



