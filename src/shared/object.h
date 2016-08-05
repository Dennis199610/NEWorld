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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "aabb.h"
#include "vec3.h"

class Object
{
public:
    virtual ~Object()
    {
    }

    const Vec3d& getPosition() const
    {
        return m_position;
    }

    void setPosition(const Vec3d& val)
    {
        m_position = val;
    }

    const Vec3d& getRotation() const
    {
        return m_rotation;
    }

    void setRotation(const Vec3d& val)
    {
        m_rotation = val;
    }

    const Vec3d& getSize() const
    {
        return m_size;
    }

    const AABB& getHitbox() const
    {
        return m_hitbox;
    }

    virtual void update() = 0;

protected:
    Vec3d m_position, m_size, m_rotation;
    AABB m_hitbox;

};

#endif // OBJECT_H_
