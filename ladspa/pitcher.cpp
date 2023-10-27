#include <cstdio>
#include <cstdlib>
#include "ladspa.h"
#include <iostream>
#include <complex>
#include <stdatomic.h>
#include <StftPitchShift/FFT.h>
#include <StftPitchShift/RFFT.h>
                                              #include <StftPitchShift/SDFT.h>
#include <StftPitchShift/STFT.h>

#include <StftPitchShift/StftPitchShiftCore.h>

using namespace stftpitchshift;
using namespace std;


#define MUTED   0
#define UNMUTED !MUTED

// LADSPA_Handle
typedef struct {
    unsigned long sampleRate;

    double pitchChange; // factor for pitch change
    double timbreChange;   // factor for formant change

    LADSPA_Data *inputBuffer; // pointer to input buffer
    LADSPA_Data *outputBuffer; // pointer to output buffer

    unsigned long remaining; // how many samples are remained before I change the state
    unsigned int state; // muted or unmuted
} pitchplug;
std::shared_ptr<FFT> fft= std::make_shared<RFFT>();
StftPitchShiftCore<float> core(fft, 1024, 256, 44100);
// handle new instance
static LADSPA_Handle instantiateMuteLine(const LADSPA_Descriptor *descriptor,
                                      unsigned long sampleRate) {

    pitchplug *pitched;

    pitched = (pitchplug *)malloc(sizeof(pitchplug));

    if (pitched == NULL) {
        return NULL;
    }
    pitched->sampleRate = sampleRate;

    return pitched;
}

// Ports
#define PORT_COUNT       4
#define ML_PITCH_CHANGE  0
#define ML_TIMBRE_CHANGE   1
#define ML_INPUT         2
#define ML_OUTPUT        3

// Assign given parameters accordingly in handle
static void connectPort(LADSPA_Handle handle, unsigned long port, LADSPA_Data *data) {
    pitchplug *pitched = (pitchplug *) handle;

    switch (port) {
    case ML_PITCH_CHANGE:
        pitched->pitchChange = (unsigned int)*data;
        break;
    case ML_TIMBRE_CHANGE:
        pitched->timbreChange = (unsigned int)*data;
        break;
    case ML_INPUT:
        pitched->inputBuffer = data;
        break;
    case ML_OUTPUT:
        pitched->outputBuffer = data;
        break;
    }
}

// initialize the state
static void activateMuteLine(LADSPA_Handle handle) {
    pitchplug *pitched = (pitchplug *)handle;

    pitched->state = MUTED;
    pitched->remaining = 0;
}

// main handler. forward samples or mute according to state
static void runMuteLine(LADSPA_Handle handle, unsigned long sampleCount) {
    pitchplug *pitched = (pitchplug *) handle;
vector<float> input(sampleCount);
vector<float> output(sampleCount);
std::vector<double> factors = {pitched->pitchChange};
cout << sampleCount;
cout << '\n';
core.factors(factors);
core.quefrency(1);
core.distortion(pitched->timbreChange);
for (unsigned long i = 0; i < sampleCount; i++) {
    input[i] = pitched->inputBuffer[i];
}
SDFT<float> sdft(1024, 256, false);
sdft(input.size(), input.data(), output.data(), [&](std::vector<std::complex<float>>& dft)         
    {
       core.shiftpitch(dft);
     });
for (unsigned long j= 0; j < sampleCount; j++) {
    pitched->outputBuffer[j] = output[j];
}
}

static void cleanupMuteLine(LADSPA_Handle handle) {
    pitchplug *pitched = (pitchplug *) handle;

    free(handle);
}

static LADSPA_Descriptor *descriptor = NULL;

// On plugin load
static void __attribute__ ((constructor)) init() {
    LADSPA_PortDescriptor * portDescriptors;
    LADSPA_PortRangeHint * portRangeHints;
    char ** portNames;

    descriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

    if (!descriptor) {
        return;
    }

    descriptor->UniqueID  = 109124; // should be unique
    descriptor->Label     = "pitcher";
    descriptor->Name      = "pitchplug";
    descriptor->Maker     = "Rysertio";
    descriptor->Copyright = "(c) 2023, Rysertio";

    descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;

    descriptor->PortCount = PORT_COUNT;
    
    portDescriptors = (LADSPA_PortDescriptor *) calloc(PORT_COUNT, sizeof(LADSPA_PortDescriptor));

    portDescriptors[ML_PITCH_CHANGE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    portDescriptors[ML_TIMBRE_CHANGE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    portDescriptors[ML_INPUT] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    portDescriptors[ML_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;

    descriptor->PortDescriptors = portDescriptors;

    portNames = (char **) calloc(PORT_COUNT, sizeof(char *));
    portNames[ML_PITCH_CHANGE] = "pitch change factor";
    portNames[ML_TIMBRE_CHANGE] = "timbre change factor";
    portNames[ML_INPUT] = "Input";
    portNames[ML_OUTPUT] = "Output";

    descriptor->PortNames = (const char * const *) portNames;

    portRangeHints = (LADSPA_PortRangeHint *) calloc(PORT_COUNT, sizeof(LADSPA_PortRangeHint));

    portRangeHints[ML_PITCH_CHANGE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_DEFAULT_1;
    portRangeHints[ML_PITCH_CHANGE].LowerBound = 1;
    portRangeHints[ML_TIMBRE_CHANGE].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_DEFAULT_1;
    portRangeHints[ML_TIMBRE_CHANGE].LowerBound = 1;
    portRangeHints[ML_INPUT].HintDescriptor = 0;
    portRangeHints[ML_OUTPUT].HintDescriptor = 0;

    descriptor->PortRangeHints = portRangeHints;

    descriptor->instantiate = instantiateMuteLine;
    descriptor->connect_port = connectPort;
    descriptor->activate = activateMuteLine;
    descriptor->run = runMuteLine;
    descriptor->run_adding = NULL;
    descriptor->set_run_adding_gain = NULL;
    descriptor->deactivate = NULL;
    descriptor->cleanup = cleanupMuteLine;
}

// On plugin unload
static void __attribute__ ((destructor)) fini() {
    return;

}

// we only have one type of plugin
const LADSPA_Descriptor * ladspa_descriptor(unsigned long index) {
    if (index != 0) {
        return NULL;
    }

    return descriptor;
}
