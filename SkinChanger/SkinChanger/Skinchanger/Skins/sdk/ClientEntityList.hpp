/*
 * entitylist.hpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "memory.hpp"

namespace sdk
{

class Entity;

class CEntInfo
{
public:
    memory::remote_pointer<Entity> m_pEntity;
    int m_SerialNumber;
    memory::remote_pointer<CEntInfo> m_pPrev;
    memory::remote_pointer<CEntInfo> m_pNext;
};

// highest entity index: 0x10038

struct ClientEntityList
{
    char padding_01[0x4];
    CEntInfo entities[4096]; // 0x10000
    char padding_00[0x28];   // 0x10038
    int m_iNumServerEnts;
    int m_iMaxServerEnts;
    int m_iNumClientNonNetworkable;
    int m_iMaxUsedServerIndex;

    memory::remote_pointer<Entity> get(int index);
};
}
