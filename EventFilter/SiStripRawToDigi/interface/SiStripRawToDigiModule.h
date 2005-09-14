#ifndef SiStripRawToDigiModule_H
#define SiStripRawToDigiModule_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include <memory>
#include <string>

class SiStripRawToDigi;
class SiStripUtility;

/**
   \class SiStripRawToDigiModule 
   \brief A plug-in module that takes a FEDRawDataCollection as input
   and creates an EDProduct in the form of a StripDigiCollection.
   \author R.Bainbridge
   \version 0.1
   \date 05/09/05

   A plug-in module that performs RawToDigi conversion. 
   Input (from Event): FEDRawDataCollection. 
   Output (EDProduct): StripDigiCollection. 
   Nota bene: this is a PROTOTYPE IMPLEMENTATION!
*/
class SiStripRawToDigiModule : public edm::EDProducer {
  
public:
  
  /** Constructor. */
  explicit SiStripRawToDigiModule( const edm::ParameterSet& );
  /** Destructor. */
  ~SiStripRawToDigiModule();

  /** Some initialisation. Retrieves cabling map from
      EventSetup. Creates RawToDigi converter object. */
  virtual void beginJob( const edm::EventSetup& );
  /** Currently does nothing. */
  virtual void endJob();
  
  /** Retrieves a FEDRawDataCollection from the Event, creates a
      StripDigiCollection (EDProduct) using the RawToDigi converter,
      and attaches it to the Event. */
  virtual void produce( edm::Event&, const edm::EventSetup& );
  
 private: 
  
  /** RawToDigi converter that creates StripDigis based on the input
      of FEDRawData objects that contain FED buffers. */
  SiStripRawToDigi* rawToDigi_;
  /** Utility class providing dummy digis, FED buffers, cabling map. */
  SiStripUtility* utility_;

  /** Event counter. */
  unsigned long event_;

  /** Defines the FED readout mode (ZS, VR, PR or SM). */
  std::string fedReadoutMode_; // FedReadoutMode fedReadoutMode_;
  /** Defines the FED readout path (VME or SLINK). */
  std::string fedReadoutPath_;

  /** Defines verbosity level for this class (0=silent -> 3=debug). */
  int verbosity_;

  /** */
  std::string edProductLabel_;
  
};

#endif // SiStripRawToDigiModule_H

