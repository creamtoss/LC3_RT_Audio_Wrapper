#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>

// Only include header of wrapper to seperate C from C++ code
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

// Set sample rate
unsigned int sampleRate = 48000;

// Set buffer / frame size
unsigned int bufferFrames = 240;

// Set number of channels
int channels = 1;




// This function serves as an interface to pass the input and output Buffer to the LC3 Codec
int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* data)
{
    double* oBuffer = (double*)outputBuffer;
    double* iBuffer = (double*)inputBuffer;

    //LC3 Codec is called
    lc3_Codec(oBuffer, iBuffer);

    return 0;
}

// ================================================================================
int main()
{

    // Setup RT Audio
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

    // ?
    double data[2];


    // Audio Stream and audioCallback function is called
    try {
        adac.openStream(&oParams, &iParams, FORMAT, sampleRate,
            &bufferFrames, &audioCallback, (void*)&data);
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
