/*
 * maps.hpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <stdint.h>
#include <string>

struct proc_map
{
    proc_map(const std::string &string);

public:
    uintptr_t start{ 0 };
    uintptr_t end{ 0 };
    int protection{ 0 };
    uintptr_t offset{ 0 };
    std::string path{ "" };
    std::string file{ "" };
};
