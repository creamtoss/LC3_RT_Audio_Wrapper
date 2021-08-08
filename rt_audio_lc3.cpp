    //---------------------------------------//
   //          rt_audio_lc3.cpp             //
  //        by: Christoph Zimmer           //
 //  e-mail: zimmerchristoph97@gmail.com  //
//---------------------------------------//

// This file provices the main() function, the setup of RT-Audio and the audioCallback() function providing the input and output buffer.
// It will replace one of the RT-Audio test projects files containing the main() function (e.g. duplex.cpp).
// README.txt provides further information regarding the integration.

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>

// Only include the header of lc3_wrapper to seperate the C code from the C++ code.
#include "lc3_wrapper.h" 

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


// ------------ PARAMETERS ------------

// The following parameters can be modified but must align the parameters of lc3_wrapper.c.

unsigned int sampleRate = 48000;

unsigned int bufferFrames = 240; // bufferFrames = sampleRate * frame_ms (of lc3_wrapper.c) * 10^(-3)

int channels = 1; // For this integration only channels = 1 has been tested.


// ------------ AUDIO CALLBACK ------------

int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* data)
{
    double* oBuffer = (double*)outputBuffer;
    double* iBuffer = (double*)inputBuffer;

    //LC3 Codec is called
    lc3_Codec(oBuffer, iBuffer);

    return 0;
}


// ------------ MAIN FUNCTION ------------

int main()
{

    // ------------ RT-AUDIO SETUP ------------
    RtAudio adac;

    if (adac.getDeviceCount() < 1) {
        std::cout << "\nNo audio devices found!" << "\n";
        exit(0);
    }

    RtAudio::StreamParameters iParams, oParams;

    //Setup input
    iParams.deviceId = adac.getDefaultInputDevice();
    iParams.nChannels = channels;

    //Setup output
    oParams.deviceId = adac.getDefaultOutputDevice();
    oParams.nChannels = channels;

    double data[2];


    // ------------ CALL AUDIO STREAM ------------
    try {
        adac.openStream(&oParams, &iParams, FORMAT, sampleRate, &bufferFrames, 
            &audioCallback, // The audioCallback() function is called for each frame.
            (void*)&data);
    }
    catch (RtAudioError& e) {
        e.printMessage();
        exit(0);
    }

    try {
        adac.startStream();
        char input;
        std::cout << "\nPlaying ... press <enter> to quit." << "\n";
        std::cin.get(input);
        std::cin.get(input);
        adac.stopStream();
    }
    catch (RtAudioError& e) {
        e.printMessage();
        goto cleanup;
    }
cleanup:
    if (adac.isStreamOpen()) adac.closeStream();

    return 0;
}
