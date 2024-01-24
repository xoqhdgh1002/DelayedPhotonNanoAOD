// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      EGMSeedGainProducer1
//
/**\class EGMSeedGainProducer1 EGMSeedGainProducer1.cc PhysicsTools/NanoAOD/plugins/EGMSeedGainProducer1.cc
 Description: [one line class summary]
 Implementation:
     [Notes on implementation]
*/

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "DataFormats/Common/interface/View.h"

//
// class declaration
//

template <typename T>
class EGMSeedGainProducer1 : public edm::global::EDProducer<> {
public:
  explicit EGMSeedGainProducer1(const edm::ParameterSet& iConfig)
      : src_(consumes<edm::View<T>>(iConfig.getParameter<edm::InputTag>("src"))) {
    produces<edm::ValueMap<float>>();
  }
  ~EGMSeedGainProducer1() override{};

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  // ----------member data ---------------------------

  edm::EDGetTokenT<edm::View<T>> src_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// member functions
//

// ------------ method called to produce the data  ------------
template <typename T>
void EGMSeedGainProducer1<T>::produce(edm::StreamID streamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  auto src = iEvent.getHandle(src_);

  unsigned nSrc = src->size();
  std::vector<float> gainSeed(nSrc, 12);

  // determine gain of seed crystal as in RecoEgamma/EgammaTools/src/PhotonEnergyCalibrator.cc
  for (unsigned i = 0; i < nSrc; i++) {
    auto obj = src->ptrAt(i);
    std::cout << i << " photon" << std::endl;
    std::cout << "Photon pt: " << obj->pt() << std::endl;
    gainSeed[i] = obj->pt();

  }

  std::unique_ptr<edm::ValueMap<float>> gainSeedV(new edm::ValueMap<float>());
  edm::ValueMap<float>::Filler fillerCorr(*gainSeedV);
  fillerCorr.insert(src, gainSeed.begin(), gainSeed.end());
  fillerCorr.fill();
  iEvent.put(std::move(gainSeedV));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
template <typename T>
void EGMSeedGainProducer1<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src")->setComment("input physics object collection");
  descriptions.addDefault(desc);
}

typedef EGMSeedGainProducer1<pat::Electron> ElectronSeedGainProducer1;
typedef EGMSeedGainProducer1<pat::Photon> PhotonSeedGainProducer1;

//define this as a plug-in
DEFINE_FWK_MODULE(ElectronSeedGainProducer1);
DEFINE_FWK_MODULE(PhotonSeedGainProducer1);

