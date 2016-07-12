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

#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstring>
#include <boost/core/noncopyable.hpp>
#include "../shared/common.h"
#include "opengl.h"

class VertexFormat
{
public:
    // Vertex attribute count
    int textureCount, colorCount, normalCount, coordinateCount;
    // Vertex attributes count (sum of all)
    int vertexAttributeCount;

    VertexFormat() : textureCount(0), colorCount(0), normalCount(0), coordinateCount(0), vertexAttributeCount(0)
    {
    }

    VertexFormat(int textureElementCount, int colorElementCount, int normalElementCount, int coordinateElementCount)
        : textureCount(textureElementCount), colorCount(colorElementCount), normalCount(normalElementCount), coordinateCount(coordinateElementCount),
          vertexAttributeCount(textureElementCount + colorElementCount + normalElementCount + coordinateElementCount)
    {
        assert(normalCount == 0 || normalCount == 3);
    }
};

class VertexArray
    :boost::noncopyable
{
public:
    VertexArray(int maxVertexes, const VertexFormat& format)
        : m_maxVertexes(maxVertexes), m_vertexes(0), m_format(format)
    {
        m_data = new float[m_maxVertexes * format.vertexAttributeCount];
        m_vertexAttributes = new float[format.vertexAttributeCount];
    }

    ~VertexArray()
    {
        delete[] m_data;
        delete[] m_vertexAttributes;
    }

    void clear()
    {
        memset(m_data, 0, m_maxVertexes * m_format.vertexAttributeCount * sizeof(float));
        memset(m_vertexAttributes, 0, m_format.vertexAttributeCount * sizeof(float));
        m_vertexes = 0;
    }

    // Set texture coordinates
    void setTexture(int size, const float* texture)
    {
        assert(size <= m_format.textureCount);
        memcpy(m_vertexAttributes, texture, size * sizeof(float));
    }

    // Set color value
    void setColor(int size, const float* color)
    {
        assert(size <= m_format.colorCount);
        memcpy(m_vertexAttributes + m_format.textureCount, color, size * sizeof(float));
    }

    // Set normal vector
    void setNormal(int size, const float* attribute)
    {
        assert(size <= m_format.normalCount);
        memcpy(m_vertexAttributes + m_format.textureCount + m_format.colorCount, attribute, size * sizeof(float));
    }

    // Add vertex
    void addVertex(const float* coords)
    {
        memcpy(m_data + m_vertexes, m_vertexAttributes, m_format.vertexAttributeCount * sizeof(float));
        memcpy(m_data + m_vertexes + m_format.vertexAttributeCount, coords, m_format.coordinateCount * sizeof(float));
        m_vertexes++;
    }

    // Get current vertex format
    const VertexFormat& getFormat() const
    {
        return m_format;
    }

    // Get current vertex data
    const float* getData() const
    {
        return m_data;
    }

    // Get current vertex count
    int getVertexCount() const
    {
        return m_vertexes;
    }

private:
    // Max vertex count
    const int m_maxVertexes;
    // Vertex count
    int m_vertexes;
    // Vertex array format
    VertexFormat m_format;
    // Vertex array
    float* m_data;
    // Current vertex attributes
    float* m_vertexAttributes;
};

class VertexBuffer
{
public:
    // Buffer ID
    VertexBufferID id;
    // Vertex count
    int vertexes;
    // Buffer format
    VertexFormat format;

    VertexBuffer()
    {
    }

    VertexBuffer(const VertexFormat& format_, VertexBufferID id_ = 0) : format(format_), id(id_)
    {
    }

    VertexBuffer(const VertexArray& va);

    // Render vertex buffer
    void render();
};

#endif // !VERTEXARRAY_H_
