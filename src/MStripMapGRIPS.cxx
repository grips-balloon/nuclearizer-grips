/*
 * MStripMapGRIPS.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer, Felix Hagemann.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MStripMapGRIPS
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MStripMapGRIPS.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MParser.h"
#include "MExceptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MStripMapGRIPS)
#endif


////////////////////////////////////////////////////////////////////////////////


MStripMapGRIPS::MStripMapGRIPS()
{
  // Construct an instance of MStripMapGRIPS
}


////////////////////////////////////////////////////////////////////////////////


MStripMapGRIPS::~MStripMapGRIPS()
{
  // Delete this instance of MStripMapGRIPS
}


////////////////////////////////////////////////////////////////////////////////


//! Load a strip map
bool MStripMapGRIPS::Open(MString FileName)
{
  m_StripMappings.clear();

  MParser Parser;
  if (Parser.Open(FileName) == false) {
    if (g_Verbosity >= c_Error) cout<<"MStripMapGRIPS: Unable to load file: "<<endl<<FileName<<error;
    return false;
  }

  for (unsigned int i = 0; i < Parser.GetNLines(); ++i) {
    if (Parser.GetTokenizerAt(i)->GetNTokens() == 0) continue;
    if (Parser.GetTokenizerAt(i)->GetTokenAtAsString(0).BeginsWith("#") == true) continue;
    // Strip map format 1 (with 9 columns)
    if (Parser.GetTokenizerAt(i)->GetNTokens() == 9) {
      MSingleStripMapping SM;
      SM.m_ReadOutID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(0);
      SM.m_RTB = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(1);
      SM.m_DRM = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(2);
      SM.m_ASICBoard = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(3);
      SM.m_ASICID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(4);
      SM.m_ChannelID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(5);
      SM.m_DetectorID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(6);
      SM.m_IsLowVoltage = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(7) == 0 ? true : false;
      SM.m_StripNumber = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(8);
      m_StripMappings.push_back(SM);
    }
    // Strip map format 2 (with 4 columns)
    if (Parser.GetTokenizerAt(i)->GetNTokens() == 4) {
      MSingleStripMapping SM;
      SM.m_ReadOutID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(0);
      SM.m_DetectorID = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(1);
      SM.m_IsLowVoltage = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(2) == 0 ? true : false;
      SM.m_StripNumber = Parser.GetTokenizerAt(i)->GetTokenAtAsUnsignedInt(3);
      
      // Infer the rest of the information from the ReadOutID
      SM.m_RTB = (SM.m_ReadOutID >> 8) & 0x01;
      SM.m_DRM = (SM.m_ReadOutID >> 7) & 0x01;
      SM.m_ASICBoard = (SM.m_ReadOutID >> 6) & 0x03;
      SM.m_ASICID = (SM.m_ReadOutID >> 5) & 0x01;
      SM.m_ChannelID = SM.m_ReadOutID & 0x1F;

      m_StripMappings.push_back(SM);
    }
  }

  // Sort by m_ReadOutID:
  sort(m_StripMappings.begin(), m_StripMappings.end(), [](const MSingleStripMapping& A, const MSingleStripMapping& B) { return A.m_ReadOutID < B.m_ReadOutID; });

  return true;
}


////////////////////////////////////////////////////////////////////////////////

bool MStripMapGRIPS::UpdateASICPolarities(vector<map<unsigned int, vector<bool>>> ASICPolarities) {
  if (!m_StripMappings.empty()) {
    for (MSingleStripMapping& S : m_StripMappings) {
      if (S.m_DetectorID > ASICPolarities.size()-1) {
        cout << "ASICPolarities len" << ASICPolarities.size() << ", but trying to index for Detector " << S.m_DetectorID << ". Assuming no more detectors" << endl;
        return true;
      }
      if (S.m_ASICBoard > ASICPolarities[S.m_DetectorID].size()-1) {
        cout << "ASICPolarities[" << S.m_DetectorID << "] len" << ASICPolarities[S.m_DetectorID].size() << ", but trying to index for Board " << S.m_ASICBoard << endl;
        return false;
      }
      S.m_IsLowVoltage = ASICPolarities[S.m_DetectorID][S.m_ASICBoard][S.m_ASICID];
    }
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


//! Return true if the given read-out ID is on file
bool MStripMapGRIPS::HasReadOutID(unsigned int ROI) const
{
  auto Iter = lower_bound(m_StripMappings.begin(), m_StripMappings.end(), ROI, [](const MSingleStripMapping& SSM, unsigned int ID) { return SSM.m_ReadOutID < ID; });
  return Iter != m_StripMappings.end() && Iter->m_ReadOutID == ROI;
}


////////////////////////////////////////////////////////////////////////////////


//! Return the index of the ROI, throw an exception otherwise
unsigned int MStripMapGRIPS::GetReadOutIDIndex(unsigned int ROI) const
{
  auto Iter = lower_bound(m_StripMappings.begin(), m_StripMappings.end(), ROI, [](const MSingleStripMapping& SSM, unsigned int ID) { return SSM.m_ReadOutID < ID; });

  if (Iter != m_StripMappings.end() && Iter->m_ReadOutID == ROI) {
    return distance(m_StripMappings.begin(), Iter);
  } else {
    throw MExceptionValueNotFound(ROI, "vector of read-out IDs");
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Get detector by read-out ID
unsigned int MStripMapGRIPS::GetDetectorID(unsigned int ROI) const
{
  return m_StripMappings[GetReadOutIDIndex(ROI)].m_DetectorID;
}


////////////////////////////////////////////////////////////////////////////////


//! Get detector side by read-out ID
bool MStripMapGRIPS::IsLowVoltage(unsigned int ROI) const
{
  return m_StripMappings[GetReadOutIDIndex(ROI)].m_IsLowVoltage;
}


////////////////////////////////////////////////////////////////////////////////


//! Get strip ID by read-out ID
unsigned int MStripMapGRIPS::GetStripNumber(unsigned int ROI) const
{
  return m_StripMappings[GetReadOutIDIndex(ROI)].m_StripNumber;
}


// MStripMapGRIPS.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
