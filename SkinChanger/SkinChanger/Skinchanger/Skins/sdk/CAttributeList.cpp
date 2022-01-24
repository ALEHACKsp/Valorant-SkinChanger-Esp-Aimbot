/*
 * CAttributeList.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: nullifiedcat
 */

#include "sdk/CAttributeList.hpp"

namespace sdk
{

uint32_t CAttributeList::get(uint16_t index) const
{
    for (int i = 0; i < attributes.m_Size; i++)
    {
        if (attributes.m_pElements[i].index == index)
        {
            return attributes.m_pElements[i].value;
        }
    }
    return 0.0f;
}

bool CAttributeList::exists(uint16_t index) const
{
    for (int i = 0; i < attributes.m_Size; i++)
    {
        if (attributes.m_pElements[i].index == index)
        {
            return true;
        }
    }
    return false;
}

void CAttributeList::set(uint16_t index, uint32_t value)
{
    bool set = false;
    for (int i = 0; i < attributes.m_Size; i++)
    {
        CAttribute attr = attributes.m_pElements[i];
        if (attr.index == index)
        {
            set        = true;
            attr.value = value;
            attributes.m_pElements.point_to_element(i).write(attr);
        }
    }
    if (not set)
    {
        CAttribute attr;
        attr.index = index;
        attr.value = value;
        attributes.m_pElements.point_to_element(attributes.m_Size).write(attr);
        attributes.m_Size++;
    }
}
}
