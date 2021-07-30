// überall noch name von mir + lc3 fraunhofer
// CWrapper_new.c umbennen
// testall.cpp und projekt umbenennen


#include "lc3.h"
#include "functions.h"



double scale = 10000;


// ------------ PARAMETERS ------------

//hier trennen was bearbeitbar und was lassen
// erwähnen das sample rate etc mit RT übereinstimmen müsen, genau so auf RT seite erwähnen

int          setupCodecFlag = 0;
unsigned int sampleRate = 48000;    // Set sample rate
short        nChannels = 1;         // Set channels
int          i = 0;                 // Sample index
int          bitrate = 600000;
int          bipsOut = 24;
float        frame_ms = 5;
int          plcMeth = 0;
int          nBytes = 0;
unsigned int nSamples = 0;
int          encoder_size = 0, decoder_size = 0;
LC3_Enc* encoder = NULL;
LC3_Dec* decoder = NULL;
LC3_Error    err = LC3_OK;
int32_t      sample_buf_in[LC3_MAX_CHANNELS * LC3_MAX_SAMPLES] = { 0 };
int32_t      sample_buf_out[LC3_MAX_CHANNELS * LC3_MAX_SAMPLES] = { 0 };
int32_t      buf_24[LC3_MAX_CHANNELS * LC3_MAX_SAMPLES] = { 0 };
uint8_t      bytes[LC3_MAX_CHANNELS * LC3_MAX_BYTES] = { 0 };


// ------------ LOCAL FUNCTIONS ------------

static void    exit_if(int condition, const char* message);
static void    interleave(int32_t** in, int32_t* out, int n, int channels);
static void    deinterleave(int32_t* in, int32_t** out, int n, int channels);


// ------------ ERROR MESSAGES ------------

static const char* ERROR_MESSAGE[] = {
    "",                                                                     /* LC3_OK                  */
    "Function call failed!",                                                /* LC3_ERROR               */
    "Frame failed to decode and was concealed!",                            /* LC3_DECODE_ERROR        */
    "Pointer argument is null!",                                            /* LC3_NULL_ERROR          */
    "Invalid sampling rate!",                                               /* LC3_SAMPLERATE_ERROR    */
    "Invalid number of channels!",                                          /* LC3_CHANNELS_ERROR      */
    "Invalid bitrate!",                                                     /* LC3_BITRATE_ERROR       */
    "Invalid number of bytes!",                                             /* LC3_NUMBYTES_ERROR      */
    "Invalid ep mode!",                                                     /* LC3_EPMODE_ERROR        */
    "Invalid frame ms value!",                                              /* LC3_FRAMEMS_ERROR       */
    "Unaligned pointer!",                                                   /* LC3_ALIGN_ERROR         */
    "96 kHz sampling rate cannot be used without -hrmode option!",          /* LC3_HRMODE_ERROR        */
    "Bitrate has not been set!",                                            /* LC3_BITRATE_UNSET_ERROR */
    "Function can't be called after bitrate was set!",                      /* LC3_BITRATE_SET_ERROR   */
    "High resolution mode and bandwidth switching are exclusive!",          /* LC3_HRMODE_BW_ERROR     */
    "Invalid PLC method!",                                                  /* LC3_PLCMODE_ERROR       */
    "Invalid bandwidth frequency!"                                          /* LC3_BW_WARNING          */
};


// ------------ LC3 CODEC ------------

void lc3_Codec(double* oBuffer, double* iBuffer)
{
	if (setupCodecFlag == 0)
	{


        // ------------ SETUP ENCODER ------------

        fprintf(stdout, "Setting up encoder... \n");
        encoder_size = lc3_enc_get_size(sampleRate, nChannels);
        encoder = malloc(encoder_size);

        err = lc3_enc_init(encoder, sampleRate, nChannels);
        exit_if(err, ERROR_MESSAGE[err]);

        err = lc3_enc_set_frame_ms(encoder, frame_ms);
        exit_if(err, ERROR_MESSAGE[err]);

        err = lc3_enc_set_bitrate(encoder, bitrate);
        exit_if(err, ERROR_MESSAGE[err]);

        nSamples = lc3_enc_get_input_samples(encoder);
        fprintf(stdout, "Encoder set up! \n \n");


        // ------------ SETUP DECODER ------------

        fprintf(stdout, "Setting up decoder... \n");
        decoder_size = lc3_dec_get_size(sampleRate, nChannels);
        decoder = malloc(decoder_size);

        err = lc3_dec_init(decoder, sampleRate, nChannels, (LC3_PlcMode)plcMeth);
        exit_if(err, ERROR_MESSAGE[err]);

        err = lc3_dec_set_frame_ms(decoder, frame_ms);
        exit_if(err, ERROR_MESSAGE[err]);

        nSamples = lc3_dec_get_output_samples(decoder);
        fprintf(stdout, "Setting up decoder... \n \n");


        // ------------ PRINT INFO ------------

        printf("Encoder size:                  %i\n", encoder_size);
        printf("Decoder size:                  %i\n", decoder_size);
        printf("Sample rate:                   %i\n", sampleRate);
        printf("Channels:                      %i\n", nChannels);
        printf("Frame length:                  %i\n", nSamples);
        printf("Output format:                 %i bit\n", bipsOut);
        printf("Target bitrate:                %i\n", bitrate);
        printf("PLC mode:                      %i\n", plcMeth);


        // ------------ SET FLAG ------------

        setupCodecFlag = 1;
	}


    // ------------ ENCODER ROUTINE ------------

    int32_t* input24[LC3_MAX_CHANNELS];
    for (i = 0; i < nChannels; i++) {
        input24[i] = buf_24 + i * nSamples;
    }

    for (i = 0; i < nSamples; i++) {
        sample_buf_in[i] = iBuffer[i] * scale;
    }

    deinterleave(sample_buf_in, input24, nSamples, nChannels);
    err = lc3_enc24(encoder, input24, bytes, &nBytes);
    exit_if(err, ERROR_MESSAGE[err]);


    // ------------ DECODER ROUTINE ------------

    int32_t* output24[LC3_MAX_CHANNELS];
    for (i = 0; i < nChannels; i++) {
        output24[i] = buf_24 + i * nSamples;
    }

    err = lc3_dec24(decoder, bytes, nBytes, output24);
    exit_if(err && err != LC3_DECODE_ERROR, ERROR_MESSAGE[err]);
    interleave(output24, sample_buf_out, nSamples, nChannels);

    for (i = 0; i < nSamples; i++) {
        oBuffer[i] = sample_buf_out[i] / scale;
    }
}


// ------------ EXIT IF ------------

static void exit_if(int condition, const char* message)
{
    if (condition)
    {
        puts(message);
        if (condition < LC3_WARNING) {
            exit(1);
        }
    }
}


// ------------ INTERLEAVE ------------

static void interleave(int32_t** in, int32_t* out, int n, int channels)
{
    int ch, i;
    for (ch = 0; ch < channels; ch++)
    {
        for (i = 0; i < n; i++)
        {
            out[i * channels + ch] = in[ch][i];
        }
    }
}


// ------------ DEINTERLEAVE ------------

static void deinterleave(int32_t* in, int32_t** out, int n, int channels)
{
    int ch, i;
    for (ch = 0; ch < channels; ch++)
    {
        for (i = 0; i < n; i++)
        {
            out[ch][i] = in[i * channels + ch];
        }
    }
}