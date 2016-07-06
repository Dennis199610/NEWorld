/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "plugin.h"
#include "logger.h"

typedef PluginData* NWAPICALL InitFunction();

int Plugin::loadFrom(const string& filename)
{
    InitFunction* init = nullptr;
    try
    {
        m_lib.load(filename);
        init = m_lib.get<InitFunction>("init");
        m_data = init();
    }
    catch (std::exception& e)
    {
        if (m_lib.is_loaded()) return m_status = 1; // Failed: could not load
        if (init == nullptr) return m_status = 2; // Failed: entry not found
        warningstream << "Failed: unexpected exception: " << e.what();
    }
    return m_status = 0;
}
