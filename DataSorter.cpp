/*
  ==============================================================================

    DataSorter.cpp
    Created: 18 Jul 2022 9:36:05pm
    Author:  Jonathan

  ==============================================================================
*/

#include "DataSorter.h"

/**
 * Constructor that initializes data members, determining ascending or descending sort order and track meta data attribute to sort by
 *
 * @param _player                 Audio player
 * @param formatManagerToUse      Format manager that records available formats
 *
 * @return                        None
 */
DataSorter::DataSorter(String& attributeToSortBy, bool forwards) : attributeToSort(attributeToSortBy), direction(forwards ? 1 : -1)
{
}

/**
 * Sort two XML elements based on a sorting attribute comparator
 *
 * @param first                   XML element representing an audio track
 * @param second                  XML element representing an audio track
 *
 * @return                        Value of < 0 if first comes before second, 0 is two objects are equivalent, > 0 if the second comes before the first
 */
int DataSorter::compareElements(XmlElement* first, XmlElement* second) const
{
    // Sort based on column header attribute
    auto result = first->getStringAttribute(attributeToSort).compareNatural(second->getStringAttribute(attributeToSort));

    if (result == 0)
    {
        result = first->getStringAttribute("customId").compareNatural(second->getStringAttribute("customId"));
    }

    return direction * result;
}
