#pragma once
#include "CoreError.h"
#include "Mesh.h"
#include "Math.h"

using namespace std;
#include <vector>
class SceneNode
{
private:
    SceneNode *         m_pParent;
    vector<SceneNode *> m_Children;
    
    Transform           m_CachedTransform;
    Transform           m_LocalTransform;

    VEC3                m_preXYZ;
    FASTVEC             m_prePYR;

    CORE_ID             m_GameObjectId;
public:
    SceneNode (CORE_ID gameObjId = 0, SceneNode * pParentNode = NULL);

    ~SceneNode ();

    Transform ComputeTransform ();
    Transform GetParentTransform ();
    void UpdateLocalTransform ();
    void UpdateAll ();

    CORE_BOOLEAN IsOrParents (SceneNode * pNode);
    SceneNode * FetchSelfOrDescendant (SceneNode * pNode);

    SceneNode * GetParent ();
    CORE_BOOLEAN Detach (SceneNode * pChild);
    CORE_BOOLEAN Attach (SceneNode * pChild);

    CORE_ID GetObjectID ();
};

typedef SceneNode CoreNode;