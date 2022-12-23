#include "MainComponent.h"

/**
 * Constructor that initializes the DJ application
 *
 * @param                         None
 *
 * @return                        None
 */
MainComponent::MainComponent()
{
    setSize(900, 650);

    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Garamond");

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio) && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
            [&](bool granted)
            { if (granted)  setAudioChannels(2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }

    // Set up user interface component of both DJ decks
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);

    // Set up library component
    addAndMakeVisible(playlistComponent);

    // Set up search input area
    addAndMakeVisible(searchInput);
    searchInput.setFont(Font(12.0f, Font::bold));
    searchInput.setEditable(true);
    searchInput.setColour(Label::outlineColourId, Colours::lightskyblue);
    searchInput.setText("", dontSendNotification);
    searchInput.setJustificationType(Justification::left);

    // Update search results based on user input
    searchInput.onTextChange = [this]
    { playlistComponent.setSearchResults(searchInput.getText().toStdString()); };

    // Set up search label
    addAndMakeVisible(searchLabel);
    searchLabel.setFont(Font(11.0f, Font::bold));
    searchLabel.setText("Search Library", dontSendNotification);
    searchLabel.setColour(Label::textColourId, Colours::white);
    searchLabel.attachToComponent(&searchInput, false);

    // Set up the 'Import Tracks' button
    addAndMakeVisible(importTracksButton);
    importTracksButton.addListener(this);
    importTracksButton.setColour(TextButton::ColourIds::buttonColourId, Colour(68, 73, 240));

    // Set up the 'Export Library' button
    addAndMakeVisible(exportLibraryButton);
    exportLibraryButton.addListener(this);
    exportLibraryButton.setColour(TextButton::ColourIds::buttonColourId, Colour(202, 38, 39));

    // Set up the 'Import Library' button
    addAndMakeVisible(importLibraryButton);
    importLibraryButton.addListener(this);
    importLibraryButton.setColour(TextButton::ColourIds::buttonColourId, Colour(186, 5, 5));

    // Add label to 'Import Track' button
    addAndMakeVisible(loadTracksLabel);
    loadTracksLabel.setText("Load Audio", dontSendNotification);
    loadTracksLabel.attachToComponent(&importTracksButton, false);
    loadTracksLabel.setJustificationType(Justification::bottomLeft);
    loadTracksLabel.setFont(Font(11.0f, Font::bold));

    // Add label to 'Import Library' button
    addAndMakeVisible(savePlaylistLabel);
    savePlaylistLabel.setText("Load Library", dontSendNotification);
    savePlaylistLabel.attachToComponent(&importLibraryButton, false);
    savePlaylistLabel.setJustificationType(Justification::bottomLeft);
    savePlaylistLabel.setFont(Font(11.0f, Font::bold));

    // Add label to 'Export Library' button
    addAndMakeVisible(loadPlaylistLabel);
    loadPlaylistLabel.setText("Save Library", dontSendNotification);
    loadPlaylistLabel.attachToComponent(&exportLibraryButton, false);
    loadPlaylistLabel.setJustificationType(Justification::bottomLeft);
    loadPlaylistLabel.setFont(Font(11.0f, Font::bold));

    // Set up cross-fade mixer
    addAndMakeVisible(crossFadeComponent);
    crossFadeComponent.addListener(this);
    crossFadeComponent.setRange(0.0, 1.0);
    crossFadeComponent.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    // Enable user to adjust the minimum and maximum values of the cross-fade mixer
    crossFadeComponent.setSliderStyle(Slider::SliderStyle::ThreeValueHorizontal);
    crossFadeComponent.setMaxValue(1.0, dontSendNotification);

    // Add label to emphasize cross-fade component
    addAndMakeVisible(crossFadeLabel);
    crossFadeLabel.setJustificationType(Justification::centred);
    crossFadeLabel.setText("Crossfade", dontSendNotification);
    crossFadeLabel.attachToComponent(&crossFadeComponent, false);
    crossFadeLabel.setFont(Font(15.0f, Font::bold));

    // Register JUCE audio formats
    formatManager.registerBasicFormats();
}

/**
 * Destructor that cleans up the object and its members upon object destruction
 *
 * @param                         None
 *
 * @return                        None
 */
MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

/**
 * Moves the audio sources to their prepared states before fetching blocks of audio data
 *
 * @param samplesPerBlockExpected     Number of samples that the source will be expected to supply each time a subsequent block of data is fetched
 * @param sampleRate                  Number of sound samples taken per second
 *
 * @return                            None
 */
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Move audio players into prepared state
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Move mixer into prepared state
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Add input audio sources to be mixed
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

/**
 * Callback called repeatedly to fetch subsequent blocks of audio data after audio sources are prepared
 *
 * @param bufferToFill                Buffer that must be filed with audio data
 *
 * @return                            None
 */
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

/**
 * Allow the audio sources to release anything they no longer need after playback has stopped
 *
 * @param                             None
 *
 * @return                            None
 */
void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being restarted due to a setting change
    player1.releaseResources();
    player2.releaseResources();

    mixerSource.releaseResources();
}

/**
 * Redraw a region of a component due to a change in the screen
 *
 * @param                         Graphics context for drawing a component or image
 *
 * @return                        None
 */
void MainComponent::paint(Graphics& g)
{
    g.fillAll(Colour(18, 6, 46));
}

/**
 * Determine what should happen to the component when resized
 *
 * @param                         None
 *
 * @return                        None
 */
void MainComponent::resized()
{
    deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight() * 5.9 / 10);
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight() * 5.9 / 10);
    crossFadeComponent.setBounds(15, getHeight() * 6.43 / 10, getWidth() - 30, getHeight() * 0.3 / 10);
    searchInput.setBounds(5, getHeight() * 7.07 / 10, getWidth() / 4, getHeight() * .4 / 10);
    importTracksButton.setBounds(10 + getWidth() / 4, getHeight() * 7.07 / 10, getWidth() / 4.2, getHeight() * .4 / 10);
    exportLibraryButton.setBounds(15 + getWidth() / 4 + getWidth() / 4.2, getHeight() * 7.07 / 10, getWidth() / 4.2, getHeight() * .4 / 10);
    importLibraryButton.setBounds(20 + getWidth() / 4 + getWidth() * 2 / 4.2, getHeight() * 7.07 / 10, getWidth() / 4.2, getHeight() * .4 / 10);
    playlistComponent.Component::setBounds(0, getHeight() * 7.6 / 10, getWidth(), getHeight() * 2.9 / 10);
}

/**
 * Called when a slider's value is changed due to dragging or typing in its text entry box
 *
 * @param                         Slider control
 *
 * @return                        None
 */
void MainComponent::sliderValueChanged(Slider* slider)
{
    // Perform linear cross fade
    if (slider == &crossFadeComponent)
    {
        // Reduce gain of one audio player by a relative amount and increase gain of another player proportionally
        player1.setGain(1 - slider->getValue());
        player2.setGain(slider->getValue());
    }
}

/**
 * Called when the 'Import Track', 'Export Library', or 'Import Library' buttons are clicked
 *
 * @param                         Button base class
 *
 * @return                        None
 */
void MainComponent::buttonClicked(Button* button)
{
    if (button == &importTracksButton)
    {
        FileChooser multipleFileChooser{ "Select Files" };

        // Enable user to select one or more tracks to import to the library
        if (multipleFileChooser.browseForMultipleFilesToOpen())
        {
            // Valid set of files selected
            Array<File> fileArray = multipleFileChooser.getResults();

            for (auto file : fileArray)
            {
                // Update XML tree and playlist for each file
                playlistComponent.processFile(file, -1);
            }
        }
    }
    else if (button == &exportLibraryButton)
    {
        playlistComponent.exportLibrary();
    }
    else if (button == &importLibraryButton)
    {
        playlistComponent.importLibrary();
    }
}