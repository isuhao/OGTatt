/* OG Tatt
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TILE_H
#define TILE_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "level.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Level;

#define LAYER_THICKNESS 1.5f

class Tile : public Object
{
    friend class Level;
    friend class Deco;
    URHO3D_OBJECT(Tile, Object);
public:
    Tile(const TileInfo info, Level *level);

    IntVector3 GetCoords() const { return info_.coords_; }
private:
    Level* level_;

    TileInfo info_;
    Node* rootNode_;
    CollisionShape* collisionShape_;
};

#endif // TILE_H
