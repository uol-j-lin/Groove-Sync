#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

using namespace juce;

class MainComponent : public AudioAppComponent,
    public Slider::Listener,
    public Button::Listener
{
public:
    /**
     * Constructor that initializes the DJ application
     *
     * @param                         None
     *
     * @return                        None
     */
    MainComponent();

    /**
     * Destructor that cleans up the object and its members upon object destruction
     *
     * @param                         None
     *
     * @return                        None
     */
    ~MainComponent();

    /**
     * Moves the audio sources to their prepared states before fetching blocks of audio data
     *
     * @param samplesPerBlockExpected     Number of samples that the source will be expected to supply each time a subsequent block of data is fetched
     * @param sampleRate
     *
     * @return                            None
     */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /**
     * Callback called repeatedly to fetch subsequent blocks of audio data after audio sources are prepared
     *
     * @param bufferToFille               Buffer that must be filed with audio data
     * @param
     *
     * @return                            None
     */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

    /**
     * Allow the audio sources to release anything they no longer need after playback has stopped
     *
     * @param                             None
     *
     * @return                            None
     */
    void releaseResources() override;

    /**
     * Redraw a region of a component due to a change in the screen
     *
     * @param                         Graphics context for drawing a component or image
     *
     * @return                        None
     */
    void paint(Graphics& g) override;

    /**
     * Determine what should happen to the component when resized
     *
     * @param                         None
     *
     * @return                        None
     */
    void resized() override;

    /**
     * Called when a slider's value is changed due to dragging or typing in its text entry box
     *
     * @param                         Slider control
     *
     * @return                        None
     */
    void sliderValueChanged(Slider* slider) override;

    /**
    * Called when the 'Import Track', 'Export Library', or 'Import Library' buttons are clicked
    *
    * @param                         Button base class
    *
    * @return                        None
    */
    void buttonClicked(Button* button) override;

private:
    Slider crossFadeComponent;

    Label crossFadeLabel;
    Label searchInput;
    Label searchLabel;

    Label loadTracksLabel;
    Label savePlaylistLabel;
    Label loadPlaylistLabel;

    TextButton importTracksButton{ "Import Tracks" };
    TextButton exportLibraryButton{ "Export Library" };
    TextButton importLibraryButton{ "Import Library" };

    PlaylistComponent playlistComponent{ &searchInput };

    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{ 100 };

    DJAudioPlayer player1{ formatManager };
    DeckGUI deckGUI1{ &player1, formatManager, thumbCache, &playlistComponent };

    DJAudioPlayer player2{ formatManager };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache, &playlistComponent };

    MixerAudioSource mixerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};