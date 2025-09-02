#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <thread>
#include <atomic>
#include <functional>
#include <cmath>
#include <iostream>

template<typename T>
class CoreAudioWaveMaker {
public:
    // changed AudioFunction: now fills an output buffer for a block
    using AudioFunction = std::function<void(float*, UInt32)>;

    CoreAudioWaveMaker(AudioFunction userFunc, double sampleRate = 44100.0) 
        : m_userFunc(userFunc), m_sampleRate(sampleRate), m_globalTime(0.0) {}

    bool Start() {
        // Describe output unit
        AudioComponentDescription desc = {};
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_DefaultOutput;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;

        AudioComponent comp = AudioComponentFindNext(nullptr, &desc);
        if (!comp) return false;
        if (AudioComponentInstanceNew(comp, &m_audioUnit) != noErr) return false;

        // Set stream format
        AudioStreamBasicDescription streamDesc = {};
        streamDesc.mSampleRate = m_sampleRate;
        streamDesc.mFormatID = kAudioFormatLinearPCM;
        streamDesc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
        streamDesc.mFramesPerPacket = 1;
        streamDesc.mChannelsPerFrame = 1;
        streamDesc.mBitsPerChannel = sizeof(float) * 8;
        streamDesc.mBytesPerPacket = sizeof(float);
        streamDesc.mBytesPerFrame = sizeof(float);

        AudioUnitSetProperty(m_audioUnit,
                             kAudioUnitProperty_StreamFormat,
                             kAudioUnitScope_Input,
                             0,
                             &streamDesc,
                             sizeof(streamDesc));

        // Set render callback (unchanged)
        AURenderCallbackStruct callback = {};
        callback.inputProc = &RenderStatic;
        callback.inputProcRefCon = this;
        AudioUnitSetProperty(m_audioUnit,
                             kAudioUnitProperty_SetRenderCallback,
                             kAudioUnitScope_Input,
                             0,
                             &callback,
                             sizeof(callback));

        // Start audio
        AudioUnitInitialize(m_audioUnit);
        AudioOutputUnitStart(m_audioUnit);
        return true;
    }

    void Stop() {
        if (m_audioUnit) {
            AudioOutputUnitStop(m_audioUnit);
            AudioUnitUninitialize(m_audioUnit);
            AudioComponentInstanceDispose(m_audioUnit);
        }
    }

    double GetTime() const { return m_globalTime; }

private:
    static OSStatus RenderStatic(void* inRefCon,
                                 AudioUnitRenderActionFlags* ioActionFlags,
                                 const AudioTimeStamp* inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList* ioData) {
        return static_cast<CoreAudioWaveMaker*>(inRefCon)->Render(ioData, inNumberFrames);
    }

    OSStatus Render(AudioBufferList* ioData, UInt32 inNumberFrames) {
        float* buffer = reinterpret_cast<float*>(ioData->mBuffers[0].mData);

        // call user-provided block function once for the whole buffer
        if (m_userFunc) {
            m_userFunc(buffer, inNumberFrames);
        } else {
            for (UInt32 i = 0; i < inNumberFrames; i++) buffer[i] = 0.0f;
        }

        // advance global time by frames
        m_globalTime += (double)inNumberFrames / m_sampleRate;
        return noErr;
    }

    AudioUnit m_audioUnit = nullptr;
    AudioFunction m_userFunc;
    double m_sampleRate;
    double m_globalTime;
};