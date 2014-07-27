#include "Config.h"
#include "Defines.h"

Config::Config()
: m_bSolutionHelper(true), m_bSelectorMovementAnimate(true)
{
   ArchiveCreate(&m_Archive);

   OpenArchiveFile(m_Archive, "HitoriSettings.tns");

   int nSettings = GetNumberArchives(m_Archive, "Settings");
   char strName[MAX_NAME_LENGTH];
   for(int i=0; i<nSettings; i++) {
      strcpy(strName, GetName(m_Archive, "Settings", i));
      if( strcmp(strName, "SolutionHelper") == 0 ) {
         if( strcmp("0", GetValue(m_Archive, "Settings", i)) == 0 ) {
            m_bSolutionHelper = false;
         } else {
            m_bSolutionHelper = true;
         }   
      }
      else if( strcmp(strName, "SelectorMovementAnimate") == 0 ) {
         if( strcmp("0", GetValue(m_Archive, "Settings", i)) == 0 ) {
            m_bSelectorMovementAnimate = false;
         } else {
            m_bSelectorMovementAnimate = true;
         }
      }
   }
}

Config::~Config()
{
   ArchiveSetBatchMode(m_Archive, ARCHIVE_ENABLE_BATCH);
   UpdateArchiveEntry(m_Archive, "Settings", "SolutionHelper", m_bSolutionHelper ? "1" : "0", NULL);
   UpdateArchiveEntry(m_Archive, "Settings", "SelectorMovementAnimate", m_bSelectorMovementAnimate ? "1" : "0", NULL);
   ArchiveSetBatchMode(m_Archive, ARCHIVE_DISABLE_BATCH);

   ArchiveFree(&m_Archive);
}

bool Config::GetSolutionHelper() const
{
   return m_bSolutionHelper;
}

void Config::SetSolutionHelper(bool bEnable)
{
   m_bSolutionHelper = bEnable;
}

bool Config::GetSelectorMovementAnimate() const
{
   return m_bSelectorMovementAnimate;
}

void Config::SetSelectorMovementAnimate(bool bEnable)
{
   m_bSelectorMovementAnimate = bEnable;
}

bool Config::BeatLevel(int nLevelNumber) const
{
   char buffer[8];
   Hit_itoa(nLevelNumber, buffer, 8);

   int nLevelsBeat = GetNumberArchives(m_Archive, "LevelsCompleted");
   char strName[MAX_NAME_LENGTH];
   for(int i=0; i<nLevelsBeat; i++) {
      strcpy(strName, GetName(m_Archive, "LevelsCompleted", i));
      if( strcmp(strName, buffer) == 0 ) {
         if( strcmp("1", GetValue(m_Archive, "LevelsCompleted", i)) == 0 ) {
            return true;
         } else {
            return false;
         }
      }
   }
   return false;
}

void Config::SetBeatLevel(int nLevelNumber)
{
   char buffer[8];
   Hit_itoa(nLevelNumber, buffer, 8);
   UpdateArchiveEntry(m_Archive, "LevelsCompleted", buffer, "1", NULL);
}

