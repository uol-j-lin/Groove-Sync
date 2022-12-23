/*
  ==============================================================================

    PlaylistComponent.h
    Created: 26 Jul 2022 1:47:04pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>

using namespace juce;

class PlaylistComponent : public juce::Component,
    public TableListBoxModel,
    public Button::Listener,
    public AudioFormatManager,
    public Label,
    public DragAndDropContainer
{
public:
    /**
     * Constructor that initializes the library component
     *
     * Restore library tracks from last session and set up library headers
     *
     * @param searchInput             Input entered into search box
     *
     * @return                        None
     */
    PlaylistComponent(Label* searchInput);

    /**
     * Destructor that cleans up after object is deallocated
     *
     * @param                         None
     *
     * @return                        None
     */
    ~PlaylistComponent() override;

    /**
     * Redraw a region of a component due to a change in the screen
     *
     * @param                         Graphics context for drawing a component or image
     *
     * @return                        None
     */
    void paint(juce::Graphics&) override;

    /**
     * Determine what should happen to the component when resized
     *
     * @param                         None
     *
     * @return                        None
     */
    void resized() override;

    /** Structure containing meta data about a track*/
    struct trackMetaData
    {
        // Unique identifier
        std::string customId;

        // Track title
        std::string title;

        // Length of track
        std::string length;

        // Format to be read by an AudioFormatReader instance
        std::string format;

        // Absolute path of track in file system
        std::string absolutePath;
    };

    /**
     * Determine the length of the track
     *
     * @param audioFile               Audio track file
     *
     * @return                        None
     */
    double getSongLength(File audioFile);

    /**
     * Convert track length into human readable form
     *
     * @param                         None
     *
     * @return                        Length of song in a user-friendly format
     */
    std::string formatSongLength(double songLength);

    /**
     * Retrieve the absolute path of the track that is selected in the library
     *
     * @param                         None
     *
     * @return                        Absolute path of selected track
     */
    std::string getSelectedPath();

    /**
    * Retrieve the file name representing a track that is selected in the library
    *
    * @param                         None
    *
    * @return                        Name of selected track
    */
    std::string getSelectedTrackName();

    /**
    * Retrieve the track that is selected in the library
    *
    * @param                         None
    *
    * @return                        Track selected in library 
    */
    trackMetaData getSelectedTrack();

    /**
     * Process user input in the search box
     *
     * @param inputText               Search term entered by the user
     *
     * @return                        None
     */
    void setSearchResults(std::string inputText);

    /**
     * Returns the number of rows in the unfiltered table
     *
     * @param                         None
     *
     * @return                        Number of rows in table
     */
    int getNumRows() override;

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
    void paintRowBackground(Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override;

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
    void paintCell(Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    /**
     * Used to create or update a custom component to go in a cell
     * 
     * @param rowNumber               Row background to draw
     * @param columnId                Unique ID to identify column
     * @param rowIsSelected           True if current row is selected, false otherwise
     *
     * @return                        None
     */
    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool isRowSelected,
        Component* existingComponentToUpdate) override;

    /**
     * Called when a button is clicked
     *
     * @param                         Button base class
     *
     * @return                        None
     */
    void buttonClicked(Button* button) override;

    /**
    * Process a file by updating XML tree and internal track playlist
    *
    * @param                         File to be processed
    * @param                         Id of button component
    *
    * @return                        None
    */
    void processFile(File file, int id);

    /**
    * Exports library as XML file into user directory structure
    *
    * @param                         None
    *
    * @return                        None
    */
    void exportLibrary();

    /**
    * Import an XML track library from a user directory 
    *
    * @param                         None
    *
    * @return                        None
    */
    void importLibrary();

    /**
    * Restore a saved XML track library 
    *
    * @param                         XML file containing track meta data
    *
    * @return                        None
    */
    void restoreLibrary(File libraryToBeRestored);

    /**
    * Allow rows from the playlist component to be dragged-and-dropped
    *
    * @param rowsToDescribe          Row to describe to be inputted into a DragAndDropTarget
    *
    * @return                        Absolute file path
    */
    var getDragSourceDescription(const SparseSet< int >& rowsToDescribe) override;

    /**
    * This callback is made when the table's sort order is changed
    *
    * @param newSortColumnId         Column id of selected header
    * @param isForwards              Determines if the tracks should be sorted based on ascending or descending column values
    *
    * @return                        None
    */
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;

    /**
    * Getter method that returns the meta data attribute name associated with a column ID
    *
    * @param columnId                Unique identifier of library column
    *
    * @return                        Attribute name
    */
    String getAttributeNameForColumnId(int columnId);

private:
    TableListBox tableComponent;

    std::vector<trackMetaData> metaData;
    std::vector<trackMetaData> searchResultData;

    std::unique_ptr<XmlElement> playlistLibrary;

    int uniqueTrackID;

    Label* userSearchInput;

    AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
