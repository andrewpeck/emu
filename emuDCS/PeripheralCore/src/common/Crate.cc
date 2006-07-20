//-----------------------------------------------------------------------
// $Id: Crate.cc,v 3.0 2006/07/20 21:15:48 geurts Exp $
// $Log: Crate.cc,v $
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.16  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.15  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.14  2006/07/14 08:11:58  mey
// Got rid of Singleton
//
// Revision 2.13  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.12  2006/06/23 13:53:29  mey
// bug fix
//
// Revision 2.11  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 2.10  2006/06/15 16:38:41  rakness
// multiple slot firmware downloading
//
// Revision 2.9  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.8  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.7  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.6  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.5  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.4  2006/01/18 12:46:48  mey
// Update
//
// Revision 2.3  2006/01/12 22:36:34  mey
// UPdate
//
// Revision 2.2  2006/01/10 23:32:30  mey
// Update
//
// Revision 2.1  2005/12/20 23:39:37  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "Crate.h"
#include <unistd.h> // for sleep
#include "VMEModule.h"
#include "VMEController.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "DAQMB.h"
#include "TMB.h"
#include "MPC.h"
#include "CCB.h"
#include "ALCTController.h"
#include "ChamberUtilities.h"
#include "Chamber.h"

Crate::Crate(int number, VMEController * controller, EmuSystem * emuSystem) : 
  theNumber(number),  
  theModules(28),
  theController(controller) 
{
  for(int i=0;i<theModules.size();i++) theModules[i] = 0;
  theChambers.clear();
  //Singleton<CrateSetup>::instance()->addCrate(number, this);
  emuSystem->addCrate(number, this);
}


Crate::~Crate() {
  for(unsigned i = 0; i < theModules.size(); ++i) {
    delete theModules[i];
  }
  delete theController;
}


void Crate::addModule(VMEModule * module) {
  if(theModules[module->slot()] != 0 ) delete theModules[module->slot()];
  theModules[module->slot()] = module;
}

void Crate::AddChamber(Chamber * chamber) {
  theChambers.push_back(chamber);
}

std::vector<ChamberUtilities> Crate::chamberUtilsMatch() const {
  //
  std::vector<DAQMB *> dmbVector = daqmbs();
  std::vector<TMB *>   tmbVector = tmbs();
  std::vector<ChamberUtilities>   result;
  //
  for( int i=0; i< dmbVector.size(); i++) {
    for( int j=0; j< tmbVector.size(); j++) {
      //
      if ( (tmbVector[j]->slot()+1) == (dmbVector[i]->slot()) ) {
	ChamberUtilities chamber ;
	chamber.SetTMB(tmbVector[j]);
	chamber.SetDMB(dmbVector[i]);
	chamber.SetMPC(this->mpc());
	chamber.SetCCB(this->ccb());
	result.push_back(chamber);
      }
      //
    }
  }
  //
  return result;
  //
}
//
std::vector<Chamber*> Crate::chambers() const {
  //
  return theChambers;
  //
}
//
std::vector<Chamber> Crate::chambersMatch() const {
  //
  std::vector<DAQMB *> dmbVector = daqmbs();
  std::vector<TMB *>   tmbVector = tmbs();
  std::vector<Chamber>   result;
  //
  for( int i=0; i< dmbVector.size(); i++) {
      for( int j=0; j< tmbVector.size(); j++) {
	//
	if ( (tmbVector[j]->slot()+1) == (dmbVector[i]->slot()) ) {
	  Chamber chamber ;
	  chamber.SetTMB(tmbVector[j]);
	  chamber.SetDMB(dmbVector[i]);
	  chamber.SetMPC(this->mpc());
	  chamber.SetCCB(this->ccb());
	  result.push_back(chamber);
	}
	//
      }
  }
  //
  return result;
  //
}
//
std::vector<DAQMB *> Crate::daqmbs() const {
  std::vector<DAQMB *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DAQMB * daqmb = dynamic_cast<DAQMB *>(theModules[i]);
    if(daqmb != 0) result.push_back(daqmb);
  }
  return result;
}


std::vector<TMB *> Crate::tmbs() const {
  std::vector<TMB *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    TMB * tmb = dynamic_cast<TMB *>(theModules[i]);
    if(tmb != 0) result.push_back(tmb);
  }
  return result;
}


CCB * Crate::ccb() const {
  return findBoard<CCB>();
}


MPC * Crate::mpc() const {
  return findBoard<MPC>();
}


void Crate::enable() {
  //
  MPC * mpc = this->mpc();
  CCB * ccb = this->ccb();
  //
  if(mpc) mpc->init();
  ccb->enable();
}
//
void Crate::disable() {
  //
  CCB * ccb = this->ccb();
  ccb->disableL1();
  ccb->disable();
  //::sleep(1);
  std::cout << "data taking disabled " << std::endl;
  //
}
//
void Crate::configure() {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  //
  theController->init();
  //
  ccb->configure();
  //
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    myTmbs[i]->configure();
  }
  //
  //::sleep(1);
  //
  printf("############### Now setup \n");

  for(unsigned i =0; i < myTmbs.size(); ++i) {
    ALCTController * alct = myTmbs[i]->alctController();
    if(alct) {
      std::cout << "alct # =" << i << std::endl;
#ifndef ALCTNEW
      alct->setup(1);
#else
      alct->configure();
#endif
    }
  }
  //
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->restoreCFEBIdle();
    myDmbs[i]->restoreMotherboardIdle();
    myDmbs[i]->configure();
  }
  //  
  if(mpc) mpc->configure();
  //::sleep(1);
}
//
void Crate::init() {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  //
  ccb->init();
  //
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    myTmbs[i]->init();
  }
  //
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->init();
  }
  //  
  if(mpc) mpc->init();
  //
}
//
