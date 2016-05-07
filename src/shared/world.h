#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include <string>
#include <cstring> // malloc, realloc

#include "common.h"
#include "chunk.h"

const int WorldBoundXLog2 = 26;
const int WorldBoundYLog2 = 9;
const int WorldBoundZLog2 = 26;
const int WorldBoundX = 1 << WorldBoundXLog2; // 2 ^ WorldBoundXLog2
const int WorldBoundY = 1 << WorldBoundYLog2; // 2 ^ WorldBoundYLog2
const int WorldBoundZ = 1 << WorldBoundZLog2; // 2 ^ WorldBoundZLog2

class World
{
private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    int chunkArraySize;
    // Loaded chunks count
    int chunkCount;

    // Expand chunk array
    void expandChunkArray(int expandCount);
    // Reduce chunk array
    void reduceChunkArray(int reduceCount);
    // New pointer at chunks[index]
    void newChunkPtr(int index);
    // Erase pointer at chunks[index]
    void eraseChunkPtr(int index);
    // Get chunk ID
    ChunkID getChunkID(const Vec3& chunkPos) const;
    // Search chunk index, or the index the chunk should insert into
    int getChunkIndex(ChunkID chunkID) const;

public:
    // World name
    std::string name;

    // Initializes world
    World();

    // Add chunk
    Chunk* addChunk(const Vec3& chunkPos);
    // Delete chunk
    int deleteChunk(const Vec3& chunkPos);
    // Get chunk pointer by index
    Chunk* getChunkPtr(int index) const;
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(const Vec3& chunkPos) const;

    // Convert world position to chunk coordinate (one axis)
    int getChunkPos(int pos) const { return pos >> ChunkSizeLog2; }
    // Convert world position to chunk coordinate (all axes)
    Vec3 getChunkPos(const Vec3& pos) const { return Vec3(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z)); }
    // Convert world position to block coordinate in chunk (one axis)
    int getBlockPos(int pos) const { return pos & (ChunkSize - 1); }
    // Convert world position to block coordinate in chunk (all axes)
    Vec3 getBlockPos(const Vec3& pos) const { return Vec3(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z)); }
    
    // Get block data
    BlockData getBlock(const Vec3& pos) const;
    // Set block data
    void setBlock(const Vec3& pos, BlockData block);

};

#endif // !WORLD_H_