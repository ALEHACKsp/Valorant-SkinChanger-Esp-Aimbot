/*
 * CAttributeList.hpp
 *
 *  Created on: Feb 3, 2018
 *      Author: nullifiedcat
 */

#pragma once

#include "CUtlVector.hpp"

namespace sdk
{

struct CAttribute
{
    uint32_t pad00{ 0 };
    uint16_t index{ 0 };
    uint32_t value{ 0 };
    uint32_t pad01{ 0 };
};

class CAttributeList
{
public:
    CAttributeList() = delete;

public:
    uint32_t get(uint16_t index) const;
    bool exists(uint16_t index) const;
    void set(uint16_t index, uint32_t value);
    // Not implemented
    void remove(uint16_t index) = delete;

public:
    uint32_t pad00;
    CUtlVector<CAttribute> attributes;
};
}
