/*
  ==============================================================================

    WaveformDisplay.h
    Created: 26 Jul 2022 4:05:16pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class WaveformDisplay : public Component,
    public ChangeListener,
    public ChangeBroadcaster
{
public:
    /**
     * Constructor that initializes the waveform component
     *
     * @param formatManageToUse       Manage available audio formats
     * @param cacheToUse              Cache runs background thread for AudioThumbnails
     *
     * @return                        None
     */
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse);

    /**
     * Destructor that performs clean up about object deallocation
     *
     * @param formatManageToUse       None
     *
     * @return                        None
     */
    ~WaveformDisplay();

    /**
     * Redraw a region of a component due to a change in the screen
     *
     * @param                         Graphics context for drawing a component or image
     *
     * @return                        None
     */
    void paint(Graphics&) override;

    /**
     * Create a rectangle in the waveform that determines the region before the playhead whose audio has been played
     *
     * @param audioThumb              Enables drawing of scaled views of the waveform shape of an audio file
     *
     * @return                        Rectangle which contains region of played audio
     */
    const Rectangle<int> createWaveformBeforePlayhead(AudioThumbnail& audioThumb);

    /**
    * Create a rectangle in the waveform that determines the region after the playhead whose audio has not been played
    *
    * @param audioThumb              Enables drawing of scaled views of the waveform shape of an audio file
    *
    * @return                        Rectangle which contains region of audio that has not been played
    */
    const Rectangle<int> createWaveformAfterPlayhead(AudioThumbnail& audioThumb);

    /**
     * Determine what should happen to the component when resized
     *
     * @param                         None
     *
     * @return                        None
     */
    void resized() override;

    /**
     * Specify the file or stream that contains the audio file
     *
     * @param audioURL                URL used as an input source to the drawing of the waveform
     *
     * @return                        None
     */
    void loadURL(URL audioURL);

    /**
     * Receive change event callback due to changes in a ChangeBroadcaster
     *
     * @param source                  ChangeBroadcaster that triggered the callback
     *
     * @return                        None
     */
    void changeListenerCallback(ChangeBroadcaster* source) override;

    /**
     * Getter that retrieves the relative position of the playhead
     *
     * @param                         None
     *
     * @return                        Relative position of playhead
     */
    double getPositionRelative();

    /**
     * Setter that sets the relative position of the playhead
     *
     * @param posRelative             Relative position to update
     *
     * @return                        None
     */
    void setPositionRelative(double posRelative);

    /**
     * Called when a mouse button is pressed
     *
     * This broadcasts the event to the audio player to update the playhead and audio track position
     *
     * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
     *
     * @return                        None
     */
    void mouseDown(const MouseEvent& event) override;

    /**
     * Called when a mouse is moved while a button is held down
     *
     * This broadcasts the event to the audio player to update the playhead and audio track position
     *
     * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
     *
     * @return                        None
     */
    void mouseDrag(const MouseEvent& event) override;

    /**
     * Called when a mouse button is released
     *
     * This broadcasts the event to the audio player to update the playhead and audio track position
     the
     * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
     *
     * @return                        None
     */
    void mouseUp(const MouseEvent& event) override;

    /**
    * Store the relative position of a cue marker
    *
    * @param cueNumber               A number from one to three to identify the marker
    * @param relativePosition        Relative position of marker
    *
    * @return                        None
    */
    void insertCueRelativePosition(int cueNumber, double relativePosition);

    /**
    * Play the audio track from the cue marker, given that it exists
    *
    * @param cueNumber               A number from one to three to identify the cue marker
    *
    * @return                        None
    */
    void playTrackFromCueMarker(int cueNumber);
    std::unordered_map<int, double> waveformHotCues;



private:
    AudioThumbnail audioThumb;

    bool fileLoaded;

    double positionRelative;

    //std::unordered_map<int, double> waveformHotCues;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
