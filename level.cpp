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
#include "honti.h"
#include "cookiejar.h"
#include "ogtattcam.h"

namespace Urho3D {

    template <> unsigned MakeHash(const IntVector2& value)
    {
        return LucKey::IntVector2ToHash(value);
    }
}

Level::Level(Vector3 position):
Object(MC->GetContext())
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Level, HandleUpdate));
    rootNode_ = MC->world.scene->CreateChild("World");
//    MC->platformMap_[rootNode_->GetID()] = WeakPtr<Dungeon>(this);

    rootNode_->SetPosition(position);
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    TmxFile2D* tmxFile{MC->cache_->GetResource<TmxFile2D>("Maps/smallTest.tmx")};
    if (tmxFile)
        InitializeFromMap(*tmxFile);
//    else
//        InitializeRandom();

}

void Level::InitializeFromMap(const TmxFile2D& tmxFile)
{
    XMLFile* xmlFile{MC->cache_->GetResource<XMLFile>(tmxFile.GetName())};

    float mapWidth{};
    float mapHeight{};

    int nthTileLayer{0};

    for (unsigned i{0}; i < tmxFile.GetNumLayers(); ++i){
        const TmxLayer2D* layer{tmxFile.GetLayer(i)};
        if (mapWidth < layer->GetWidth())
            mapWidth = layer->GetWidth();
        if (mapHeight < layer->GetHeight())
            mapHeight = layer->GetHeight();

        if (!layer)
            return;
        if (layer->GetType() == LT_TILE_LAYER){

            const TmxTileLayer2D& tileLayer{*static_cast<const TmxTileLayer2D*>(layer)};

            for (int y{0}; y < tileLayer.GetHeight(); ++y) {
                for (int x{0}; x < tileLayer.GetWidth(); ++x) {
                    if (Tile2D* tile{tileLayer.GetTile(x, y)}) {
                        if (tile->HasProperty("model")){

                            TileInfo info{};
                            info.coords_ = IntVector3(x, nthTileLayer, y);
                            info.obstacle_ = tile->HasProperty("obstacle");

                            if (tile->HasProperty("model")){
                                info.modelName_ = tile->GetProperty("model");
                            }

                            if (tile->HasProperty("material")){
                                info.materialName_ = tile->GetProperty("material");
                            }

                            AddTile(info);
                        }
                    }
                }
            }
            ++nthTileLayer;

        } else if (layer->GetType() == LT_OBJECT_GROUP){

            const TmxObjectGroup2D& objectGroup{*static_cast<const TmxObjectGroup2D*>(layer)};

            for (unsigned i{0}; i < objectGroup.GetNumObjects(); ++i) { ///Vector<TileMapObject2D*> TmxObjectGroup2D::GetObjects()
                TileMapObject2D* object{objectGroup.GetObject(i)};

                int gid{object->GetTileGid()};
                PropertySet2D* properties{tmxFile.GetTilePropertySet(gid)};
                if (!properties)
                    continue;

                float scaleFactor{1.56f};
                Vector3 pos(scaleFactor * object->GetPosition().x_,
                            0.0f,
                            scaleFactor * object->GetPosition().y_);
                pos += Vector3(-0.5f*mapWidth, 0.0f, -0.5f*mapHeight+1);

                Quaternion rot{}; ///float TileMapObject2D::GetRotation()
                ///int TileMapObject2D::GetId()

                //Create objects
                if (properties->HasProperty("streetlight")) {
                    new StreetLight(pos,
                                    rot);
                } else if (properties->HasProperty("car")) {
                    new Cookiejar(pos);
                } else if (properties->HasProperty("bike")) {
                    new Honti(pos);
                }
            }
        }
    }
    rootNode_->Translate(Vector3(-0.5f * mapWidth,
                                 0.0f,
                                 0.5f * mapHeight));
}

void Level::AddTile(TileInfo info)
{
//    tileMap_[info.coords_] = new Tile(info, this);
    new Tile(info, this);
}

/*
void Level::InitializeRandom()
{

    IntVector2 firstCoordPair{IntVector2(0,0)};
    tileMap_[firstCoordPair] = new Tile(firstCoordPair, this);
    // Add random tiles
    int addedTiles{1};
    int worldSize{64};//Random(32, 128);

    while (addedTiles < worldSize){
        //Pick a random exsisting tile from a list.
        Vector<IntVector2> coordsVector{tileMap_.Keys()};
        IntVector2 randomTileCoords{coordsVector[Random((int)coordsVector.Size())]};

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
*/

void Level::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

/*
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

bool Level::CheckEmpty(IntVector3 coords, bool checkTiles = true) const
{
    if (checkTiles) return (!tileMap_.Keys().Contains(coords));
    else return (!collisionMap_.Keys().Contains(coords));
}


bool Level::CheckEmptyNeighbour(IntVector3 coords, TileElement element, bool checkTiles = true) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector3 Level::GetNeighbourCoords(IntVector3 coords, TileElement element) const
{
    IntVector3 shift = IntVector3::ZERO;
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

CornerType Level::PickCornerType(IntVector3 tileCoords, TileElement element) const
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
*/
