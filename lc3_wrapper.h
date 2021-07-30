// This header is the interface between the LC3 Codec (in C) and RT Audio (in cpp)

extern "C" {
	void lc3_Codec(double* oBuffer, double* iBuffer); 
}