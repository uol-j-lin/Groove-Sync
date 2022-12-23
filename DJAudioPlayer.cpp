/*
==============================================================================

DJAudioPlayer.cpp
Created: 25 Jul 2022 4:05:28pm
Author:  Jonathan

==============================================================================
*/

#include "DJAudioPlayer.h"
/**
 * Constructor for audio player that initializes the format manager to recognize audio formats
 *
 * @param _formatManager          Format manager that maintains a list of available audio formats
 *
 * @return                        None
 */
DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
    : formatManager(_formatManager), loopTrackAudio(false)
{
}

/**
 * Destructor for the audio player
 *
 * @param                         None
 *
 * @return                        None
 */
DJAudioPlayer::~DJAudioPlayer()
{
}

/**
 * Moves the audio source into a prepared state before fetching blocks of audio data
 *
 * @param samplesPerBlockExpected     Number of samples that the source will be expected to supply each time a subsequent block of data is fetched
 * @param sampleRate
 *
 * @return                            None
 */
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    bandIIRFilterSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    lowIIRFilterSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    highIIRFilterSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    currentSampleRate = sampleRate;
}

/**
 * Callback called repeatedly to fetch subsequent blocks of audio data after audio source is prepared
 *
 * @param bufferToFille               Buffer that must be filed with audio data
 * @param
 *
 * @return                            None
 */
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    highIIRFilterSource.getNextAudioBlock(bufferToFill);
}

/**
 * Allow the audio source to release anything it no longer needs after playback has stopped
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();

    bandIIRFilterSource.releaseResources();
    lowIIRFilterSource.releaseResources();
    highIIRFilterSource.releaseResources();
}

/**
 * Create suitable reader for input stream to audio sources
 *
 * @param audioURL                    URL used to create an input stream
 *
 * @return                            None
 */
void DJAudioPlayer::loadURL(URL audioURL)
{
    // Create suitable reader for input stream based on known formats
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    // Valid reader
    if (reader != nullptr)
    {
        // Wrap format reader in audio source that integrates with audio system life cycle
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
            true));

        // Set reader used as input source to the transport source, which controls playback
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);

        // Pass ownership of audio format reader source to class scope variable to keep playing it
        readerSource.reset(newSource.release());
    }
}

/**
 * Getter method that retrieves the relative position of the playhead
 *
 * @param                             None
 *
 * @return                            None
 */
double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

/**
 * Setter method that sets the gain of the audio player
 *
 * @param gain                        Gain to update
 *
 * @return                            None
 */
void DJAudioPlayer::setGain(double gain)
{
    if (gain >= 0 && gain <= 1.0)
    {
        transportSource.setGain(gain);
    }
}

/**
 * Setter method that sets the speed of the audio player
 *
 * @param ratio                       Speed to update
 *
 * @return                            None
 */
void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio >= 0 && ratio <= 5.0)
    {
        resampleSource.setResamplingRatio(ratio);
    }
}

/**
 * Setter method that sets the position of the audio player in seconds
 *
 * @param posInSecs                   Position to update
 *
 * @return                            None
 */
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

/**
 * Setter method that sets the relative position of the audio player
 *
 * @param posRelative                 Gain to update
 *
 * @return                            None
 */
void DJAudioPlayer::setPositionRelative(double posRelative)
{
    if (posRelative >= 0 && posRelative <= 1.0)
    {
        double posInSecs = transportSource.getLengthInSeconds() * posRelative;
        setPosition(posInSecs);
    }
}

/**
 * Setter method that sets the frequency range to band pass
 *
 * @param _bandPassFrequency          Band frequency to update
 *
 * @return                            None
 */
void DJAudioPlayer::setBandPassFrequency(double _bandPassFrequency)
{
    bandPassFrequency = _bandPassFrequency;

    // Set coefficients for a band pass filter
    bandIIRFilterSource.setCoefficients(IIRCoefficients::makeBandPass(currentSampleRate, bandPassFrequency));
}

/**
 * Setter method that sets the frequency range to low pass
 *
 * @param _lowPassFrequency           Low frequency to update
 *
 * @return                            None
 */
void DJAudioPlayer::setLowPassFrequency(double _lowPassFrequency)
{
    lowPassFrequency = _lowPassFrequency;

    // Set coefficients for a low pass filter
    lowIIRFilterSource.setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, lowPassFrequency));
}

/**
 * Setter method that sets the frequency range to high pass
 *
 * @param _highPassFrequency          High frequency to update
 *
 * @return                            None
 */
void DJAudioPlayer::setHighPassFrequency(double _highPassFrequency)
{
    highPassFrequency = _highPassFrequency;

    // Set coefficients for a high pass filter
    highIIRFilterSource.setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, highPassFrequency));
}

/**
 * Move the track position back two seconds
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::movePositionBack()
{
    double backwardPosition = (transportSource.getCurrentPosition() - 2) >= 0 ? transportSource.getCurrentPosition() - 2 : 0;
    transportSource.setPosition(backwardPosition);
}

/**
 * Move the track position forwards two seconds
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::movePositionForward()
{
    double forwardPosition = (transportSource.getCurrentPosition() + 2) <= transportSource.getLengthInSeconds() ? transportSource.getCurrentPosition() + 2 : transportSource.getLengthInSeconds();
    transportSource.setPosition(forwardPosition);
}

/**
 * Move the track position to the beginning of the audio track
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::backToStart()
{
    double startPosition = 0;
    transportSource.setPosition(0);
}

/**
 * Getter method that retrieves the length of an audio track
 *
 * @param                             None
 *
 * @return                            Audio track length in seconds
 */
double DJAudioPlayer::getSongLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

/**
 * Play the audio track from its current position
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::start()
{
    transportSource.start();
}

/**
 * Stop the audio track at its current position
 *
 * @param                             None
 *
 * @return                            None
 */
void DJAudioPlayer::stop()
{
    transportSource.stop();
}

/**
 * Determine whether the audio track has ended
 *
 * @param                              None
 *
 * @return                             True if the track has ended, false otherwise
 */
bool DJAudioPlayer::finishedPlaying()
{
    // Account for floating point rounding errors
    return abs(getPositionRelative() - 1.00) < 1e-2;
}

/**
 * Loop the audio source
 *
 * @param                              None
 *
 * @return                             None
 */
void DJAudioPlayer::toggleAudioLoop()
{
    loopTrackAudio = !loopTrackAudio;
}

/**
 * Determines if the audio track is currently set to loop
 *
 * @param                              None
 *
 * @return                             True if the audio track is set to loop
 */
bool DJAudioPlayer::isLooping()
{
    return loopTrackAudio;
}