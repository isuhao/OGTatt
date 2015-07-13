#include "pedestrian.h"

Pedestrian::Pedestrian(Context *context, MasterControl *masterControl, Vector3 pos):
    NPC(context, masterControl, pos)
{
    male_ = (Random(1.0f)>0.5f) ? true : false;
    rootNode_->SetName("Pedestrian");
    rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));

    rigidBody_->SetMass(5.0f);

    modelNode_ = rootNode_->CreateChild("ModelNode");

    bodyModel_ = modelNode_->CreateComponent<StaticModel>();
    if (male_){
        bodyModel_->SetModel(masterControl_->resources.models.characters.male);
        bodyModel_->SetMaterial(1, masterControl_->resources.materials.cloth);
        bodyModel_->SetMaterial(2, masterControl_->resources.materials.skin);
        bodyModel_->SetMaterial(0, masterControl_->resources.materials.pants);
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    else{
        bodyModel_->SetModel(masterControl_->resources.models.characters.female);
        bodyModel_->SetMaterial(2, masterControl_->resources.materials.cloth);
        bodyModel_->SetMaterial(1, masterControl_->resources.materials.skin);
        bodyModel_->SetMaterial(0, masterControl_->resources.materials.pants);
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    bodyModel_->SetCastShadows(true);
}
