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

#ifndef LEVEL_H
#define LEVEL_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Tile;
class WallCollider;
struct TileInfo;

typedef struct TileInfo {
    IntVector3 coords_{};
    bool obstacle_{};
    String modelName_{};
    Vector<String> materialNames_{};
} TileInfo;

enum TileElement {TE_CENTER = 0, TE_NORTH, TE_EAST, TE_SOUTH, TE_WEST, TE_NORTHWEST, TE_NORTHEAST, TE_SOUTHEAST, TE_SOUTHWEST, TE_LENGTH};
enum CornerType {CT_NONE, CT_IN, CT_OUT, CT_TWEEN, CT_DOUBLE, CT_FILL};
enum TileType {TT_SPACE, TT_EMPTY, TT_ENGINE};

class Level : public Object
{
    URHO3D_OBJECT(Level, Object);
    friend class InputMaster;
public:
    Level(Context* context);

    Node* rootNode_;

//    bool CheckEmpty(Vector3 coords, bool checkTiles) const { return CheckEmpty(IntVector3(round(coords.x_), round(coords.y_), round(coords.z_)), checkTiles); }
//    bool CheckEmpty(IntVector3 coords, bool checkTiles) const;
//    bool CheckEmptyNeighbour(IntVector3 coords, TileElement element, bool tileMap) const;
//    IntVector3 GetNeighbourCoords(IntVector3 coords, TileElement element) const;
//    CornerType PickCornerType(IntVector3 tileCoords, TileElement element) const;
//    TileType GetTileType(IntVector3 coords);
//    TileType GetNeighbourType(IntVector3 coords, TileElement element);

//    void AddColliders();

    void AddTile(const TileInfo& info);
private:
    RigidBody* rigidBody_;
//    HashMap<IntVector3, SharedPtr<Tile> > tileMap_;
//    HashMap<IntVector3, SharedPtr<WallCollider> > collisionMap_;
//    HashMap<IntVector3, TileType> buildingMap_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

//    void SetTileType(IntVector3 coords, TileType type = TT_ENGINE);
//    void EmptyTile(IntVector3 coords) {SetTileType(coords, TT_EMPTY);}
    void InitializeFromMap(const TmxFile2D& tmxFile);
//    void InitializeRandom();
};

#endif
