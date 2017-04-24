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

#include "player.h"
#include "bullet.h"
#include "muzzle.h"
#include "ogtattcam.h"
#include "spawnmaster.h"
#include "inputmaster.h"

#include "character.h"

void Character::RegisterObject(Context *context)
{
    context->RegisterFactory<Character>();
}

Character::Character(Context* context):
    Controllable(context),
    male_{static_cast<bool>(Random(2))},
    alive_{true},
    maxHealth_{100.0f},
    hairStyle_{static_cast<Hair>(Random(HAIR_ALL))},
    sinceLastTurn_{},
    turnInterval_{}
{
}


void Character::OnNodeSet(Node *node)
{
    Controllable::OnNodeSet(node_);

    node_->SetRotation(Quaternion(Random(360.0f), Vector3::UP));

    //Setup physics components
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
//    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.95f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetLinearRestThreshold(0.001f);
    rigidBody_->SetAngularRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(1.0f);

    shot_sfx = MC->GetSample("Shot");

    //Set up graphics components
    CreateBody();

    animCtrl_ = node_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Models/IdleRelax.ani", 1, true, 0.15f);
    animCtrl_->SetSpeed("Models/IdleRelax.ani", 1.0f);
    animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");

    collisionShape_->SetCylinder(0.4f, 1.0f, Vector3::UP * 0.5f);

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Character, HandleNodeCollisionStart));
}

void Character::CreateBody()
{
    for (int c{0}; c < 5; ++c) {
        switch (c){
        case 0:{
            colors_.Push(LucKey::RandomSkinColor());
        } break;
        case 4:{
            colors_.Push(LucKey::RandomHairColor());
        } break;
        default: colors_.Push(LucKey::RandomColor()); break;
        }
    }

    if (male_)
        model_->SetModel(MC->GetModel("Male"));
    else
        model_->SetModel(MC->GetModel("Female"));

    model_->SetMorphWeight(0, Random());

    for (unsigned m{0}; m < model_->GetNumGeometries(); ++m){
        model_->SetMaterial(m, MC->CACHE->GetTempResource<Material>("Materials/Basic.xml"));
        Color diffColor{ colors_[m] };
        if (m == 4){
            if (hairStyle_ == HAIR_BALD)
                diffColor = colors_[0];
            else if (hairStyle_ == HAIR_MOHAWK)
                diffColor = LucKey::RandomHairColor(true);
        }
        model_->GetMaterial(m)->SetShaderParameter("MatDiffColor", diffColor);
        Color specColor{ diffColor * (1.0f-0.1f*m) };
        specColor.a_ = 23.0f - 4.0f * m;
        model_->GetMaterial(m)->SetShaderParameter("MatSpecColor", specColor);
    }

    if (hairStyle_){
        Node* hairNode{node_->GetChild("Head", true)->CreateChild("Hair")};
        hairNode->SetScale(1.0f - (0.13f * !male_));
        hairModel_ = hairNode->CreateComponent<AnimatedModel>();
        hairModel_->SetCastShadows(true);

        switch (hairStyle_){
        default: case HAIR_BALD: case HAIR_SHORT: hairModel_->SetModel(nullptr); break;
        case HAIR_MOHAWK:
            hairModel_->SetModel(MC->GetModel("Mohawk"));
            break;
        case HAIR_SEAGULL:
            hairModel_->SetModel(MC->GetModel("Seagull"));
                    break;
        case HAIR_MUSTAIN:
            hairModel_->SetModel(MC->GetModel("Mustain"));
                    break;
        case HAIR_FROTOAD:
            hairModel_->SetModel(MC->GetModel("Frotoad"));
                    break;
        case HAIR_FLATTOP:
            hairModel_->SetModel(MC->GetModel("Flattop"));
            break;
        }
        if (hairStyle_ != HAIR_BALD && hairStyle_ != HAIR_SHORT)
        {
            hairModel_->SetMorphWeight(0, Random());

            //Set color for hair model
            hairModel_->SetMaterial(CACHE->GetTempResource<Material>("Materials/Basic.xml"));
            Color diffColor = colors_[4];
            hairModel_->GetMaterial()->SetShaderParameter("MatDiffColor", diffColor);
            Color specColor{ diffColor * 0.23f };
            specColor.a_ = 5.0f;
            hairModel_->GetMaterial()->SetShaderParameter("MatSpecColor", specColor);
        }
    }
}

void Character::Set(Vector3 position)
{
    Controllable::Set(position);
    health_= maxHealth_;
}

void Character::Update(float timeStep)
{
    if (!alive_) {

        PODVector<RigidBody*> limbs{};
        node_->GetComponents<RigidBody>(limbs, true);
        for (RigidBody* r : limbs) {

            if (r->IsActive()) {
                r->SetGravityOverride(r->GetGravityOverride() * (1.0f + timeStep));
                return;
            }
        }
        for (RigidBody* r : limbs) {

            r->SetEnabled(false);
        }

        return;
    }

    Controllable::Update(timeStep);

    //Orientation vectors
    Player* player{ GetSubsystem<InputMaster>()->GetPlayerByControllable(this) };
    if (player){
        OGTattCam* cam{ MC->GetCamera(player->GetPlayerId()) };
        if (cam){
            Vector3 camRight{ cam->GetNode()->GetRight() };
            Vector3 camForward{ cam->GetNode()->GetDirection() };
            camRight = LucKey::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
            camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
        }
    }
    //Movement values
    float thrust{ 256.0f };
    float maxSpeed{ 18.0f };

    //Apply movement
    Vector3 force{ move_ * thrust * (1.0f + 0.666f * actions_[0]) * timeStep };
    rigidBody_->ApplyForce(force);

    //Update rotation according to direction of the player's movement.
    Vector3 velocity{ LucKey::Scale(rigidBody_->GetLinearVelocity(), Vector3(1.0f, 0.0f, 1.0f)) };
    Vector3 lookDirection{ velocity + 2.0f * aim_ };
    Quaternion rotation{ node_->GetWorldRotation() };
    Quaternion aimRotation{ rotation };
    aimRotation.FromLookRotation(lookDirection);
    node_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));

    //Update animation
    if (velocity.Length() > 0.1f){

        animCtrl_->PlayExclusive("Models/WalkRelax.ani", 1, true, 0.15f);
        animCtrl_->SetSpeed("Models/WalkRelax.ani", velocity.Length() * 2.3f);
        animCtrl_->SetStartBone("Models/WalkRelax.ani", "MasterBone");
    }
    else if (health_ < maxHealth_){

        animCtrl_->StopAll(0.23f);
        animCtrl_->PlayExclusive("Models/IdleAlert.ani", 0, true, 0.15f);
        animCtrl_->SetStartBone("Models/IdleAlert.ani", "MasterBone");
    } else {
        animCtrl_->StopAll(0.23f);
        animCtrl_->PlayExclusive("Models/IdleRelax.ani", 0, true, 0.1f);
        animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");
    }

    //Shooting
    float bulletHeight{ 0.34f };

    sinceLastShot_ += timeStep;

    if (aim_.Length() > 0.1f) {
        if (sinceLastShot_ > shotInterval_)
        {
            sinceLastShot_ = 0.0;
            Bullet* bullet{ SPAWN->Create<Bullet>() };
            bullet->Set(node_->GetPosition() + Vector3::UP * bulletHeight + collisionShape_->GetSize().x_ * 0.5f * aim_, aim_);

            Muzzle* muzzle{ SPAWN->Create<Muzzle>() };
            muzzle->Set(node_->GetPosition() + Vector3::UP * bulletHeight + 0.1f * aim_, aim_);
            PlaySample(shot_sfx);
        }
    }
}

void Character::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{
    MemoryBuffer contacts{ eventData[NodeCollisionStart::P_CONTACTS].GetBuffer() };

    while (!contacts.IsEof())
    {
        Vector3 contactPosition{ contacts.ReadVector3() };
        Vector3 contactNormal{ contacts.ReadVector3() };
        float contactDistance{ contacts.ReadFloat() };
        float contactImpulse{ contacts.ReadFloat() };

        if (contactImpulse > 2.35f){

           Hit(contactImpulse * 10.0f);
        }
    }
}

void Character::Hit(float damage)
{
    health_ -= damage;

    //Die
    if (health_ <= 0.0f){
        Die();
    }
}
void Character::Die()
{
    alive_ = false;
    CreateRagdoll();
}
void Character::CreateRagdoll()
{
    animCtrl_->SetEnabled(false);

    Vector3 velocity{ rigidBody_->GetLinearVelocity() };

    node_->RemoveComponent<RigidBody>();
    node_->RemoveComponent<CollisionShape>();

    CreateRagdollBone("Hips",  SHAPE_BOX,     Vector3(0.3f, 0.23f, 0.1f),  Vector3(0.0f, 0.05f, -0.05f), Quaternion::IDENTITY);
    CreateRagdollBone("Chest", SHAPE_BOX,     Vector3(0.23f, 0.3f, 0.1f),  Vector3(0.0f, 0.0f,   0.0f),  Quaternion::IDENTITY);
    CreateRagdollBone("Neck",  SHAPE_CAPSULE, Vector3(0.08f, 0.1f, 0.08f), Vector3(0.0f, 0.0f,   0.0f),  Quaternion::IDENTITY);
    CreateRagdollBone("Head",  SHAPE_CAPSULE, Vector3(0.13f, 0.2f, 0.13f), Vector3(0.0f, 0.05f,  0.0f),  Quaternion::IDENTITY);

    CreateRagdollConstraint("Head", "Neck",  CONSTRAINT_CONETWIST, Vector3::UP, Vector3::UP, Vector2(45.0f, 45.0f), Vector2(-20.0f, -20.0f), true);
    CreateRagdollConstraint("Neck", "Chest", CONSTRAINT_HINGE, Vector3::LEFT, Vector3::LEFT, Vector2(45.0f, 45.0f), Vector2(-20.0f, -20.0f), true);
    CreateRagdollConstraint("Chest", "Hips", CONSTRAINT_CONETWIST, Vector3::UP, Vector3::UP, Vector2(45.0f, 45.0f), Vector2(-20.0f, -20.0f), true);

    CreateRagdollBone("UpperArm.L", SHAPE_CAPSULE, Vector3(0.11f, 0.23f, 0.11f), Vector3(0.0f, 0.07f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("UpperArm.R", SHAPE_CAPSULE, Vector3(0.11f, 0.23f, 0.11f), Vector3(0.0f, 0.07f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("LowerArm.L", SHAPE_CAPSULE, Vector3(0.1f, 0.2f, 0.1f), Vector3(0.0f, 0.1f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("LowerArm.R", SHAPE_CAPSULE, Vector3(0.1f, 0.2f, 0.1f), Vector3(0.0f, 0.1f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Hand.L",     SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Hand.R",     SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);

    CreateRagdollConstraint("UpperArm.L", "Chest", CONSTRAINT_CONETWIST, Vector3::UP, Vector3::RIGHT, Vector2(60.0f, 90.0f), Vector2(-45.0f, -45.0f), true);
    CreateRagdollConstraint("UpperArm.R", "Chest", CONSTRAINT_CONETWIST, Vector3::UP, Vector3::LEFT, Vector2(60.0f, 90.0f), Vector2(-45.0f, -45.0f), true);
    CreateRagdollConstraint("LowerArm.L", "UpperArm.L", CONSTRAINT_HINGE, Vector3::FORWARD, Vector3::FORWARD, Vector2(120.0f, 120.0f), Vector2(0.0f, 0.0f), true);
    CreateRagdollConstraint("LowerArm.R", "UpperArm.R", CONSTRAINT_HINGE, Vector3::FORWARD, Vector3::FORWARD, Vector2(120.0f, 120.0f), Vector2(0.0f, 0.0f), true);
    CreateRagdollConstraint("Hand.L",     "LowerArm.L", CONSTRAINT_HINGE, Vector3::RIGHT, Vector3::RIGHT, Vector2(90.0f, 90.0f), Vector2(-45.0f, -45.0f), true);
    CreateRagdollConstraint("Hand.R",     "LowerArm.R", CONSTRAINT_HINGE, Vector3::LEFT, Vector3::LEFT, Vector2(90.0f, 90.0f), Vector2(-45.0f, -45.0f), true);

    CreateRagdollBone("Thigh.L",  SHAPE_CAPSULE, Vector3(0.13f, 0.23f, 0.13f), Vector3(0.0f, 0.07f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Thigh.R",  SHAPE_CAPSULE, Vector3(0.13f, 0.23f, 0.13f), Vector3(0.0f, 0.07f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Shin.L",   SHAPE_CAPSULE, Vector3(0.1f, 0.23f, 0.1f), Vector3(0.0f, 0.1f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Shin.R",   SHAPE_CAPSULE, Vector3(0.1f, 0.23f, 0.1f), Vector3(0.0f, 0.1f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Instep.L", SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Instep.R", SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Toes.L",   SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);
    CreateRagdollBone("Toes.R",   SHAPE_BOX,     Vector3(0.1f, 0.1f, 0.07f), Vector3(0.0f, 0.05f, 0.0f), Quaternion::IDENTITY);

    CreateRagdollConstraint("Thigh.L", "Hips",   CONSTRAINT_CONETWIST, Vector3::LEFT,      Vector3::RIGHT,   Vector2( 45.0f,  10.0f),   Vector2(-45.0f, -10.0f), true);
    CreateRagdollConstraint("Thigh.R", "Hips",   CONSTRAINT_CONETWIST, Vector3::RIGHT,      Vector3::LEFT,    Vector2( 45.0f,  10.0f),   Vector2(-45.0f, -10.0f), true);
    CreateRagdollConstraint("Shin.L", "Thigh.L",  CONSTRAINT_HINGE,     Vector3::LEFT, Vector3::LEFT, Vector2(120.0f, 120.0f),   Vector2(  0.0f,   0.0f), true);
    CreateRagdollConstraint("Shin.R", "Thigh.R",  CONSTRAINT_HINGE,     Vector3::RIGHT, Vector3::RIGHT, Vector2(120.0f, 120.0f),   Vector2(  0.0f,   0.0f), true);
    CreateRagdollConstraint("Instep.L", "Shin.L", CONSTRAINT_HINGE,     Vector3::RIGHT,   Vector3::RIGHT,   Vector2( 45.0f,  45.0f),   Vector2(-23.0f, -23.0f), true);
    CreateRagdollConstraint("Instep.R", "Shin.R", CONSTRAINT_HINGE,     Vector3::LEFT,    Vector3::LEFT,    Vector2( 45.0f,  45.0f),   Vector2(-23.0f, -23.0f), true);
    CreateRagdollConstraint("Toes.L", "Instep.L", CONSTRAINT_HINGE,     Vector3::RIGHT,   Vector3::RIGHT,   Vector2( 10.0f,  10.0f),   Vector2(-10.0f, -10.0f), true);
    CreateRagdollConstraint("Toes.R", "Instep.R", CONSTRAINT_HINGE,     Vector3::LEFT,    Vector3::LEFT,    Vector2( 10.0f,  10.0f),   Vector2(-10.0f, -10.0f), true);

    PODVector<RigidBody*> limbs{};
    node_->GetComponents<RigidBody>(limbs, true);
    for (RigidBody* r : limbs) {

        r->SetLinearVelocity(velocity);
    }
}

void Character::CreateRagdollBone(const String& boneName, ShapeType type, const Vector3& size, const Vector3& position,
    const Quaternion& rotation)
{
    // Find the correct child scene node recursively
    Node* boneNode{ node_->GetChild(boneName, true) };
    if (!boneNode)
        return;

    RigidBody* body{ boneNode->CreateComponent<RigidBody>() };
    // Set mass to make movable
    body->SetMass(100.0f * size.x_ * size.y_ * size.z_);
    // Set damping parameters to smooth out the motion
    body->SetLinearDamping(0.5f);
    body->SetAngularDamping(0.8f);
    // Set rest thresholds to ensure the ragdoll rigid bodies come to rest to not consume CPU endlessly
    body->SetLinearRestThreshold(2.3f);
    body->SetAngularRestThreshold(23.0f);
    body->SetFriction(0.42f);
    body->ApplyImpulse(Vector3::UP * 5.0f);
    body->Activate();

    CollisionShape* shape{ boneNode->CreateComponent<CollisionShape>() };
    // We use either a box or a capsule shape for all of the bones
    if (type == SHAPE_BOX)
        shape->SetBox(size, position, rotation);
    else
        shape->SetCapsule(size.x_, size.y_, position, rotation);
}

void Character::CreateRagdollConstraint(const String& boneName, const String& parentName, ConstraintType type,
    const Vector3& axis, const Vector3& parentAxis, const Vector2& highLimit, const Vector2& lowLimit,
    bool disableCollision)
{
    Node* boneNode{ node_->GetChild(boneName, true) };
    Node* parentNode{ node_->GetChild(parentName, true) };
    if (!boneNode || !parentNode)
        return;

    Constraint* constraint{ boneNode->CreateComponent<Constraint>() };
    constraint->SetConstraintType(type);
    // Most of the constraints in the ragdoll will work better when the connected bodies don't collide against each other
    constraint->SetDisableCollision(disableCollision);
    // The connected body must be specified before setting the world position
    constraint->SetOtherBody(parentNode->GetComponent<RigidBody>());
    // Position the constraint at the child bone we are connecting
    constraint->SetWorldPosition(boneNode->GetWorldPosition());
    // Configure axes and limits
    constraint->SetAxis(axis);
    constraint->SetOtherAxis(parentAxis);
    constraint->SetHighLimit(highLimit);
    constraint->SetLowLimit(lowLimit);
}

Substance Character::GetSubstance()
{
    return Substance::Flesh;
}

void Character::Think(float timeStep)
{
//    aim_ = Quaternion(Random(360.0f), Vector3::UP) * Vector3::FORWARD * !Random(23);

    sinceLastTurn_ += timeStep;
    if (sinceLastTurn_ > turnInterval_){
        sinceLastTurn_ = 0.0f;
        turnInterval_ = Random(2.3f, 5.0f);
        int r = Random(5);
        switch (r) {
        case 0:
            move_ = Vector3::FORWARD;
            break;
        case 1:
            move_ = Vector3::LEFT;
            break;
        case 2:
            move_ = Vector3::BACK;
            break;
        case 3:
            move_ = Vector3::RIGHT;
            break;
        case 4:
            move_ = Vector3::ZERO;
            break;
        default:
            break;
        }
        if (r!=4){
            move_ += Vector3(Random(-0.1f, 0.1f), 0.0f, Random(-0.1f, 0.1f));
        }

        move_ *= 0.5f;
    }
}
