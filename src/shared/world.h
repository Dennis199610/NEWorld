/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include <string>
#include <cstdlib> // malloc, realloc, free
#include "aabb.h"
#include "chunk.h"
#include "blockmanager.h"
#include "chunkpointerarray.h"

class PluginManager;

class World
{
public:
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    virtual ~World()
    {
        if (m_chunks != nullptr)
        {
            // TODO: Save and destroy chunks
            free(m_chunks);
        }
    }

    // Get world name
    const std::string& getWorldName() const
    {
        return m_name;
    }

    // Get chunk count
    size_t getChunkCount() const
    {
        return m_chunkCount;
    }

    // Get chunk pointer by index
    Chunk* getChunkPtr(size_t index) const
    {
        assert(index < m_chunkCount);
        return m_chunks[index];
    }

    // Get chunk pointer by chunk coordinates
    // Optimized for clustered search
    Chunk* getChunkPtr(const Vec3i& chunkPos) const
    {
        // TODO: Try chunk pointer cache
        Chunk* res = getChunkPtrNonclustered(chunkPos);
        // TODO: Update chunk pointer array
        // TODO: Update chunk pointer cache
        return res;
    }

    // Non-clustered & thread-safe version of getChunkPtr()
    // Will not update CPA and CPC
    Chunk* getChunkPtrNonclustered(const Vec3i& chunkPos) const
    {
        // TODO: Try chunk pointer array
        size_t index = getChunkIndex(chunkPos);
        if (index >= m_chunkCount || m_chunks[index]->getPosition() != chunkPos) return nullptr;
        return m_chunks[index];
    }

    bool isChunkLoaded(const Vec3i& chunkPos) const
    {
        return getChunkPtr(chunkPos) != nullptr;
    }

    // Add chunk
    virtual Chunk* addChunk(const Vec3i& chunkPos) = 0;

    // Delete chunk
    int deleteChunk(const Vec3i& chunkPos);

#ifdef NEWORLD_COMPILER_RSHIFT_ARITH
    // Convert world position to chunk coordinate (one axis)
    static int getChunkPos(int pos)
    {
        return pos >> ChunkSizeLog2;
    }
#else
    // Convert world position to chunk coordinate (one axis)
    static int getChunkPos(int pos)
    {
        if (pos >= 0) return pos / ChunkSize;
        return (pos - ChunkSize + 1) / ChunkSize;
    }
#endif

    // Convert world position to chunk coordinate (all axes)
    static Vec3i getChunkPos(const Vec3i& pos)
    {
        return Vec3i(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockPos(int pos)
    {
        return pos & (ChunkSize - 1);
    }

    // Convert world position to block coordinate in chunk (all axes)
    static Vec3i getBlockPos(const Vec3i& pos)
    {
        return Vec3i(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z));
    }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        assert(chunk != nullptr);
        return chunk->getBlock(getBlockPos(pos));
    }

    // Get block reference
    BlockData& getBlock(const Vec3i& pos)
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        assert(chunk != nullptr);
        return chunk->getBlock(getBlockPos(pos));
    }

    // Set block data
    void setBlock(const Vec3i& pos, BlockData block) const
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        assert(chunk != nullptr);
        chunk->setBlock(getBlockPos(pos), block);
    }

    int getDaylightBrightness() const
    {
        return m_daylightBrightness;
    }

    const BlockManager& getBlockTypes() const
    {
        return m_blocks;
    }

    ChunkPointerArray& getChunkPointerArray()
    {
        return m_cpa;
    }

    std::vector<AABB> getHitboxes(const AABB& range) const;

    // Main update
    void update();

protected:
    World(const std::string& name, const PluginManager& plugins, const BlockManager& blocks)
        : m_name(name), m_plugins(plugins), m_blocks(blocks), m_chunkCount(0), m_chunkArraySize(1024), m_cpa(8), m_daylightBrightness(15)
    {
        m_chunks = static_cast<Chunk**>(malloc(m_chunkArraySize * sizeof(Chunk*)));
    }

    World(World&& rhs) noexcept
        : m_name(std::move(rhs.m_name)), m_plugins(rhs.m_plugins), m_blocks(rhs.m_blocks),
          m_chunkCount(rhs.m_chunkCount), m_chunkArraySize(rhs.m_chunkArraySize), m_cpa(std::move(rhs.m_cpa)), m_daylightBrightness(rhs.m_daylightBrightness)
    {
        std::swap(m_chunks, rhs.m_chunks);
    }

    // World name
    std::string m_name;
    // Loaded plugins
    const PluginManager& m_plugins;
    // Loaded blocks
    const BlockManager& m_blocks;
    // Loaded chunk count
    size_t m_chunkCount;
    // Size of chunk array
    size_t m_chunkArraySize;
    // All chunks (chunk array)
    Chunk** m_chunks;
    // CPA
    ChunkPointerArray m_cpa;

    int m_daylightBrightness;

    // Expand chunk array
    void expandChunkArray(size_t expandCount);

    // Reduce chunk array
    void reduceChunkArray(size_t reduceCount)
    {
        assert(m_chunkCount >= reduceCount);
        m_chunkCount -= reduceCount;
    }

    // New pointer at m_chunks[index]
    void newChunkPtr(size_t index)
    {
        expandChunkArray(1);
        for (size_t i = m_chunkCount - 1; i > index; i--)
            m_chunks[i] = m_chunks[i - 1];
        m_chunks[index] = nullptr;
    }

    // Erase pointer at m_chunks[index]
    void eraseChunkPtr(size_t index)
    {
        for (size_t i = index; i < m_chunkCount - 1; i++)
            m_chunks[i] = m_chunks[i + 1];
        reduceChunkArray(1);
    }

    // Search chunk index, or the index the chunk should insert into
    size_t getChunkIndex(const Vec3i& chunkPos) const;

};

#endif // !WORLD_H_
