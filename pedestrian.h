#include <Urho3D/Urho3D.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "mastercontrol.h"
#include "npc.h"

class Pedestrian : public NPC
{
    OBJECT(Pedestrian);
public:
    Pedestrian(Context *context, MasterControl* masterControl, Vector3 pos);
protected:
    Node* modelNode_;
    StaticModel* bodyModel_;
    bool male_;
private:
    void HandleUpdate(StringHash eventType, VariantMap &eventData);
};
