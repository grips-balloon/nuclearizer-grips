/*
 * MGUIOptionsLoaderMeasurementsHDFGRIPS.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
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


// Include the header:
#include "MGUIOptionsLoaderMeasurementsHDFGRIPS.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <MString.h>
#include <TGLabel.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MModuleLoaderMeasurementsHDFGRIPS.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MGUIOptionsLoaderMeasurementsHDFGRIPS)
#endif


////////////////////////////////////////////////////////////////////////////////


MGUIOptionsLoaderMeasurementsHDFGRIPS::MGUIOptionsLoaderMeasurementsHDFGRIPS(MModule* Module)
  : MGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


MGUIOptionsLoaderMeasurementsHDFGRIPS::~MGUIOptionsLoaderMeasurementsHDFGRIPS()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void MGUIOptionsLoaderMeasurementsHDFGRIPS::Create()
{
  PreCreate();

  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 10, 10, 10, 10);

  m_FileSelectorHDF = new MGUIEFileSelector(m_OptionsFrame, "Please select a HDF5 file:",
    dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->GetFileName());
  m_FileSelectorHDF->SetFileType("HDF5 file", "*.hdf5");
  m_FileSelectorHDF->SetFileType("HDF5 file", "*.hdf");
  m_OptionsFrame->AddFrame(m_FileSelectorHDF, LabelLayout);


  m_LoadContinuationFiles = new TGCheckButton(m_OptionsFrame, "Enable loading continuation HDF5 files", 1);
  m_LoadContinuationFiles->SetOn(dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->GetLoadContinuationFiles());
  m_LoadContinuationFiles->Associate(this);
  m_OptionsFrame->AddFrame(m_LoadContinuationFiles, LabelLayout);


  m_FileSelectorStripMap = new MGUIEFileSelector(m_OptionsFrame, "Please select a strip map file:",
    dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->GetFileNameStripMap());
  m_FileSelectorStripMap->SetFileType("Strip map file", "*.map");
  m_OptionsFrame->AddFrame(m_FileSelectorStripMap, LabelLayout);
  
  // Nearest neighbor checkbox
  m_IncludeNearestNeighbor = new TGCheckButton(m_OptionsFrame, "Include Nearest Neighbors");
  m_IncludeNearestNeighbor->SetOn(dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->GetIncludeNearestNeighbor());
  m_IncludeNearestNeighbor->Associate(this);
  m_OptionsFrame->AddFrame(m_IncludeNearestNeighbor, LabelLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool MGUIOptionsLoaderMeasurementsHDFGRIPS::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Modify here if you have more buttons

  bool Status = true;

  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      break;
     default:
      break;
    }
    break;
  default:
    break;
  }
  
  if (Status == false) {
    return false;
  }

  // Call also base class
  return MGUIOptions::ProcessMessage(Message, Parameter1, Parameter2);
}


////////////////////////////////////////////////////////////////////////////////


bool MGUIOptionsLoaderMeasurementsHDFGRIPS::OnApply()
{
  // Modify this to store the data in the module!

  dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->SetFileName(m_FileSelectorHDF->GetFileName());
  dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->SetLoadContinuationFiles(m_LoadContinuationFiles->IsOn());
  dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->SetFileNameStripMap(m_FileSelectorStripMap->GetFileName());
  dynamic_cast<MModuleLoaderMeasurementsHDFGRIPS*>(m_Module)->SetIncludeNearestNeighbor(m_IncludeNearestNeighbor->IsOn());

  return true;
}


// MGUIOptionsLoaderMeasurementsHDFGRIPS: the end...
////////////////////////////////////////////////////////////////////////////////
