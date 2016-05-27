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

#ifndef VEC3_H_
#define VEC3_H_

#include <cmath>
#include <algorithm>
using std::abs;
using std::max;

#include <boost/operators.hpp>

template<typename T>
class Vec3:
    boost::totally_ordered<Vec3<T> >
{
public:
    T x, y, z;

    Vec3() :x(0), y(0), z(0)
    {}

    Vec3(T x_, T y_, T z_) :x(x_), y(y_), z(z_)
    {}

    // Get the square of vector length, notice that the result can overflow. TODO: fixit
    T lengthSqr() const
    {
        return x*x + y*y + z*z;
    }

    // Get vector length
    double length() const
    {
        return sqrt(double(lengthSqr()));
    }

    // Get the Euclidean Distance between vectors
    double euclideanDistance(const Vec3& rhs) const
    {
        return (*this - rhs).length();
    }

    // Get the Chebyshev Distance between vectors
    T chebyshevDistance(const Vec3& rhs) const
    {
        return max(max(abs(x - rhs.x), abs(y - rhs.y)), abs(z - rhs.z));
    }

    // Get the Manhattan Distance between vectors
    T manhattanDistance(const Vec3& rhs) const
    {
        return abs(x - rhs.x) + abs(y - rhs.y) + abs(z - rhs.z);
    }

    bool operator< (const Vec3& rhs) const
    {
        if (x != rhs.x)
            return x < rhs.x;
        if (y != rhs.y)
            return y < rhs.y;
        if (z != rhs.z)
            return z < rhs.z;
        return false;
    }

    bool operator== (const Vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    Vec3 operator+ (const Vec3& rhs) const
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3 operator- (const Vec3& rhs) const
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
};

using  Vec3i = Vec3<int>;
using  Vec3f = Vec3<float>;
using  Vec3d = Vec3<double>;

#endif // !VEC3_H_
