/*
  ==============================================================================

	DeckGUI.cpp
	Created: 26 Jul 2022 3:54:28pm
	Author:  Jonathan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"
#include "BinaryData.h"

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

DeckGUI::DeckGUI(DJAudioPlayer* _player,
	AudioFormatManager& formatManagerToUse,
	AudioThumbnailCache& cacheToUse,
	PlaylistComponent* _playlistComponent)
	: player(_player),
	waveformDisplay(formatManagerToUse, cacheToUse),
	playlistComponent(_playlistComponent),
	rotationAngle(0.0)
{
	// Load transport and vinyl graphics from in-memory image file or cache
	playButtonGraphic = ImageCache::getFromMemory(BinaryData::playButton_png, BinaryData::playButton_pngSize);
	pauseButtonGraphic = ImageCache::getFromMemory(BinaryData::pauseButton_png, BinaryData::pauseButton_pngSize);
	stopButtonGraphic = ImageCache::getFromMemory(BinaryData::stopButton_png, BinaryData::stopButton_pngSize);
	rewindButtonGraphic = ImageCache::getFromMemory(BinaryData::rewindButton_png, BinaryData::rewindButton_pngSize);
	fastForwardButtonGraphic = ImageCache::getFromMemory(BinaryData::fastForwardButton_png, BinaryData::fastForwardButton_pngSize);
	loopButtonGraphic = ImageCache::getFromMemory(BinaryData::loopButton_png, BinaryData::loopButton_pngSize);
	vinylGraphic = ImageCache::getFromMemory(BinaryData::vinyl_png, BinaryData::vinyl_pngSize);

	// Load hot cue player graphics from in-memory image file or cache
	firstCuePlayer = ImageCache::getFromMemory(BinaryData::numberOne_png, BinaryData::numberOne_pngSize);
	secondCuePlayer = ImageCache::getFromMemory(BinaryData::numberTwo_png, BinaryData::numberTwo_pngSize);
	thirdCuePlayer = ImageCache::getFromMemory(BinaryData::numberThree_png, BinaryData::numberThree_pngSize);

	// Add transport and vinyl graphics to the deck user interface
	addAndMakeVisible(playImageButton);
	addAndMakeVisible(pauseImageButton);
	addAndMakeVisible(stopImageButton);
	addAndMakeVisible(rewindImageButton);
	addAndMakeVisible(fastForwardImageButton);
	addAndMakeVisible(loopImageButton);

	// Add hot cues to the interface as a sub-component
	addAndMakeVisible(playFirstCueButton);
	addAndMakeVisible(playSecondCueButton);
	addAndMakeVisible(playThirdCueButton);

	// Add playlist queue to the deck user interface
	addAndMakeVisible(playlistQueue);

	// Set up properties for transport graphics to be drawn in for different states
	playImageButton.setImages(false, false, true, playButtonGraphic, 0.0f, {}, playButtonGraphic, 0.0f, {}, playButtonGraphic, 0.0f, {});
	pauseImageButton.setImages(false, false, true, pauseButtonGraphic, 0.0f, {}, pauseButtonGraphic, 0.0f, {}, pauseButtonGraphic, 0.0f, {});
	stopImageButton.setImages(false, false, true, stopButtonGraphic, 0.0f, {}, stopButtonGraphic, 0.0f, {}, stopButtonGraphic, 0.0f, {});
	rewindImageButton.setImages(false, false, true, rewindButtonGraphic, 0.0f, {}, rewindButtonGraphic, 0.0f, {}, rewindButtonGraphic, 0.0f, {});
	fastForwardImageButton.setImages(false, false, true, fastForwardButtonGraphic, 0.0f, {}, fastForwardButtonGraphic, 0.0f, {}, fastForwardButtonGraphic, 0.0f, {});
	loopImageButton.setImages(false, false, true, loopButtonGraphic, 0.0f, {}, loopButtonGraphic, 0.0f, {}, loopButtonGraphic, 0.0f, {});

	// Set up properties of hot cue player graphics to be drawn in for different states
	playFirstCueButton.setImages(false, false, true, firstCuePlayer, 0.0f, {}, firstCuePlayer, 0.0f, {}, firstCuePlayer, 0.0f, {});
	playSecondCueButton.setImages(false, false, true, secondCuePlayer, 0.0f, {}, secondCuePlayer, 0.0f, {}, secondCuePlayer, 0.0f, {});
	playThirdCueButton.setImages(false, false, true, thirdCuePlayer, 0.0f, {}, thirdCuePlayer, 0.0f, {}, thirdCuePlayer, 0.0f, {});

	// Add sub-components to the deck interface
	addAndMakeVisible(loadButton);
	addAndMakeVisible(queueTrackButton);
	addAndMakeVisible(volSlider);
	addAndMakeVisible(bandPassSlider);
	addAndMakeVisible(lowPassSlider);
	addAndMakeVisible(highPassSlider);
	addAndMakeVisible(speedSlider);
	addAndMakeVisible(waveformDisplay);
	addAndMakeVisible(firstCueMarker);
	addAndMakeVisible(secondCueMarker);
	addAndMakeVisible(thirdCueMarker);

	LookAndFeel::setDefaultLookAndFeel(&customDial);

	// Add labels above and to the left of waveform display to indicate a DJ deck
	addAndMakeVisible(DJAppLabel);
	DJAppLabel.setText("DJ Deck", dontSendNotification);
	DJAppLabel.setJustificationType(Justification::centredLeft);
	DJAppLabel.setFont(Font(16.0f, Font::bold));
	DJAppLabel.attachToComponent(&songTitleLabel, false);

	// Add a label below the 'DJ deck' label to indicate the audio track title
	addAndMakeVisible(songTitleLabel);
	songTitleLabel.attachToComponent(&waveformDisplay, false);
	songTitleLabel.setText("Audio Track Title", dontSendNotification);
	songTitleLabel.setFont(Font(13.0f));

	// Add labels above and to the right of waveform display for the audio length
	addAndMakeVisible(songLengthLabel);
	songLengthLabel.setText("Audio Length", dontSendNotification);
	songLengthLabel.attachToComponent(&songPositionLabel, false);
	songLengthLabel.setJustificationType(Justification::bottomRight);
	songLengthLabel.setFont(Font(11.0f));

	// Add labels above and to the right of waveform display for the audio position
	addAndMakeVisible(songPositionLabel);
	songPositionLabel.setText("Audio Position", dontSendNotification);
	songPositionLabel.setBorderSize(BorderSize<int>{5});
	songPositionLabel.attachToComponent(&waveformDisplay, false);
	songPositionLabel.setJustificationType(Justification::bottomRight);
	songPositionLabel.setFont(Font(11.0f));

	// Make sliders for frequency attention filters into rotary dials
	bandPassSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	lowPassSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	highPassSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	speedSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	// Make gain/volume sliders vertical
	volSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	volSlider.setTextBoxIsEditable(false);

	// Remove text boxes from dials
	bandPassSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 30);
	lowPassSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 30);
	highPassSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 30);
	speedSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 30);

	// Set text label information for volume vertical bar
	addAndMakeVisible(volLabel);
	volLabel.setText("Gain", dontSendNotification);
	volLabel.setJustificationType(Justification::centred);
	volLabel.attachToComponent(&volSlider, false);

	// Set text label information for band pass rotary dial
	addAndMakeVisible(bandPassLabel);
	bandPassLabel.setText("Band Pass", dontSendNotification);
	bandPassLabel.setJustificationType(Justification::centred);
	bandPassLabel.attachToComponent(&bandPassSlider, false);

	// Set text label information for low pass rotary dial
	addAndMakeVisible(lowPassLabel);
	lowPassLabel.setText("Low Pass", dontSendNotification);
	bandPassLabel.setJustificationType(Justification::centred);
	lowPassLabel.attachToComponent(&lowPassSlider, false);

	// Set text label information for high pass rotary dial
	addAndMakeVisible(highPassLabel);
	highPassLabel.setText("High Pass", dontSendNotification);
	bandPassLabel.setJustificationType(Justification::centred);
	highPassLabel.attachToComponent(&highPassSlider, false);

	// Set text label information for speed rotary dial
	addAndMakeVisible(speedLabel);
	speedLabel.setText("Speed", dontSendNotification);
	speedLabel.setJustificationType(Justification::centred);
	speedLabel.attachToComponent(&speedSlider, false);

	// Customize dial appearance
	getLookAndFeel().setColour(Slider::thumbColourId, Colours::aquamarine);
	getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, Colours::royalblue);
	getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Colours::ghostwhite);

	// Customize slider appearance
	getLookAndFeel().setColour(Slider::backgroundColourId, Colours::royalblue);
	getLookAndFeel().setColour(Slider::trackColourId, Colours::royalblue);

	// Customize load button appearance
	loadButton.setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);

	// Customize queue track button appearance
	queueTrackButton.setColour(TextButton::ColourIds::buttonColourId, Colour(22, 22, 22));

	// Set custom appearance for speed dial
	speedSlider.setLookAndFeel(&speedDialLookAndFeel);

	// Adjust sensitivity of dials so that users do not have to drag excessively in horizontal direction
	bandPassSlider.setMouseDragSensitivity(60);
	lowPassSlider.setMouseDragSensitivity(60);
	highPassSlider.setMouseDragSensitivity(60);
	speedSlider.setMouseDragSensitivity(60);

	// Register listeners to receive events when the state of the transport controls change
	playImageButton.addListener(this);
	pauseImageButton.addListener(this);
	stopImageButton.addListener(this);
	rewindImageButton.addListener(this);
	fastForwardImageButton.addListener(this);
	loopImageButton.addListener(this);

	// Register listeners to receive events when the state of sliders and buttons change
	loadButton.addListener(this);
	queueTrackButton.addListener(this);
	volSlider.addListener(this);
	speedSlider.addListener(this);
	bandPassSlider.addListener(this);
	lowPassSlider.addListener(this);
	highPassSlider.addListener(this);
	firstCueMarker.addListener(this);
	secondCueMarker.addListener(this);
	thirdCueMarker.addListener(this);
	playFirstCueButton.addListener(this);
	playSecondCueButton.addListener(this);
	playThirdCueButton.addListener(this);

	// Ensure mouse events in the waveform display trigger changes in the waveform color before and after playhead
	waveformDisplay.addChangeListener(this);

	// Set ranges and default values of sliders with appropriate units
	volSlider.setRange(0.0, 100.0, 0.1);
	volSlider.setTextValueSuffix(" %");
	volSlider.setValue(100.0);

	speedSlider.setValue(1.0);
	speedSlider.setRange(0.0, 5.0, 0.1);
	speedSlider.setTextValueSuffix(" x");

	bandPassSlider.setRange(20.0, 8000.0, 1);
	bandPassSlider.setValue(500.0);
	bandPassSlider.setTextValueSuffix(" Hz");

	lowPassSlider.setRange(20.0, 8000.0, 1);
	lowPassSlider.setValue(20000.0);
	lowPassSlider.setTextValueSuffix(" Hz");

	highPassSlider.setRange(20.0, 8000.0, 1);
	highPassSlider.setValue(20.0);
	highPassSlider.setTextValueSuffix(" Hz");

	// Enable double clicks to return to default dial value
	bandPassSlider.setDoubleClickReturnValue(true, 500.0);
	lowPassSlider.setDoubleClickReturnValue(true, 8000.0);
	highPassSlider.setDoubleClickReturnValue(true, 20.0);
	speedSlider.setDoubleClickReturnValue(true, 1.0);

	// Make repeated callbacks to set the relative position of the waveform at 90ms intervals
	startTimer(600);
}

/**
 * DeckGUI destructor
 *
 * @param                         None
 *
 * @return                        None
 */

DeckGUI::~DeckGUI()
{
	// Prevent timer callbacks from being triggered
	stopTimer();
}

/**
 * Redraw a region of a component due to a change in the screen
 *
 * @param                         Graphics context for drawing a component or image
 *
 * @return                        None
 */
void DeckGUI::paint(Graphics& g)
{
	// Paint the background
	g.fillAll(Colour(2, 24, 37));

	g.setColour(Colour(75, 86, 97));

	// Draw an outline around the component
	g.drawRect(Component::getLocalBounds(), 3);

	// Draw an outline around the vertical gain slider
	g.drawRect(Component::getWidth() * 35.85 / 42, Component::getHeight() / 13, Component::getWidth() / 8, Component::getHeight() / 13 * 3.8, 2);

	// Draw transport controls in a line below the waveform display and gain control
	g.drawImageWithin(playButtonGraphic, 25, 150, 35, 35, RectanglePlacement());
	g.drawImageWithin(pauseButtonGraphic, 90, 150, 35, 35, RectanglePlacement());
	g.drawImageWithin(stopButtonGraphic, 155, 150, 35, 35, RectanglePlacement());
	g.drawImageWithin(rewindButtonGraphic, 25, 195, 35, 35, RectanglePlacement());
	g.drawImageWithin(fastForwardButtonGraphic, 90, 195, 35, 35, RectanglePlacement());
	g.drawImageWithin(loopButtonGraphic, 155, 195, 35, 35, RectanglePlacement());

	// Draw play cue graphics in a vertical line to the right of the 'Cue' buttons
	g.drawImageWithin(firstCuePlayer, 405, 150, 40, 23, RectanglePlacement());
	g.drawImageWithin(secondCuePlayer, 405, 178, 40, 23, RectanglePlacement());
	g.drawImageWithin(thirdCuePlayer, 405, 206, 40, 23, RectanglePlacement());

	// Draw the vinyl to the right of the loaded audio track
	g.drawImageWithin(vinylGraphic, 72, 14, 18, 18, RectanglePlacement());
}

/**
 * Determine what should happen to the component when resized
 *
 * @param                         None
 *
 * @return                        None
 */
void DeckGUI::resized()
{
	const int border = 10;
	const int dialWidth = getWidth() / 4.4 - border;
	const int dialHeight = getHeight() / 6;
	const int imageButtonWidth = getWidth() / 6;
	const int imageButtonHeight = getHeight() / 6;
	double rowH = getHeight() / 13;

	waveformDisplay.setBounds(10, rowH * 2, getWidth() * 0.83 - 10, rowH * 2.8);
	playImageButton.setBounds(25, 150, 35, 35);
	pauseImageButton.setBounds(90, 150, 35, 35);
	stopImageButton.setBounds(155, 150, 35, 35);
	rewindImageButton.setBounds(25, 195, 35, 35);
	fastForwardImageButton.setBounds(90, 195, 35, 35);
	loopImageButton.setBounds(155, 195, 35, 35);
	playlistQueue.setBounds(225, 150, 130, 85);

	// Position the 'Cue' buttons and cue marker icons to the right of the playlist queue
	firstCueMarker.setBounds(365, 150, 40, 23);
	secondCueMarker.setBounds(365, 178, 40, 23);
	thirdCueMarker.setBounds(365, 206, 40, 23);
	playFirstCueButton.setBounds(405, 150, 40, 23);
	playSecondCueButton.setBounds(405, 178, 40, 23);
	playThirdCueButton.setBounds(405, 206, 40, 23);

	volSlider.setBounds(getWidth() * 35.85 / 42, rowH, getWidth() / 8, rowH * 3.8);
	bandPassSlider.setBounds(border, rowH * 8.8 + border, dialWidth, dialHeight);
	lowPassSlider.setBounds(getWidth() / 4 + border, rowH * 8.8 + border, dialWidth, dialHeight);
	highPassSlider.setBounds(getWidth() * 2 / 4 + border, rowH * 8.8 + border, dialWidth, dialHeight);
	speedSlider.setBounds(getWidth() * 3 / 4 + border, rowH * 8.8 + border, dialWidth, dialHeight);
	loadButton.setBounds(10, rowH * 11.6, getWidth() / 2 - 15, rowH * 1.2);
	queueTrackButton.setBounds(getWidth() / 2 + 4, rowH * 11.6, getWidth() / 2 - 15, rowH * 1.2);
}

/**
 * Called when a button is clicked
 *
 * @param                         Button base class
 *
 * @return                        None
 */
void DeckGUI::buttonClicked(Button* button)
{
	if (button == &playImageButton)
	{
		player->start();
	}
	if (button == &pauseImageButton)
	{
		player->stop();
	}
	if (button == &loadButton)
	{
		File selectedFile = playlistComponent->getSelectedPath();

		// Load URL into audio player
		player->loadURL(URL{ selectedFile });

		// Display waveform of audio file
		waveformDisplay.loadURL(URL{ selectedFile });

		// Update audio track title
		songTitleLabel.setText(selectedFile.getFileNameWithoutExtension(), dontSendNotification);

		// Update audio track length
		songLengthLabel.setText(playlistComponent->formatSongLength(playlistComponent->getSongLength(selectedFile)), dontSendNotification);
	}
	if (button == &queueTrackButton)
	{
		// Get selected track as a file
		std::string selectedPath = playlistComponent->getSelectedPath();

		// Get selected track name
		std::string selectedTrackName = playlistComponent->getSelectedTrackName();

		// Store meta data of track
		std::vector<std::string> selectedRowMetaData;
		selectedRowMetaData.push_back(selectedPath);
		selectedRowMetaData.push_back(selectedTrackName);

		// Queue track to be played
		playlistQueue.enqueueTrack(selectedRowMetaData);
	}
	if (button == &rewindImageButton)
	{
		// Rewind a few seconds
		player->movePositionBack();
	}
	if (button == &fastForwardImageButton)
	{
		// Skip forward a few seconds
		player->movePositionForward();
	}
	if (button == &stopImageButton)
	{
		// Restart and pause the track
		player->backToStart();
		player->stop();
	}
	if (button == &loopImageButton)
	{
		// Toggle looping
		player->toggleAudioLoop();
	}
	if (button == &firstCueMarker)
	{
		// Insert yellow cue marker at current relative position in audio track
		waveformDisplay.insertCueRelativePosition(1, player->getPositionRelative());
	}
	if (button == &secondCueMarker)
	{
		// Insert purple cue marker at current relative position in audio track
		waveformDisplay.insertCueRelativePosition(2, player->getPositionRelative());
	}
	if (button == &thirdCueMarker)
	{
		// Insert green cue marker at current relative position in audio track
		waveformDisplay.insertCueRelativePosition(3, player->getPositionRelative());
	}
	if (button == &playFirstCueButton)
	{
		// Play audio track from first cue marker
		waveformDisplay.playTrackFromCueMarker(1);
	}
	if (button == &playSecondCueButton)
	{
		// Play audio track from second cue marker
		waveformDisplay.playTrackFromCueMarker(2);
	}
	if (button == &playThirdCueButton)
	{
		// Play audio track from third cue marker
		waveformDisplay.playTrackFromCueMarker(3);
	}
}

/**
 * Called when a slider's value is changed due to dragging or typing in its text entry box
 *
 * @param                         Slider control
 *
 * @return                        None
 */
void DeckGUI::sliderValueChanged(Slider* slider)
{
	// Set gain as a percentage
	if (slider == &volSlider)
	{
		player->setGain(slider->getValue() / 100);
	}
	// Set speed
	if (slider == &speedSlider)
	{
		player->setSpeed(slider->getValue());
	}
	if (slider == &bandPassSlider)
	{
		player->setBandPassFrequency(slider->getValue());
	}
	if (slider == &lowPassSlider)
	{
		player->setLowPassFrequency(slider->getValue());
	}
	if (slider == &highPassSlider)
	{
		player->setHighPassFrequency(slider->getValue());
	}
}

/**
 * Callback to check whether this target is interested in the set of files being offered
 *
 * @param files                   The set of absolute pathnames of the files that the user is dragging
 *
 * @return                        True if the component wants to receive other callbacks of this type of object, false if no other callbacks will be made
 */
bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
	return true;
}

/**
 * Callback to indicate that the user has dropped the files onto this component
 *
 * @param files                   The set of absolute pathnames of the files that the user is dragging
 * @param x                       Mouse x position, relative to component
 * @param y                       Mouse y position, relative to component
 *
 * @return                        None
 */
void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
	if (files.size() == 1)
	{
		player->loadURL(URL{ File{files[0]} });
	}
}

/**
 * Callback routine that gets called periodically to display the playhead location and track position to the user.
 *
 * If the track has completed and the track is set to loop, the audio track is restarted.
 *
 * When the current track ends, it plays queued tracks in order if the track is not set to loop.
 *
 * @param                         None
 *
 * @return                        None
 */
void DeckGUI::timerCallback()
{
	// Update waveform display according to relative position
	double positionRelative = player->getPositionRelative();
	waveformDisplay.setPositionRelative(positionRelative);
	repaint();

	// Update the audio position indicator given a position change of at least a second
	/*
	if (player->getPositionRelative() >= -1e2 && songPositionLabel.getText().toStdString() != playlistComponent->formatSongLength(positionRelative * playlistComponent->getSongLength(File(playlistComponent->getSelectedTrack().absolutePath))))
	{
		// Update the audio position in human readable form
		songPositionLabel.setText(playlistComponent->formatSongLength(positionRelative * playlistComponent->getSongLength(File(playlistComponent->getSelectedTrack().absolutePath))), dontSendNotification);
	}
	*/

	// Determine if current track has ended and looping is not enabled
	if (player->finishedPlaying() && !player->isLooping())
	{
		// Determine if there are tracks in queue
		if (!playlistQueue.isEmpty())
		{
			// Get next track
			File nextTrack = playlistQueue.dequeueTrack();

			// Generate waveform and begin playing track
			player->loadURL(URL{ nextTrack });
			player->start();
		}
	}

	// Determine if the track has completed playing and if the track is set to loop
	if (player->finishedPlaying() && player->isLooping())
	{
		// Loop the audio track
		player->backToStart();
		player->start();
	}
}

/**
 * Receive callback due to changes in mouse behavior to update the position of the playhead
 *
 * @param source                  ChangeBroadcaster that triggered the callback
 *
 * @return                        None
 */
void DeckGUI::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &waveformDisplay)
	{
		player->setPositionRelative(waveformDisplay.getPositionRelative());
	}
}

/**
 * Callback to determine whether this target is interested in the type of object being dragged
 *
 * @param dragSourceDetails       Contains information about the source of the drag operation
 *
 * @return                        True if component wants to receive callbacks regarding this type of object, false otherwise
 */
bool DeckGUI::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
	return true;
}

/**
 * Callback to indicate that the user has dropped something onto this component
 *
 * @param dragSourceDetails       Contains information about the source of the drag operation
 *
 * @return                        None
 */
void DeckGUI::itemDropped(const SourceDetails& dragSourceDetails)
{
	// Convert absolute path of drag source into file
	File dragSourceFile = File{ dragSourceDetails.description };

	// Load URL into audio player
	player->loadURL(URL{ dragSourceFile });

	// Display waveform of audio file
	waveformDisplay.loadURL(URL{ dragSourceFile });

	// Update audio track title and length
	songTitleLabel.setText(dragSourceFile.getFileNameWithoutExtension(), dontSendNotification);
	songLengthLabel.setText(playlistComponent->formatSongLength(playlistComponent->getSongLength(dragSourceFile)), dontSendNotification);
}
