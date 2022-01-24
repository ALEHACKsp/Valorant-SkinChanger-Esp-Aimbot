/*
 * skinchanger.cpp
 *
 *  Created on: Aug 30, 2017
 *      Author: nullifiedcat
 */

#include "skinchanger.hpp"
#include "tf2.hpp"
#include "memory.hpp"
#include "hardcoded.hpp"
#include "sdk/CAttributeList.hpp"

#include <fstream>
#include <iostream>
#include "../include/sdk/ClientEntityList.hpp"

namespace skinchanger
{

std::unordered_map<int, skinchange_data> skinchanges{};
weapon_cookie cookie{ 0 };
nlohmann::json schema_data{};

void weapon_cookie::update(int entity)
{
    auto ent = tf2::get_entity(entity);

    if (!ent.pointer)
        return;

    sdk::CAttributeList list = memory::read<sdk::CAttributeList>(
        ent.pointer + netvars::m_AttributeList);
    attrs  = list.attributes.m_Size;
    eidx   = entity;
    defidx = memory::read<int>(ent.pointer + netvars::m_iItemDefinitionIndex);
    valid  = true;
}

bool weapon_cookie::check()
{
    if (!valid)
        return false;

    auto ent = tf2::get_entity(eidx);
    if (!ent.pointer)
        return false;

    sdk::CAttributeList list = memory::read<sdk::CAttributeList>(
        ent.pointer + netvars::m_AttributeList);
    if (attrs != list.attributes.m_Size)
        return false;
    if (defidx !=
        memory::read<int>(ent.pointer + netvars::m_iItemDefinitionIndex))
        return false;
    return true;
}

void skinchange_data::apply(int entity)
{
    auto ent = tf2::get_entity(entity);
    if (!ent.pointer)
        return;

    auto remote_defidx_pointer = memory::remote_pointer<int>(
        ent.pointer + netvars::m_iItemDefinitionIndex);

    if (redirect && remote_defidx_pointer.read() != redirect)
    {
        remote_defidx_pointer.write(redirect);
        skinchange(redirect).apply(entity);
        return;
    }

    auto remote_attributes_pointer =
        memory::remote_pointer<sdk::CAttributeList>(ent.pointer +
                                                    netvars::m_AttributeList);

    sdk::CAttributeList list = remote_attributes_pointer.read();

    for (const auto &mod : modifiers)
    {
        list.set(mod.defidx, mod.value);
    }

    remote_attributes_pointer.write(list);
}

int last_weapon = 0;

void update()
{
    auto local = tf2::get_entity(tf2::get_local_player());
    if (!local.pointer)
        return;

    int current_weapon =
        memory::remote_pointer<int>(local.pointer + netvars::m_hActiveWeapon)
            .read() &
        0xFFF;

    if ((last_weapon != current_weapon) || !cookie.check())
    {
        for (int i = 0; i < 4; i++)
        {
            int weapon =
                local.offset<int>(netvars::m_hMyWeapons + i * 4).read() & 0xFFF;

            if (weapon < 32 || weapon >= 0xFFF)
                continue;

            auto weapon_entity = tf2::get_entity(weapon);
            if (not weapon_entity.pointer)
                continue;

            skinchange(
                weapon_entity.offset<int>(netvars::m_iItemDefinitionIndex)
                    .read())
                .apply(weapon);
        }
        cookie.update(current_weapon);
    }
    last_weapon = current_weapon;
}

skinchange_data &skinchange(int idx)
{
    try
    {
        return skinchanges.at(idx);
    }
    catch (std::out_of_range &oor)
    {
        skinchanges.emplace(idx, skinchange_data{});
        return skinchanges.at(idx);
    }
}

/*void fromJSON(const nlohmann::json &json)
{
    for (auto it = json.begin(); it != json.end(); ++it)
    {
        skinchange_data mod{};

        int defidx          = std::stoi(it.key());
        mod.defidx          = defidx;
        mod.redirect = it.value()["redirect"].get<int>();
        printf("Loading data for: %d -> %d\n", mod.defidx, mod.redirect);
        auto mods = it.value()["modifiers"];
        for (auto mi = mods.begin(); mi != mods.end(); ++mi)
        {
            modifier mmod{};
            mmod.defidx = uint16_t(mi.value()[0].get<int>());
            auto v      = mi.value()[1];
            float fv    = v.get<float>();

            if (v.is_number_float())
            {
                mmod.value = *(uint32_t *) &fv;
            }
            else
            {
                mmod.value  = uint32_t(fv);
            }
            printf("\tAttribute %i %.2f / %u\n", mmod.defidx, *(float
*)&mmod.value, mmod.value);
            mod.modifiers.push_back(mmod);
        }
        skinchanges[defidx] = mod;
    }
    std::cout << "Successfully loaded!\n";
}*/

void fromJSON(const nlohmann::json &json)
{
    for (auto it = json.begin(); it != json.end(); ++it)
    {
        printf("Trying to load %s..\n", it.key().c_str());

        skinchange_data mod{};

        int defidx = schema_data["items"][it.key()].get<int>();
        mod.defidx = defidx;

        auto data = it.value();

        printf("Loading %s: %d\n", it.key().c_str(), defidx);

        if (data.find("redirect") != data.end())
        {
            mod.redirect =
                schema_data["items"][data["redirect"].get<std::string>()];
            printf("Redirect -> %d\n", mod.redirect);
            continue;
        }

        auto mods = data["attributes"];
        for (auto mi = mods.begin(); mi != mods.end(); ++mi)
        {
            modifier mmod{};
            mmod.defidx = uint16_t(
                schema_data["attributes"][mi.key()]["index"].get<int>());

            float value = 0.0f;

            if (mi.value().is_string())
            {
                value = schema_data["enums"][mi.value().get<std::string>()]
                            .get<float>();
            }
            else
            {
                value = mi.value().get<float>();
            }

            if (schema_data["attributes"][mi.key()]["integer"].get<bool>())
            {
                mmod.value = uint32_t(value);
            }
            else
            {
                mmod.value = *(uint32_t *) &value;
            }

            mod.modifiers.push_back(mmod);
        }

        if (data.find("australium") != data.end())
        {
            printf("\tAustralium\n");
            float one = 1.0f;
            mod.modifiers.push_back(modifier{ 2027, *(uint32_t *) &one });
            mod.modifiers.push_back(modifier{ 542, *(uint32_t *) &one });
            mod.modifiers.push_back(modifier{ 2022, *(uint32_t *) &one });
        }
        if (data.find("skin") != data.end())
        {
            printf("\tSkin: %s\n", data["skin"].get<std::string>().c_str());
            int skin = schema_data["skins"][data["skin"].get<std::string>()]
                           .get<int>();
            mod.modifiers.push_back(modifier{ 834, skin });
        }

        skinchanges[defidx] = mod;
    }
    std::cout << "Successfully loaded!\n";
}

bool load(const std::string &filename)
{
    std::ifstream file("data/" + filename + ".json");
    if (file.bad())
    {
        std::cerr << "Error opening file\n";
        return false;
    }
    nlohmann::json json;

    try
    {
        file >> json;
        fromJSON(json);
    }
    catch (std::exception &ex)
    {
        std::cerr << "could not load: " << ex.what() << "\n";
        return false;
    }
    return true;
}

void init()
{
    std::ifstream schema("data/schema.json");
    if (schema.bad())
    {
        std::cerr << "can't load data/schema.json\n";
        return;
    }
    schema >> schema_data;

    auto upgrades = schema_data["upgrades"];

    for (auto u = upgrades.begin(); u != upgrades.end(); ++u)
    {
        int source                  = std::atoi(u.key().c_str());
        int up                      = u.value().get<int>();
        skinchange(source).redirect = up;
        printf("Upgrade %d -> %d\n", source, up);
    }
}
}
