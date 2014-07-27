#ifndef CONFIG_H
#define CONFIG_H

extern "C"
{
	#include <os.h>
	#include "ArchiveLib/ArchiveLib.h"
}

class Config
{
public:
   Config();
   ~Config();

   bool GetSolutionHelper() const;
   void SetSolutionHelper(bool bEnable);

   bool GetSelectorMovementAnimate() const;
   void SetSelectorMovementAnimate(bool bEnable);

   bool BeatLevel(int nLevelNumber) const;
   void SetBeatLevel(int nLevelNumber);

protected:
   ArchiveLib	m_Archive;
   bool		m_bSolutionHelper;
   bool		m_bSelectorMovementAnimate;
};

#endif

