/*
 * tf2.hpp
 *
 *  Created on: Feb 3, 2018
 *      Author: nullifiedcat
 */

#pragma once

#include "memory.hpp"
#include <cstdint>
#include "sdk/ClientEntityList.hpp"

namespace tf2
{

extern uintptr_t client_so;
extern uintptr_t server_so;

void init();
int get_local_player();
void update();
memory::remote_pointer<sdk::Entity> get_entity(int entity);

extern memory::remote_pointer<class ClientState> remote_client_state;
extern memory::remote_pointer<sdk::ClientEntityList> remote_entity_list;
extern sdk::ClientEntityList *local_entity_list;
}
