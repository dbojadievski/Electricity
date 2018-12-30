#include "SceneNode.h"

SceneNode::SceneNode (CORE_ID gameObjId, SceneNode * pParentNode)
{
    this->m_GameObjectId            = gameObjId;
    this->m_pParent                 = pParentNode;

    if (!this->m_pParent)
    {
        this->m_LocalTransform      = IdentityTransform ();
        this->m_CachedTransform     = IdentityTransform ();
    }
    else
    {
        this->m_LocalTransform      = IdentityTransform ();
        this->m_pParent->m_Children.push_back (this);
        this->UpdateLocalTransform ();
    }
}

Transform
SceneNode::GetParentTransform ()
{
    Transform retVal            = IdentityTransform();
    
    if (this->m_pParent)
        retVal                  =  this->m_pParent->m_LocalTransform;
    
    return retVal;
}

Transform
SceneNode::ComputeTransform ()
{
    Transform parentTransform   = this->GetParentTransform ();
    Transform newTransform      = (parentTransform * this->m_LocalTransform);

    return newTransform;
}

void
SceneNode::UpdateLocalTransform ()
{
    // TODO(Dino): If we add pre-transform values, mix them in here.
    Transform translationMatrix     = XMMatrixTranslation (this->m_preXYZ.x, this->m_preXYZ.y, this->m_preXYZ.z);
    this->m_LocalTransform          = ( this->m_LocalTransform * translationMatrix );
    this->m_CachedTransform         = ( this->ComputeTransform () * translationMatrix );
    
    this->m_preXYZ.x    = 0.0f;
    this->m_preXYZ.y    = 0.0f;
    this->m_preXYZ.z    = 0.0f;
}

void
SceneNode::UpdateAll ()
{
    this->UpdateLocalTransform ();

    size_t numChildren              = this->m_Children.size ();
    for (size_t currChildIdx = 0; currChildIdx < numChildren; currChildIdx++)
    {
        SceneNode * pChild          = this->m_Children[currChildIdx];
        pChild->UpdateLocalTransform ();
    }
}

CORE_BOOLEAN
SceneNode::IsOrParents (SceneNode * pNode)
{
    CORE_BOOLEAN retVal     = false;

    assert (pNode);
    if (!pNode)
        goto end;


    if (this == pNode)
    {
        retVal              = true;
        goto end;
    }
    
    for (size_t i = 0; i < this->m_Children.size (); i++)
    {
        SceneNode * pNext       = this->m_Children[i];
        retVal                  = pNext->IsOrParents (pNode);
        if (retVal)
            break;
    }

end:
    return retVal;
}

SceneNode::~SceneNode ()
{
    this->m_pParent         = NULL;
    this->m_CachedTransform = IdentityTransform ();
    this->m_LocalTransform  = IdentityTransform ();

    this->m_preXYZ.x        = 0.0f;
    this->m_preXYZ.y        = 0.0f;
    this->m_preXYZ.z        = 0.0f;

    size_t numChildren          = this->m_Children.size ();
    for (size_t currChildIdx    = 0; currChildIdx < numChildren; currChildIdx++)
    {
        SceneNode * pChild  = this->m_Children[currChildIdx];
        delete pChild;
    }
    this->m_Children.clear ();
}

SceneNode *
SceneNode::FetchSelfOrDescendant (SceneNode * pNode)
{
    SceneNode  *pRetVal     = NULL;
    
    assert (pNode);
    if (!pNode)
        goto end;

    if (this == pNode) 
    {
        pRetVal             = this;
        goto end;
    }

    for (size_t childIdx    = 0; childIdx < this->m_Children.size (); childIdx++)
    {
        SceneNode * pChild  = this->m_Children[childIdx];
        pRetVal             = pChild->FetchSelfOrDescendant (pNode);
        if (pRetVal)
            break;
    }
end:
    return pRetVal;
}

CORE_BOOLEAN
SceneNode::Detach (SceneNode * pChild)
{
    CORE_BOOLEAN isDetached     = false;

    assert (pChild);
    if (!pChild)
        goto end;

    for (size_t childIdx    = 0; childIdx < this->m_Children.size(); childIdx++)
    {
        SceneNode * pNode   = this->m_Children[childIdx];
        assert (pNode);

        if (pNode == pChild)
        {
            this->m_Children.erase (this->m_Children.begin() + childIdx);
            pChild->m_pParent   = NULL;
            pChild->UpdateAll ();
            isDetached          = true;
            break;
        }
    }

    end:
    return isDetached;
}

CORE_BOOLEAN
SceneNode::Attach (SceneNode * pChild)
{
    CORE_BOOLEAN isAttached         = false;

    assert (pChild);
    assert (!pChild->m_pParent);

    /* Don't break existing hierarchies. Make the user detach consciously. */
    if (!pChild || pChild->m_pParent)
        goto end;
    
    pChild->m_pParent               = this;
    isAttached                      = true;
    this->m_Children.push_back (pChild);
    pChild->UpdateAll ();

end:
    return isAttached;
}

SceneNode *
SceneNode::GetParent ()
{
    return this->m_pParent;
}

CORE_ID
SceneNode::GetObjectID ()
{
    return this->m_GameObjectId;
}