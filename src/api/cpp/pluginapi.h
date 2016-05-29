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

#ifndef PLUGINAPI_H_
#define PLUGINAPI_H_

namespace NEWorld
{

    typedef int int32;
    typedef unsigned int uint32;

    struct Vec3i
    {
        int32 x, y, z;
    };

    struct BlockType
    {
        char* blockname = nullptr;
        bool solid;
        bool translucent;
        bool opaque;
        int32 explodePower;
        int32 hardness;
    };

    struct BlockData
    {
        uint32 id : 12;
        uint32 brightness : 4;
        uint32 state : 16;
    };

    typedef BlockData* (*buildChunkFunc)(const Vec3i&);

    struct PluginData
    {
        char* pluginName = nullptr;
        int32 blocksCount;
        BlockType* blocks = nullptr;
        buildChunkFunc buildChunk = nullptr;
    };

}

#endif