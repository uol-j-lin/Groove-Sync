/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 26 Jul 2022 1:47:04pm
    Author:  Jonathan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "DataSorter.h"

/**
 * Constructor that initializes the library component
 *
 * Restore library tracks from last session and set up library headers
 *
 * @param searchInput             Input entered into search box
 *
 * @return                        None
 */
PlaylistComponent::PlaylistComponent(Label* searchInput) : userSearchInput(searchInput), uniqueTrackID(1)
{
    // Set up column headers
    tableComponent.getHeader().addColumn("#", 1, 20);
    tableComponent.getHeader().addColumn("Track Title", 2, 176);
    tableComponent.getHeader().addColumn("Duration", 3, 176);
    tableComponent.getHeader().addColumn("Audio Format", 4, 176);
    tableComponent.getHeader().addColumn("Load Audio", 5, 176);
    tableComponent.getHeader().addColumn("Delete Audio", 6, 176);

    // Set model for table component
    tableComponent.setModel(this);

    Component::addAndMakeVisible(tableComponent);

    if (playlistLibrary == nullptr)
    {
        playlistLibrary.reset(new XmlElement("TrackMetaData"));
    }

    if (getNumRows() == 0)
    {
        restoreLibrary(File{ "C:/Users/Admin/Downloads/juce-6.1.6-windows/JUCE/modules/NewProject/Source/playlist.xml" });
    }
}

/**
 * Destructor that cleans up after object is deallocated
 *
 * @param                         None
 *
 * @return                        None
 */
PlaylistComponent::~PlaylistComponent()
{
}

/**
 * Redraw a region of a component due to a change in the screen
 *
 * @param                         Graphics context for drawing a component or image
 *
 * @return                        None
 */
void PlaylistComponent::paint(juce::Graphics& g)
{
    // Paint the background
    g.fillAll(Colour(8, 14, 46));

    g.setColour(Colours::lightblue);

    // Draw an outline around the component
    g.drawRect(Component::getLocalBounds(), 2);
}

/**
 * Determine what should happen to the component when resized
 *
 * @param                         None
 *
 * @return                        None
 */
void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, Component::getWidth(), Component::getHeight());
}

/**
 * Determine the length of the track
 *
 * @param inputText               Search term entered by the user
 *
 * @return                        None
 */
double PlaylistComponent::getSongLength(File audioFile)
{
    double trackLengthInSeconds = 0;
    formatManager.registerBasicFormats();
    URL audioURL = URL{ audioFile };

    // Determine song length based on total samples and sample rate
    AudioFormatReader* audioFormatReader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (audioFormatReader != nullptr)
    {
        trackLengthInSeconds = audioFormatReader->lengthInSamples / audioFormatReader->sampleRate;
        delete audioFormatReader;
    }
    return trackLengthInSeconds;
}

/**
 * Convert track length into human readable form
 *
 * @param                         None
 *
 * @return                        Length of song in a user-friendly format
 */
std::string PlaylistComponent::formatSongLength(double songLength)
{
    int numMinutes = 0;
    std::string numMinutesString = "", numSecondsString = "";

    // Determine number of minutes audio track is composed of
    while ((double)songLength - 60 > 0)
    {
        numMinutes++;
        songLength -= 60;
    }

    // Convert audio track in minutes to string representation
    numMinutesString = std::to_string(numMinutes);

    // Pad second representation with leading zeros to maintain consistent time structure
    numSecondsString = (int)songLength < 10 ? "0" + std::to_string((int)songLength) : std::to_string((int)songLength);

    std::string trackLengthInSeconds = "";

    // Create human readable representation with minutes and seconds
    trackLengthInSeconds = numMinutesString + ":" + numSecondsString;

    return trackLengthInSeconds;
}

/**
 * Retrieve the absolute path of the track that is selected in the library
 *
 * @param                         None
 *
 * @return                        Absolute path of selected track
 */
std::string PlaylistComponent::getSelectedPath()
{
    return tableComponent.getNumSelectedRows() != 0 ? metaData[tableComponent.getSelectedRow()].absolutePath : "";
}

/**
 * Retrieve the file name representing a track that is selected in the library
 *
 * @param                         None
 *
 * @return                        Name of selected track
 */
std::string PlaylistComponent::getSelectedTrackName()
{
    return tableComponent.getNumSelectedRows() != 0 ? metaData[tableComponent.getSelectedRow()].title : "";
}

/**
 * Retrieve the track that is selected in the library
 *
 * @param                         None
 *
 * @return                        Track selected in library
 */
PlaylistComponent::trackMetaData PlaylistComponent::getSelectedTrack()
{
    return tableComponent.getNumSelectedRows() != 0 ? metaData[tableComponent.getSelectedRow()] : trackMetaData();
}

/**
 * Process user input in the search box
 *
 * @param inputText               Search term entered by the user
 *
 * @return                        None
 */
void PlaylistComponent::setSearchResults(std::string inputText)
{
    // Reset the search results
    searchResultData.clear();
    int numRows = tableComponent.getNumRows();
    for (int i = 0; i < numRows; ++i)
    {
        // Determine if a track meets the search requirements
        if (metaData[i].title.find(inputText) != std::string::npos)
        {
            // Store search result
            searchResultData.push_back(metaData[i]);
        }
    }

    // Update the UI
    tableComponent.updateContent();
    Component::repaint();
}

/**
 * Returns the number of rows in the unfiltered table
 *
 * @param                         None
 *
 * @return                        Number of rows in table
 */
int PlaylistComponent::getNumRows()
{
    return metaData.size();
}

/**
 * Draw the background behind one of the rows in the table
 *
 * @param rowNumber               Row background to draw
 * @param width                   Width of background
 * @param height                  Height of background
 * @param rowIsSelected           True if current row is selected, false otherwise
 *
 * @return                        None
 */
void PlaylistComponent::paintRowBackground(Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::cornflowerblue);
    }
    else
    {
        g.fillAll(Colour(93, 118, 147));
    }
}

/**
 * Draw one of the cells in the table
 *
 * @param rowNumber               Row background to draw
 * @param columnId                Unique ID to identify column
 * @param width                   Width of background
 * @param height                  Height of background
 * @param rowIsSelected           True if current row is selected, false otherwise
 *
 * @return                        None
 */
void PlaylistComponent::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    if (userSearchInput->getText() == "")
    {
        if (rowNumber < getNumRows())
        {
            if (columnId == 1)
            {
                g.drawText(std::to_string(rowNumber + 1),
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
            if (columnId == 2)
            {
                g.drawText(metaData[rowNumber].title,
                    1, 0,
                    width - 4, height,
                    Justification::left,
                    true);
            }
            if (columnId == 3)
            {
                g.drawText(metaData[rowNumber].length,
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
            if (columnId == 4)
            {
                g.drawText(metaData[rowNumber].format,
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
        }
    }
    else
    {
        int numRowsSearch = searchResultData.size();
        if (rowNumber < numRowsSearch)
        {
            if (columnId == 1)
            {
                g.drawText(std::to_string(rowNumber + 1),
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
            if (columnId == 2)
            {
                g.drawText(searchResultData[rowNumber].title,
                    1, 0,
                    width - 4, height,
                    Justification::left,
                    true);
            }
            if (columnId == 3)
            {
                g.drawText(searchResultData[rowNumber].length,
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
            if (columnId == 4)
            {
                g.drawText(searchResultData[rowNumber].format,
                    1, 0,
                    width - 4, height,
                    Justification::centred,
                    true);
            }
        }
    }
}

/**
 * Used to create or update a custom component to go in a cell
 *
 * @param rowNumber               Row background to draw
 * @param columnId                Unique ID to identify column
 * @param rowIsSelected           True if current row is selected, false otherwise
 *
 * @return                        None
 */
Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{
    if (columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Import Track" };
            String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    if (columnId == 6)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Delete Track" };
            btn->setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
            String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

/**
 * Called when a button is clicked
 *
 * @param                         Button base class
 *
 * @return                        None
 */
void PlaylistComponent::buttonClicked(Button* button)
{
    // Determine the row number of the button
    int id = std::stoi(button->getComponentID().toStdString());

    if (button->getButtonText() == "Import Track")
    {
        // Valid file selected
        FileChooser fileChooser{ "Select a File" };
        if (fileChooser.browseForFileToOpen())
        {
            File fileSelected = fileChooser.getResult();

            // Update XML tree and playlist
            processFile(fileSelected, id);
        }
    }
    else if (button->getButtonText() == "Delete Track")
    {
        bool beginDecrement;
        auto decrementIterator = nullptr;

        // Delete the track from the unfiltered vector of tracks
        for (auto iterator = metaData.begin(); iterator != metaData.end(); ++iterator)
        {
            if (std::to_string(id) == String(iterator->customId).toStdString())
            {
                metaData.erase(iterator);
                break;
            }
        }

        // Decrement the unique identifier for each struct after the deleted element to prevent ID to row number discrepancies
        for (auto decrementIterator = metaData.begin(); decrementIterator != metaData.end(); ++decrementIterator)
        {
            if (std::to_string(id) < String(decrementIterator->customId).toStdString())
            {
                decrementIterator->customId = std::to_string(std::stoi(decrementIterator->customId) - 1);
            }
        }

        bool beginCustomIdDecrement = false;
        XmlElement* childToRemove = nullptr;

        // Delete the track from the XML tree
        for (auto* element : playlistLibrary->getChildIterator())
        {
            if (beginCustomIdDecrement)
            {
                element->setAttribute("customId", String(std::stoi(element->getStringAttribute("customId", "invalidCustomId").toStdString()) - 1));
            }
            if (element->getStringAttribute("customId").toStdString() == std::to_string(id))
            {
                childToRemove = element;
                beginCustomIdDecrement = true;
            }
        }
        playlistLibrary->removeChildElement(childToRemove, true);

        // Update playlist stored in XML file
        playlistLibrary->writeTo(File{ "C:/Users/Admin/Downloads/juce-6.1.6-windows/JUCE/modules/NewProject/Source/playlist.xml" });

        // Update UI to remove deleted track
        tableComponent.updateContent();
        Component::repaint();
    }
}

/**
 * Process a file by updating XML tree and internal track playlist
 *
 * @param                         File to be processed
 * @param                         Id of button component
 *
 * @return                        None
 */
void PlaylistComponent::processFile(File file, int id)
{
    // Extract meta data from file
    String fileName = String{ file.getFileNameWithoutExtension() };
    String fileFormat = String{ file.getFileExtension() };
    String absolutePath = String{ file.getFullPathName() };

    // Create track object with metadata
    trackMetaData trackMetaData;
    trackMetaData.customId = std::to_string(id);
    trackMetaData.title = fileName.toStdString();
    trackMetaData.length = formatSongLength(getSongLength(file));
    trackMetaData.format = fileFormat.toStdString();
    trackMetaData.absolutePath = absolutePath.toStdString();

    if (id != -1)
    {
        metaData[id] = trackMetaData;
    }
    else
    {
        metaData.push_back(trackMetaData);
    }

    // Store information into XML element for library persistence
    XmlElement* track = new XmlElement{ "Track" };

    track->setAttribute("customId", id);
    track->setAttribute("title", fileName.toStdString());
    track->setAttribute("length", formatSongLength(getSongLength(file)));
    track->setAttribute("format", fileFormat.toStdString());
    track->setAttribute("absolutePath", absolutePath.toStdString());

    auto* existingElement = playlistLibrary->getChildByAttribute("customId", std::to_string(id));

    if (id == -1 || !existingElement)
    {
        // Create new track
        playlistLibrary->addChildElement(track);
    }
    else
    {
        // Update existing track
        playlistLibrary->replaceChildElement(existingElement, track);
    }

    // Write document to a file as UTF-8
    playlistLibrary->writeTo(File{ "C:/Users/Admin/Downloads/juce-6.1.6-windows/JUCE/modules/NewProject/Source/playlist.xml" });

    // Update UI
    tableComponent.updateContent();
    Component::repaint();
}

/**
 * Exports library as XML file into user directory structure
 *
 * @param                         None
 *
 * @return                        None
 */
void PlaylistComponent::exportLibrary()
{
    FileChooser libraryName{ "Select Library Name" };

    if (libraryName.browseForFileToSave(false))
    {
        // Write XmlDocument playlist to a file
        File fileToSave = libraryName.getResult();
        playlistLibrary->writeTo(fileToSave);
    }
}

/**
 * Import an XML track library from a user directory
 *
 * @param                         None
 *
 * @return                        None
 */
void PlaylistComponent::importLibrary()
{
    FileChooser selectedLibrary{ "Selected Existing Library" };

    // Restore selected library
    if (selectedLibrary.browseForFileToOpen())
    {
        restoreLibrary(selectedLibrary.getResult());
    }
}

/**
 * Restore a saved XML track library
 *
 * @param                         XML file containing track meta data
 *
 * @return                        None
 */
void PlaylistComponent::restoreLibrary(File libraryToBeRestored)
{
    // Restore library tracks stored in an XML file from previous session
    XmlDocument playlistXMLDocument{ libraryToBeRestored };

    playlistLibrary = playlistXMLDocument.getDocumentElement();

    // Clear internal representation of audio meta data
    metaData.clear();

    if (playlistLibrary->hasTagName("TrackMetaData"))
    {
        int elementCustomIndex = 0;
        for (XmlElement* element : playlistLibrary->getChildWithTagNameIterator("Track"))
        {
            // Iterate its sub-elements for tracks
            trackMetaData restoreChildTrack;

            // Found track, use its attributes
            String customTrackId = std::to_string(elementCustomIndex);
            element->setAttribute("customId", std::to_string(elementCustomIndex));
            elementCustomIndex++;

            // Get attributes of each track
            String trackTitle = element->getStringAttribute("title");
            String length = element->getStringAttribute("length");
            String format = element->getStringAttribute("format");
            String absolutePath = element->getStringAttribute("absolutePath");

            // Create a track record from the  XML element to store internally
            restoreChildTrack.customId = customTrackId.toStdString();
            restoreChildTrack.title = trackTitle.toStdString();
            restoreChildTrack.length = length.toStdString();
            restoreChildTrack.format = format.toStdString();
            restoreChildTrack.absolutePath = absolutePath.toStdString();

            // Store track record internally
            metaData.push_back(restoreChildTrack);

            // Update XML playlist file
            playlistLibrary->writeTo(File{ "C:/Users/Admin/Downloads/juce-6.1.6-windows/JUCE/modules/NewProject/Source/playlist.xml" });
        }
    }

    // Update UI
    tableComponent.updateContent();
    Component::repaint();
}

/**
 * Allow rows from the playlist component to be dragged-and-dropped
 *
 * @param rowsToDescribe          Row to describe to be inputted into a DragAndDropTarget
 *
 * @return                        Absolute file path
 */
var PlaylistComponent::getDragSourceDescription(const SparseSet<int>& rowsToDescribe)
{
    if (getNumRows() > 0)
    {
        // Provide the absolute path of the audio track being dragged to one of two decks for loading
        return metaData[tableComponent.getSelectedRow()].absolutePath;
    }
}

/**
 * This callback is made when the table's sort order is changed
 *
 * @param newSortColumnId         Column id of selected header
 * @param isForwards              Determines if the tracks should be sorted based on ascending or descending column values
 *
 * @return                        None
 */
void PlaylistComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    if (newSortColumnId != 0)
    {
        // Get the attribute name mapped to a column ID
        DataSorter dataSorter(getAttributeNameForColumnId(newSortColumnId), isForwards);

        // Sort based on column header attribute
        playlistLibrary->sortChildElements(dataSorter);

        metaData.clear();

        for (XmlElement* element : playlistLibrary->getChildWithTagNameIterator("Track"))
        {
            // Iterate its sub-elements for tracks
            trackMetaData restoreChildTrack;

            // Found track, use its attributes
            String customTrackId = element->getStringAttribute("customId");

            // Get attributes of each track
            String trackTitle = element->getStringAttribute("title");
            String length = element->getStringAttribute("length");
            String format = element->getStringAttribute("format");
            String absolutePath = element->getStringAttribute("absolutePath");

            // Create a track record from the XML element to store internally
            restoreChildTrack.customId = customTrackId.toStdString();
            restoreChildTrack.title = trackTitle.toStdString();
            restoreChildTrack.length = length.toStdString();
            restoreChildTrack.format = format.toStdString();
            restoreChildTrack.absolutePath = absolutePath.toStdString();

            // Add sorted track to playlist
            metaData.push_back(restoreChildTrack);
        }

        // Update UI
        tableComponent.updateContent();
        Component::repaint();
    }
}

/**
 * Getter method that returns the meta data attribute name associated with a column ID
 *
 * @param columnId                Unique identifier of library column
 *
 * @return                        Attribute name
 */
String PlaylistComponent::getAttributeNameForColumnId(int columnId)
{
    std::string columnHeader = tableComponent.getHeader().getColumnName(columnId).toStdString();

    String columnAttribute;

    // Map each column header to a data member for each track meta data structure
    if (columnHeader == "Track Title")
    {
        columnAttribute = "title";
    }
    else if (columnHeader == "Duration")
    {
        columnAttribute = "length";
    }
    else if (columnHeader == "Audio Format")
    {
        columnAttribute = "format";
    }
    else
    {
        columnAttribute = "columnId";
    }
    return columnAttribute;
}