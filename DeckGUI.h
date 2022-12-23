/*
  ==============================================================================

    DeckGUI.h
    Created: 26 Jul 2022 3:54:28pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "CustomDial.h"
#include "PlaylistComponent.h"
#include "PlaylistQueue.h"

using namespace juce;

class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
        /**
            * Register a color for the slider thumb
            *
            * @param thumbColourId      Slider thumb color
            *
            * @return                   None
            */
        setColour(Slider::thumbColourId, Colours::orangered);

        /**
            * Register a color for the slider outline
            *
            * @param thumbColourId      Slider outline color
            *
            * @return                   None
            */
        setColour(Slider::rotarySliderOutlineColourId, Colours::darkorange);

        /**
            * Register a color for the slider area bounded by the outline
            *
            * @param thumbColourId      Slider fill color
            *
            * @return                   None
            */
        setColour(Slider::rotarySliderFillColourId, Colours::ghostwhite);
    }
};

class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public FileDragAndDropTarget,
    public Timer,
    public ChangeListener,
    public DragAndDropTarget
{
public:
    /**
     * DeckGUI constructor to initialize child components, set up graphical environment, and begin background threads
     *
     * @param _player                 Audio player
     * @param formatManagerToUse      Format manager that records available formats
     * @param cacheToUse              Cache that maintains low-res previews of audio files in memory
     * @param _playlistComponent      Library that stores audio track meta data
     *
     * @return                        None
     */
    DeckGUI(DJAudioPlayer* _player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse,
        PlaylistComponent* _playlistComponent);

    /**
     * DeckGUI destructor
     *
     * @param                         None
     *
     * @return                        None
     */
    ~DeckGUI();

    /**
     * Redraw a region of a component due to a change in the screen
     *
     * @param                         Graphics context for drawing a component or image
     *
     * @return                        None
     */
    void paint(Graphics&) override;

    /**
     * Determine what should happen to the component when resized
     *
     * @param                         None
     *
     * @return                        None
     */
    void resized() override;

    /**
     * Called when a button is clicked
     *
     * @param                         Button base class
     *
     * @return                        None
     */
    void buttonClicked(Button*) override;

    /**
     * Called when a slider's value is changed due to dragging or typing in its text entry box
     *
     * @param                         Slider control
     *
     * @return                        None
     */
    void sliderValueChanged(Slider* slider) override;

    /**
     * Callback to check whether this target is interested in the set of files being offered
     *
     * @param files                   The set of absolute pathnames of the files that the user is dragging
     *
     * @return                        True if the component wants to receive other callbacks of this type of object, false if no other callbacks will be made
     */
    bool isInterestedInFileDrag(const StringArray& files) override;

    /**
     * Callback to indicate that the user has dropped the files onto this component
     *
     * @param files                   The set of absolute pathnames of the files that the user is dragging
     * @param x                       Mouse x position, relative to component
     * @param y                       Mouse y position, relative to component
     *
     * @return                        None
     */
    void filesDropped(const StringArray& files, int x, int y) override;

    /**
     * Callback routine that gets periodically to display the playhead location and song position to the user
     *
     * @param                         None
     *
     * @return                        None
     */
    void timerCallback() override;

    /**
     * Receive callback due to changes in mouse behavior to update the position of the playhead
     *
     * @param source                  ChangeBroadcaster that triggered the callback
     *
     * @return                        None
     */
    void changeListenerCallback(ChangeBroadcaster* source);

    /**
    * Callback to determine whether this target is interested in the type of object being dragged
    *
    * @param dragSourceDetails       Contains information about the source of the drag operation
    *
    * @return                        True if component wants to receive callbacks regarding this type of object, false otherwise
    */
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;

    /** 
    * Callback to indicate that the user has dropped something onto this component
    *
    * @param dragSourceDetails       Contains information about the source of the drag operation
    *
    * @return                        None
    */
    void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
    TextButton loadButton{ "Load Deck" };
    TextButton queueTrackButton{ "Queue Track" };

    TextButton firstCueMarker{ "Cue 1" };
    TextButton secondCueMarker{ "Cue 2" };
    TextButton thirdCueMarker{ "Cue 3" };

    ImageButton playImageButton;
    ImageButton pauseImageButton;
    ImageButton stopImageButton;
    ImageButton rewindImageButton;
    ImageButton fastForwardImageButton;
    ImageButton loopImageButton;

    ImageButton playFirstCueButton;
    ImageButton playSecondCueButton;
    ImageButton playThirdCueButton;

    Image playButtonGraphic;
    Image pauseButtonGraphic;
    Image stopButtonGraphic;
    Image rewindButtonGraphic;
    Image fastForwardButtonGraphic;
    Image loopButtonGraphic;
    Image vinylGraphic;

    Image firstCuePlayer;
    Image secondCuePlayer;
    Image thirdCuePlayer;

    Slider volSlider;
    Slider bandPassSlider;
    Slider lowPassSlider;
    Slider highPassSlider;
    Slider speedSlider;

    Label volLabel;
    Label bandPassLabel;
    Label lowPassLabel;
    Label highPassLabel;
    Label speedLabel;
    Label songLengthLabel;
    Label songPositionLabel;
    Label songTitleLabel;
    Label DJAppLabel;

    OtherLookAndFeel speedDialLookAndFeel;

    WaveformDisplay waveformDisplay;

    DJAudioPlayer* player;

    PlaylistComponent* playlistComponent;

    PlaylistQueue playlistQueue;

    CustomDial customDial;

    float rotationAngle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
