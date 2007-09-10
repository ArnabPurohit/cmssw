#include "RecoTauTag/RecoTau/interface/PFRecoTauTagInfoProducer.h"

PFRecoTauTagInfoProducer::PFRecoTauTagInfoProducer(const ParameterSet& iConfig){
  PFJetTracksAssociatorProducer_      = iConfig.getParameter<string>("PFJetTracksAssociatorProducer");
  PVProducer_                         = iConfig.getParameter<string>("PVProducer");
  smearedPVsigmaX_                    = iConfig.getParameter<double>("smearedPVsigmaX");
  smearedPVsigmaY_                    = iConfig.getParameter<double>("smearedPVsigmaY");
  smearedPVsigmaZ_                    = iConfig.getParameter<double>("smearedPVsigmaZ");	
  PFRecoTauTagInfoAlgo_=new PFRecoTauTagInfoAlgorithm(iConfig);
  produces<PFTauTagInfoCollection>();      
}
PFRecoTauTagInfoProducer::~PFRecoTauTagInfoProducer(){
  delete PFRecoTauTagInfoAlgo_;
}

void PFRecoTauTagInfoProducer::produce(Event& iEvent, const EventSetup& iSetup){
  Handle<JetTracksAssociationCollection> thePFJetTracksAssociatorCollection;
  iEvent.getByLabel(PFJetTracksAssociatorProducer_,thePFJetTracksAssociatorCollection);
  
  // query a rec/sim PV
  Handle<VertexCollection> thePVs;
  iEvent.getByLabel(PVProducer_,thePVs);
  const VertexCollection vertCollection=*(thePVs.product());
  Vertex thePV;
  if(vertCollection.size()) thePV=*(vertCollection.begin());
  else{
    Vertex::Error SimPVError;
    SimPVError(0,0)=smearedPVsigmaX_*smearedPVsigmaX_;
    SimPVError(1,1)=smearedPVsigmaY_*smearedPVsigmaY_;
    SimPVError(2,2)=smearedPVsigmaZ_*smearedPVsigmaZ_;
    Vertex::Point SimPVPoint(RandGauss::shoot(0.,smearedPVsigmaX_),  
			     RandGauss::shoot(0.,smearedPVsigmaY_),  
			     RandGauss::shoot(0.,smearedPVsigmaZ_));
    thePV=Vertex(SimPVPoint,SimPVError,1,1,1);    
  }
  
  PFTauTagInfoCollection* extCollection=new PFTauTagInfoCollection();

  unsigned int i_Assoc=0;
  for(JetTracksAssociationCollection::const_iterator iAssoc=thePFJetTracksAssociatorCollection->begin();iAssoc!=thePFJetTracksAssociatorCollection->end();iAssoc++){
    PFTauTagInfo myPFTauTagInfo=PFRecoTauTagInfoAlgo_->buildPFTauTagInfo((*iAssoc).first.castTo<PFJetRef>(),(*iAssoc).second,thePV);
    extCollection->push_back(myPFTauTagInfo);
    ++i_Assoc;
  }
  
  auto_ptr<PFTauTagInfoCollection> resultExt(extCollection);  
  OrphanHandle<PFTauTagInfoCollection> myPFTauTagInfoCollection=iEvent.put(resultExt);
}
