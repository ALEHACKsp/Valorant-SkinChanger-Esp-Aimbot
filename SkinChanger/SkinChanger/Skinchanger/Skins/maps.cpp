/*
 * maps.cpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#include "maps.hpp"

#include <sys/mman.h>

proc_map::proc_map(const std::string &string)
{
    size_t endpos;
    start           = std::stoull(string, &endpos, 16);
    std::string sub = string.substr(endpos + 1);
    end             = std::stoull(sub, &endpos, 16);
    sub             = sub.substr(endpos + 1);
    if (sub.at(0) == 'r')
        protection |= PROT_READ;
    if (sub.at(1) == 'w')
        protection |= PROT_WRITE;
    if (sub.at(2) == 'x')
        protection |= PROT_EXEC;
    sub    = sub.substr(5);
    offset = std::stoull(sub, &endpos, 16);
    // Skipping the device numbers
    sub = sub.substr(endpos + 7);
    // Skipping inode
    std::stoi(sub, &endpos, 10);
    sub = sub.substr(endpos + 1);
    for (size_t i = 0; i < sub.length(); i++)
    {
        if (sub.at(i) != ' ' && sub.at(i) != '\n')
        {
            path = sub.substr(i);
            break;
        }
    }
    if (path.length())
    {
        size_t name_begin = path.rfind('/');
        if (name_begin != std::string::npos)
        {
            file = path.substr(name_begin + 1);
        }
    }
}
