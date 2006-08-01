#ifndef EMUjtag_h
#define EMUjtag_h

#include <string>
#include <fstream>
#include "EMU_JTAG_constants.h"

class TMB;
class EMUjtag 
{
public:
  // *****************************************************************************************
  // ** N.B. In this class, the data arrays are ordered such that the least significant bit **
  // ** (array index 0) is the first bit which is shifted into or out of the JTAG chain.    **
  // *****************************************************************************************
  //
  //EMUjtag();
  EMUjtag(TMB * );
  ~EMUjtag();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  /////////////////////////////////////////////////////////////////////////////
  // set up your JTAG stuff correctly:
  /////////////////////////////////////////////////////////////////////////////
  void setup_jtag(int JTAGchain);
  //
  //////////////////////////////////////////////////////////////////////////////////////////////
  // Use EMUjtag to shift data into Instruction Registers and into (and out of) Data Registers
  //////////////////////////////////////////////////////////////////////////////////////////////
  // See EMU_JTAG_constants.h for the values of
  //    - JTAGchain 
  //    - selected_chip
  //    - opcode
  //    - length_of_register
  // for following methods:
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register,
		   const int * tdi_to_shift_in);  //1 bit per index
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register); //drop tdi argument for read-only registers
  //
  inline int * GetDRtdo() { return chip_tdo_in_bits_ ; }
  inline int GetRegLength() { return register_length_ ; }
  //
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // Check if JTAG read values = JTAG write values:
  //  ... recommended use:  after write command, call CompareBitByBit if GetCheckJtagWrite()==true
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  inline void SetCheckJtagWrite(bool check_write) { check_write_ = check_write; }   
  inline bool GetCheckJtagWrite() { return check_write_; }
  //
  void CompareBitByBit(int * write_vector,     // Compare the data in the first "length" bits
		       int * read_vector,      // of "write_vector[]" with "read_vector[]"
		       int length);           
  //
  //
  ///////////////////////////////////////////////////////////////
  // Useful functions to convert one type into another type
  ///////////////////////////////////////////////////////////////
  void packCharBuffer(int * vector_of_bits,    // pack a vector of bits into a vector of characters
		      int Nbits,               // number of bits to pack
		      char * vector_of_char); 
  //
  void unpackCharBuffer(char * vector_of_char, // turn a vector of characters into a vector of bits
			int length,            // length of vector of bits
			int firstBit,          // first bit to begin (i.e., vector_of_bits[0] = (vector_of_char>>firstBit)
			int * vector_of_bits);
  //
  int bits_to_int(int * vector_of_bits,        // convert a vector of bits into an integer
		  int length_of_vector,        // length of vector of bits
		  int start_from_MsbOrLsb);    // start_from = LSBfirst => LSB of return integer = vector_of_bits[0]
  //                                                         = MSBfirst => LSB of return integer = vector_of_bits[length_of_vector-1]
  //
  void int_to_bits(int value_to_expand,        // Turn an integer into a vector of bits
		   int number_of_bits,         // number of bits into which to expand integer
		   int * vector_of_bits,
		   int start_from_MsbOrLsb);   // start_from = LSBfirst => vector_of_bits[0] = LSB of "value_to_expand"
  //                                                         = MSBfirst => vector_of_bits[0] = MSB of "value_to_expand"
  //
  //
  ////////////////////////////////////////////////////////////////////////
  // XSVF programming:
  ////////////////////////////////////////////////////////////////////////
  void SetXsvfFilename(std::string filename);       //set the base filename by which the rest of the filenames are referred to
  //
  //------------------------------------------------
  // prom image 
  //------------------------------------------------
  void CreateUserPromFile();                        //create Prom Image File with configuration data
  bool ReadUserPromFile();                          //read Prom Image File from disk
  //
  int GetUserPromImage(int address);                //address=[0 - (TOTAL_NUMBER_OF_ADDRESSES-1)]
  //------------------------------------------------
  // XSVF image
  //------------------------------------------------
  void SetWhichUserProm(int device);          //device = [ChipLocationTmbUserPromTMB, ChipLocationTmbUserPromALCT]
  int  GetWhichUserProm();
  //
  void CreateXsvfFile();                      //creates XSVF file from Prom Image File for GetWhichUserProm()
  //
  void ReadXsvfFile(bool create_logfile);     //read XSVF file from disk
  void ReadXsvfFile();                        //no argument given => no logfile created
  //
  //
  ////////////////////////////////////////////////////////////////////////
  // SVF programming:
  ////////////////////////////////////////////////////////////////////////
  void Parse(char *buf,int *Count,char **Word);
  int  SVFLoad(int*, const char *, int);
  //
  //
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  bool debug_;
  bool check_write_;
  /////////////////////////////////
  // setup private values:
  /////////////////////////////////
  void SetWriteToDevice_(bool communicate);
  bool GetWriteToDevice_();
  bool write_to_device_;
  //
  int jtag_chain_;
  int devices_in_chain_;
  //
  //
  /////////////////////////////////
  // ShfIR_ShfDR private values:
  /////////////////////////////////
  void ShfIR_(const int selected_chip, 
	      const int opcode);
  void ShfDR_(const int selected_chip, 
	      const int size_of_register, 
	      const int * write_data);
  //
  int bits_in_opcode_[MAX_NUM_DEVICES];
  int chip_id_;
  int register_length_;
  // The following are ordered such that:
  //    - LSB of byte index = 0 = first bit which is shifted in/out
  //    - bit index = 0 = first bit which is shifted in/out
  int tdi_in_bits_[MAX_NUM_FRAMES];        // -> tdi for full chain (including bypass bits)
  int chip_tdo_in_bits_[MAX_NUM_FRAMES];   // -> tdo only for chip we are interested in
  char tdi_in_bytes_[MAX_BUFFER_SIZE];     // -> tdi for full chain (including bypass bits)
  char tdo_in_bytes_[MAX_BUFFER_SIZE];     // -> tdo for full chain (including bypass bits)
  char tdo_mask_in_bytes_[MAX_BUFFER_SIZE];
  //
  ////////////////////////////
  // XSVF private values:
  ////////////////////////////
  //----------------------
  // file stuff
  //----------------------
  std::string filename_dat_;
  std::string filename_xsvf_;
  std::string filename_log_;
  //
  std::string AddTrailer_(std::string filename,
			  std::string trailer);
  //
  std::ofstream Logfile_;
  //
  //-------------------
  // prom image stuff
  //-------------------
  int address_counter_;
  int read_ascii_prom_image_[TOTAL_NUMBER_OF_ADDRESSES];
  int write_ascii_prom_image_[TOTAL_NUMBER_OF_ADDRESSES];
  //
  void InsertBlockBoundaries_(int * data_to_go_into_prom, 
			      int number_of_data_words_to_go_into_prom);
  void SetUserPromImage_(int address, int value);                 
  void WritePromDataToDisk_();
  //
  //
  //-------------------
  // xsvf file stuff
  //-------------------
  int image_counter_;
  int xdr_length_;
  //
  int which_user_prom_;
  char write_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  int number_of_write_bytes_;
  //
  bool CreateXsvfImage_();
  void WritePreambleIntoXsvfImage_();
  void WritePromImageIntoXsvfImage_();
  void WriteInterimBetweenImageAndVerifyIntoXsvfImage_();
  void WritePromImageIntoXsvfVerify_();
  void WritePostambleIntoXsvfImage_();
  void WriteXsvfImageToDisk_();
  //
  void SetWriteXsvfImage_(int address,int value);
  void WriteXCOMPLETE_();
  void WriteXTDOMASK_(int number_of_bits_in_mask,
		      int * vector_of_bits_for_mask);
  void WriteXSIR_(int opcode);
  int  SumOpcodeBits_();
  void WriteXRUNTEST_(int length_of_time);         //length of time = pause in microseconds
  void WriteXREPEAT_(int number_of_times);
  void WriteXSDRSIZE_(int number_of_bits_in_data_register);
  void WriteXSDRTDO_(int number_of_bits_in_data_register,
		     int * tdi_bits,
		     int * tdo_expected_bits);
  void WriteXSTATE_(int state);                    //state = [TLR, RTI]
  //
  //
  //
  int read_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  int number_of_read_bytes_;
  //
  int  GetReadXsvfImage_(int address);
  //
  void DecodeXsvfImage_();
  void ParseXCOMPLETE_();
  void ParseXTDOMASK_();
  void ParseXSIR_();
  void ParseXRUNTEST_();
  void ParseXREPEAT_();
  void ParseXSDRSIZE_();
  void ParseXSDRTDO_();
  void ParseXSTATE_();
  //
  int NumberOfCommands_[NUMBER_OF_DIFFERENT_XSVF_COMMANDS];
  //
  //
};

#endif

