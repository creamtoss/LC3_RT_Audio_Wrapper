    //---------------------------------------//
   //            lc3_wrapper.h              //
  //        by: Christoph Zimmer           //
 //  e-mail: zimmerchristoph97@gmail.com  //
//---------------------------------------//

// This file is the interface between the LC3 (in C) and RT-Audio (in C++).
// README.txt provides further information regarding the integration.

extern "C" {
	void lc3_Codec(double* oBuffer, double* iBuffer); 
}