/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 25 Jul 2022 4:05:28pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class DJAudioPlayer : public AudioSource
{
public:
    /**
     * Constructor for audio player that initializes the format manager to recognize audio formats
     *
     * @param _formatManager          Format manager that maintains a list of available audio formats
     *
     * @return                        None
     */
    DJAudioPlayer(AudioFormatManager& _formatManager);

    /**
     * Destructor for the audio player
     *
     * @param                         None
     *
     * @return                        None
     */
    ~DJAudioPlayer();

    /**
     * Moves the audio source into a prepared state before fetching blocks of audio data
     *
     * @param samplesPerBlockExpected     Number of samples that the source will be expected to supply each time a subsequent block of data is fetched
     * @param sampleRate
     *
     * @return                            None
     */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /**
     * Callback called repeatedly to fetch subsequent blocks of audio data after audio source is prepared
     *
     * @param bufferToFille               Buffer that must be filed with audio data
     * @param
     *
     * @return                            None
     */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

    /**
     * Allow the audio source to release anything it no longer needs after playback has stopped
     *
     * @param                             None
     *
     * @return                            None
     */
    void releaseResources() override;

    /**
     * Create suitable reader for input stream to audio sources
     *
     * @param audioURL                    URL used to create an input stream
     *
     * @return                            None
     */
    void loadURL(URL audioURL);

    /**
     * Getter method that retrieves the relative position of the playhead
     *
     * @param                             None
     *
     * @return                            None
     */
    double getPositionRelative();

    /**
     * Setter method that sets the gain of the audio player
     *
     * @param gain                        Gain to update
     *
     * @return                            None
     */
    void setGain(double gain);

    /**
     * Setter method that sets the speed of the audio player
     *
     * @param ratio                       Speed to update
     *
     * @return                            None
     */
    void setSpeed(double ratio);

    /**
     * Setter method that sets the position of the audio player in seconds
     *
     * @param posInSecs                   Position to update
     *
     * @return                            None
     */
    void setPosition(double posInSecs);

    /**
     * Setter method that sets the relative position of the audio player
     *
     * @param posRelative                 Gain to update
     *
     * @return                            None
     */
    void setPositionRelative(double posRelative);

    /**
     * Setter method that sets the frequency range to band pass
     *
     * @param _bandPassFrequency          Band frequency to update
     *
     * @return                            None
     */
    void setBandPassFrequency(double _bandPassFrequency);

    /**
     * Setter method that sets the frequency range to low pass
     *
     * @param _lowPassFrequency           Low frequency to update
     *
     * @return                            None
     */
    void setLowPassFrequency(double _lowPassFrequency);

    /**
     * Setter method that sets the frequency range to high pass
     *
     * @param _highPassFrequency          High frequency to update
     *
     * @return                            None
     */
    void setHighPassFrequency(double _highPassFrequency);

    /**
     * Move the track position back two seconds
     *
     * @param                             None
     *
     * @return                            None
     */
    void movePositionBack();

    /**
     * Move the track position forwards two seconds
     *
     * @param                             None
     *
     * @return                            None
     */
    void movePositionForward();

    /**
     * Move the track position to the beginning of the audio track
     *
     * @param                             None
     *
     * @return                            None
     */
    void backToStart();

    /**
     * Getter method that retrieves the length of an audio track
     *
     * @param                             None
     *
     * @return                            Audio track length in seconds
     */
    double getSongLengthInSeconds();

    /**
     * Play the audio track from its current position
     *
     * @param                             None
     *
     * @return                            None
     */
    void start();

    /**
     * Stop the audio track at its current position
     *
     * @param                             None
     *
     * @return                            None
     */
    void stop();

    /**
    * Determine whether the audio track has ended
    *
    * @param                              None
    *
    * @return                             True if the track has ended, false otherwise
    */
    bool finishedPlaying();

    /**
    * Loop the audio source
    *
    * @param                              None
    *
    * @return                             None
    */
    void toggleAudioLoop();

    /**
    * Determines if the audio track is currently set to loop
    *
    * @param                              None
    *
    * @return                             True if the audio track is set to loop
    */
    bool isLooping();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;

    // Apply multiple audio filters to the audio source by chaining them sequentially

    // Chain the source that enables playback control into the source that enables sample rate modification 
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    // Chain sources that apply signal attenuation at varying frequencies into one another 
    IIRFilterAudioSource bandIIRFilterSource{ &resampleSource, false };
    IIRFilterAudioSource lowIIRFilterSource{ &bandIIRFilterSource, false };
    IIRFilterAudioSource highIIRFilterSource{ &lowIIRFilterSource, false };

    double bandPassFrequency;
    double lowPassFrequency;
    double highPassFrequency;

    double currentSampleRate;
    bool loopTrackAudio;
};

