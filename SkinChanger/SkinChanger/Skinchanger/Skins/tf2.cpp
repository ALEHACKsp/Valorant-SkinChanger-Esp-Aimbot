/*
 * tf2.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: nullifiedcat
 */

#include "tf2.hpp"
#include "memory.hpp"
#include "maps.hpp"
#include "hardcoded.hpp"

#include <fstream>

#include <stdio.h>

namespace tf2
{

uintptr_t client_so{ 0 };
uintptr_t engine_so{ 0 };

memory::remote_pointer<class ClientState> remote_client_state;
memory::remote_pointer<sdk::ClientEntityList> remote_entity_list;
sdk::ClientEntityList *local_entity_list;

void init()
{
    printf("Starting, process = %d\n", memory::active_process);

    local_entity_list =
        (sdk::ClientEntityList *) malloc(sizeof(sdk::ClientEntityList));

    std::ifstream maps("/proc/" + std::to_string(memory::active_process) +
                           "/maps",
                       std::ios::in);

    std::string line;
    while (std::getline(maps, line))
    {
        try
        {
            proc_map map(line);
            if (!client_so && map.file == "client.so")
            {
                client_so = map.start;
                printf("client.so: %08x\n", client_so);
            }
            if (!engine_so && map.file == "engine.so")
            {
                engine_so = map.start;
                printf("engine.so: %08x\n", engine_so);
            }
        }
        catch (std::exception &ex)
        {
            printf("Exception: %s\n", ex.what());
            continue;
        }
    }

    printf("Getting global objects...\n");

    remote_entity_list.pointer  = client_so + offsets::entity_list;
    remote_client_state.pointer = engine_so + offsets::client_state;

    printf("Done.\n");
}

int get_local_player()
{
    return 1 + remote_client_state.offset<int>(offsets::local_player).read();
}

void update()
{
    memory::read_buffer(remote_entity_list.pointer,
                        (unsigned char *) local_entity_list,
                        sizeof(sdk::ClientEntityList));
}

memory::remote_pointer<sdk::Entity> get_entity(int entity)
{
    return local_entity_list->entities[entity].m_pEntity;
}
}
