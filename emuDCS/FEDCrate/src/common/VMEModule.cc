//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 1.2 2006/01/21 19:55:02 gilmore Exp $
// $Log: VMEModule.cc,v $
// Revision 1.2  2006/01/21 19:55:02  gilmore
// *** empty log message ***
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

extern unsigned long vmeadd;
extern int delay_type;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(int newcrate, int newslot): 
  theSlot(newslot)
{
  theCrate = Singleton<CrateSetup>::instance()->crate(newcrate);
  #ifdef debugV
    cout << "creating VMEModule in crate " << theCrate->number() << endl;
  #endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif  
   vmeadd=0x00000000|(theSlot<<19);
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}

 
int VMEModule::CAEN_read(unsigned long Address,unsigned short int *data){
  theController->start(this);
  return theController->CAEN_read(Address,data);
}

int VMEModule::CAEN_write(unsigned long Address,unsigned short int *data){
  theController->start(this);
  return theController->CAEN_write(Address,data);
}

void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  theController->start(this);
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan(reg, snd, cnt, rcv, ird);
}


void VMEModule::scan_reset(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan_reset(reg, snd, cnt, rcv, ird);
}

void VMEModule::InitJTAG(int port) {
  theController->start(this);
  theController->InitJTAG(port);
}


void VMEModule::CloseJTAG() {
  theController->start(this);
  theController->CloseJTAG();
  theController->end();
}

void VMEModule::flush_vme()
{
  theController->start(this);
  theController->flush_vme();
}

void VMEModule::handshake_vme()
{
  theController->start(this);
  theController->handshake_vme();
}

void VMEModule::sleep_vme(const char *outbuf)
{
  theController->start(this);
  theController->sleep_vme(outbuf);
}
void VMEModule::sleep_vme2(unsigned short int time)
{
  theController->start(this);
  theController->sleep_vme2(time);
}

void VMEModule::long_sleep_vme2(float time)
{
  theController->start(this);
  theController->long_sleep_vme2(time);
}

void VMEModule::send_last()
{
  theController->start(this);
  theController->send_last();
}


void VMEModule::vmepara(const char *cmd,const char *snd,char *rcv)
{
  theController->start(this);
  theController->vmepara(cmd,snd,rcv);
}

void VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{
  theController->start(this);
  theController->vmeser(cmd,snd,rcv);
}

void VMEModule::dcc(const char *cmd,char *rcv)
{
  theController->start(this);
  theController->dcc(cmd,rcv);
}

void VMEModule::vme_adc(int ichp,int ichn,char *rcv)
{
  theController->start(this);
  theController->vme_adc(ichp,ichn,rcv);
}


void VMEModule::irq_pthread_start()
{
  theController->start(this);
  theController->irq_pthread_start();
}

int VMEModule::irq_tester(int ival)
{
  theController->start(this);
  return theController->irq_tester(ival);
}

void VMEModule::irq_pthread_info()
{
  theController->start(this);
  theController->irq_pthread_info();

}

void VMEModule::irq_pthread_end()
{
  theController->start(this);
  theController->irq_pthread_end();
}

int VMEModule::CAEN_err()
{
  theController->start(this);
  return theController->caen_err;
}

void VMEModule::CAEN_err_reset()
{
  theController->start(this);
  theController->caen_err=0;
}

VMEController* VMEModule::getTheController(){
  return theController;
}

