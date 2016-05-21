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

#include <fstream>

#include "Urho3D/Urho2D/Drawable2D.h"

#include "level.h"
#include "tile.h"
#include "wallcollider.h"
#include "streetlight.h"
#include "ogtattcam.h"

namespace Urho3D {
template <> unsigned MakeHash(const IntVector2& value)
  {
    return LucKey::IntVector2ToHash(value);
  }
}

Level::Level(Context *context, Vector3 position, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Level, HandleUpdate));
    rootNode_ = masterControl_->world.scene->CreateChild("World");
//    masterControl_->platformMap_[rootNode_->GetID()] = WeakPtr<Dungeon>(this);

    rootNode_->SetPosition(position);
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    TmxFile2D* tmxFile{masterControl_->cache_->GetResource<TmxFile2D>("Maps/test.tmx")};
    if (tmxFile)
        InitializeFromMap(*tmxFile);
    else
        InitializeRandom();

}


void Level::InitializeFromMap(const TmxFile2D& tmxFile)
{
    XMLFile* xmlFile{masterControl_->cache_->GetResource<XMLFile>(tmxFile.GetName())};
    XMLElement xmlMap{xmlFile->GetRoot("map")};

    const TmxLayer2D* layer0{tmxFile.GetLayer(0)};
    if (!layer0)
        return;
    if (layer0->GetType() != LT_TILE_LAYER)
        return;

    const TmxTileLayer2D& tileLayer{*static_cast<const TmxTileLayer2D*>(layer0)};

    for (int y{0}; y < tileLayer.GetHeight(); ++y) {
        for (int x{0}; x < tileLayer.GetWidth(); ++x) {
            if (Tile2D* tile{tileLayer.GetTile(x, y)}) {
                if (tile->HasProperty("building"))
                    AddTile(IntVector2(x, y));
            }
        }
    }

    rootNode_->Translate(Vector3(-0.5f * tileLayer.GetWidth(), 0.0f, 0.5f * tileLayer.GetHeight()));

    //Read objects
    const TmxLayer2D* layer1{tmxFile.GetLayer(1)};
    if (!layer1)
        return;
    if (layer1->GetType() != LT_OBJECT_GROUP)
        return;

    const TmxObjectGroup2D& objectGroup{*static_cast<const TmxObjectGroup2D*>(layer1)};

    for (unsigned i{0}; i < objectGroup.GetNumObjects(); ++i) { ///Vector<TileMapObject2D*> TmxObjectGroup2D::GetObjects()
        TileMapObject2D* o{objectGroup.GetObject(i)};

        int gid{o->GetTileGid()};
        PropertySet2D* properties{tmxFile.GetTilePropertySet(gid)};
        if (!properties)
            continue;

        float scaleFactor{1.56f};
        Vector3 pos(scaleFactor * o->GetPosition().x_,
                    0.0f,
                    scaleFactor * o->GetPosition().y_ - tileLayer.GetHeight() + 1.0f);
        Quaternion rot{}; ///float TileMapObject2D::GetRotation()
                          ///int TileMapObject2D::GetId()

        //Create objects
        if (properties->HasProperty("streetlight")) {
            new StreetLight(context_,
                            masterControl_,
                            pos + rootNode_->GetPosition(),
                            rot);
        }
    }

    AddColliders();
}


void Level::InitializeRandom()
{
    IntVector2 firstCoordPair{IntVector2(0,0)};
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    int addedTiles{1};
    int worldSize{64};//Random(32, 128);

    while (addedTiles < worldSize){
        //Pick a random exsisting tile from a list.
        Vector<IntVector2> coordsVector = tileMap_.Keys();
        IntVector2 randomTileCoords = coordsVector[Random((int)coordsVector.Size())];

        //Check neighbours in random orer
        char startDir{static_cast<char>(Random(1,4))};
        for (int direction{startDir}; direction < startDir+4; direction++){
            int clampedDir{direction};
            if (clampedDir > 4) clampedDir -= 4;
            if (CheckEmptyNeighbour(randomTileCoords, (TileElement)clampedDir, true))
            {
                IntVector2 newTileCoords{GetNeighbourCoords(randomTileCoords, (TileElement)clampedDir)};
                AddTile(newTileCoords);
                addedTiles++;
            }
        }
    }

    AddColliders();
}

bool Level::EnableSlot(IntVector2 coords)
{
    //collisionMap_[coords]->rootNode_->SetEnabled(true);
}
void Level::EnableSlots()
{
    /*for (int i = 0; i < collisionMap_.Values().Length(); i++)
    {
        if (GetTileType(collisionMap_.Values()[i]->coords_) <= TT_EMPTY)
            EnableSlot(collisionMap_.Values()[i]->coords_);
    }*/
}

bool Level::DisableSlot(IntVector2 coords)
{
    //collisionMap_[coords]->rootNode_->SetEnabled(false);
}
void Level::DisableSlots()
{
    /*
    for (int i = 0; i < collisionMap_.Values().Length(); i++)
    {
        DisableSlot(collisionMap_.Values()[i]->coords_);
    }
    */
}

void Level::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

void Level::AddTile(IntVector2 newTileCoords)
{
    tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
}

void Level::AddColliders()
{
   Vector<IntVector2> tileCoordsVec = tileMap_.Keys();
    for (uint nthTile = 0; nthTile < tileCoordsVec.Size(); nthTile++){
        for (int element = 0; element <= 4; element++){
            IntVector2 tileCoords{tileCoordsVec[nthTile]};
            IntVector2 checkCoords{GetNeighbourCoords(tileCoords, (TileElement)element)};
            if (CheckEmpty(checkCoords, false) && CheckEmpty(checkCoords, true)){
                collisionMap_[tileCoords] = new WallCollider(context_, this, tileCoords);
                break;
            }
        }
    }
}

bool Level::CheckEmpty(IntVector2 coords, bool checkTiles = true) const
{
    if (checkTiles) return (!tileMap_.Keys().Contains(coords));
    else return (!collisionMap_.Keys().Contains(coords));
}


bool Level::CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool checkTiles = true) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector2 Level::GetNeighbourCoords(IntVector2 coords, TileElement element) const
{
    IntVector2 shift = IntVector2::ZERO;
    switch (element){
    case TE_NORTH: shift.y_ =  1; break;
    case TE_EAST:  shift.x_ =  1; break;
    case TE_SOUTH: shift.y_ = -1; break;
    case TE_WEST:  shift.x_ = -1; break;
    case TE_NORTHEAST: shift.x_ =  1; shift.y_ =  1; break;
    case TE_SOUTHEAST: shift.x_ =  1; shift.y_ = -1; break;
    case TE_SOUTHWEST: shift.x_ = -1; shift.y_ = -1; break;
    case TE_NORTHWEST: shift.x_ = -1; shift.y_ =  1; break;
    default: case TE_CENTER: break;
    }
    return coords + shift;
}

CornerType Level::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3] = {false, false, false};
    switch (element){
    case TE_NORTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_EAST);
    }
    break;
    case TE_SOUTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_EAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
    }break;
    case TE_SOUTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_WEST);
    }break;
    case TE_NORTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_WEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
    }break;
    default: break;
    }

    int neighbourMask = 0;
    for (int i = 2; i >= 0; i--){
        neighbourMask += !emptyCheck[i] << i;
    }
    switch (neighbourMask){
    case 0: return CT_IN; break;
    case 1: return CT_TWEEN; break;
    case 2: return CT_DOUBLE; break;
    case 3: return CT_NONE; break;
    case 4: return CT_NONE; break;
    case 5: return CT_OUT; break;
    case 6: return CT_NONE; break;
    case 7: return (element == TE_SOUTHEAST) ? CT_FILL : CT_NONE; break;
    default: return CT_NONE; break;
    }
}
