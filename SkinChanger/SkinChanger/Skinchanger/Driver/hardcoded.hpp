/*
 * hardcoded.hpp
 *
 *  Created on: Feb 3, 2018
 *      Author: nullifiedcat
 */

#pragma once

#include <cstdint>

namespace offsets
{

constexpr uintptr_t entity_list  = 0x2114b60;
constexpr uintptr_t client_state = 0x9459a0;
constexpr uintptr_t local_player = 0x1A8;
}

namespace netvars
{

constexpr uintptr_t m_iItemDefinitionIndex = 0x928;
constexpr uintptr_t m_AttributeList        = 0x9ac;
constexpr uintptr_t m_hMyWeapons           = 0xcdc;
constexpr uintptr_t m_hActiveWeapon        = 0xd9c;
}
