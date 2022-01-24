/*
 * skinchanger.hpp
 *
 *  Created on: Aug 30, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "json.hpp"
#include <unordered_map>
#include <string>
#include "../include/sdk/CUtlVector.hpp"

namespace skinchanger
{

struct weapon_cookie
{
    void update(int entity);
    bool check();

public:
    int eidx{ 0 };
    int defidx{ 0 };
    int attrs{ 0 };
    bool valid{ false };
};

struct modifier
{
    uint16_t defidx{ 0 };
    uint32_t value{ 0 };
};

struct skinchange_data
{
    void apply(int entity);

public:
    int defidx{ 0 };
    int redirect{ 0 };
    std::vector<modifier> modifiers{};
};

skinchange_data &skinchange(int idx);

void fromJSON(const nlohmann::json &json, bool merge);
bool load(const std::string &filename);
void update();
void init();
}
