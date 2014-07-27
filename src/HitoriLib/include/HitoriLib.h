#ifndef HITORILIB_H_INCLUDED
#define HITORILIB_H_INCLUDED

typedef void* HitoriLib;

#define HITORILIB_OK			(0)
#define HITORILIB_BADARGUMENT		(-1)
#define HITORILIB_OUT_OF_MEMORY		(-2)

#define HITORILIB_SOLVED		(0)
#define HITORILIB_SOLVE_SAME_COLUMN	(-1)
#define HITORILIB_SOLVE_SAME_ROW	(-2)
#define HITORILIB_SOLVE_ADJACENT	(-3)

#define HITORILIB_NOT_ISOLATED		(0)
#define HITORILIB_ISOLATED		(1)

#define HITORILIB_CANNOT_UNDO		(1)
#define HITORILIB_CANNOT_REDO		(1)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int HitoriLibCreate(HitoriLib* api, const char* pstrFile );
int HitoriLibFree(HitoriLib* api);

int GetHitoriLibError(HitoriLib api);
void ClearHitoriLibError(HitoriLib api);

//////////////////////////////////////////////
//HitoriLib related functions
//////////////////////////////////////////////
int GetHitoriBoardWidth(HitoriLib api);
int GetHitoriBoardHeight(HitoriLib api);
int GetHitoriSpotValue(HitoriLib api, int nX, int nY);
int MarkHitoriSpot(HitoriLib api, int nX, int nY);
int IsHitoriSpotMarked(HitoriLib api, int nX, int nY);
int IsHitoriSolved(HitoriLib api);
int IsHitoriIsolatedSpot(HitoriLib api, int nX, int nY);
int SetHitoriToSolved(HitoriLib api);
int HitoriUndo(HitoriLib api);
int HitoriRedo(HitoriLib api);

#endif //HITORILIB_H_INCLUDED
