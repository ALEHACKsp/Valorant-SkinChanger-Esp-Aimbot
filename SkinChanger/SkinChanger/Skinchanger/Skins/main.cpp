/*
 * main.cpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#include "memory.hpp"
#include "maps.hpp"

#include "skinchanger.hpp"
#include "tf2.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    pid_t pid = strtoul(argv[1], nullptr, 10);

    skinchanger::init();
    if (!skinchanger::load("user"))
    {
        skinchanger::load("nullifiedcat");
    }

    memory::set_active_process(pid);
    tf2::init();

    while (true)
    {
        tf2::update();
        skinchanger::update();
        // yeah 600 updates per second, more updates = less bugs with unusual
        // effects
        usleep(1000000 / 600);
    }
}
