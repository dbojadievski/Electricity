#include "Scene.h"

CoreScene::CoreScene (CORE_ID id)
{
    this->m_ID                  = id;
}

CoreScene::~CoreScene ()
{
    for (size_t currNodeIdx     = 0; currNodeIdx < this->m_Nodes.size (); currNodeIdx++)
    {
        CoreNode * pNode        = this->m_Nodes[currNodeIdx];
        assert (pNode);
        delete pNode;
    }
}

CoreNode *
CoreScene::Fetch (CoreNode * pNode)
{
    CoreNode * pRetVal      = NULL;

    assert (pNode);
    if (!pNode)
        goto end;

    for (size_t headIdx     = 0; headIdx < this->m_Nodes.size (); headIdx++)
    {
        CoreNode * pHead    = this->m_Nodes[headIdx];
        pRetVal             = pHead->FetchSelfOrDescendant (pHead);
        
        if (pRetVal)
            break;
    }
end:
    return pRetVal;
}

CORE_BOOLEAN
CoreScene::HasNode (CoreNode * pNode)
{
    CORE_BOOLEAN hasNode        = false;

    assert (pNode);
    if (!pNode)
        goto end;

    for (size_t currHeadIdx     = 0; currHeadIdx < this->m_Nodes.size (); currHeadIdx++)
    {
        CoreNode * pHead        = this->m_Nodes[currHeadIdx];
        assert (pHead);
        hasNode                 = pHead->IsOrParents (pNode);
        if (hasNode)
            break;
    }

    end:
    return hasNode;
}

CORE_BOOLEAN
CoreScene::Add (CoreNode * pNode)
{
    CORE_BOOLEAN added          = false;

    assert (pNode);
    if (!pNode)
        goto end;

    CORE_BOOLEAN contains       = this->HasNode (pNode);
    if (contains)
        goto end;

    this->m_Nodes.push_back (pNode);

    end:
    return added;
}

CORE_BOOLEAN
CoreScene::Delete (CoreNode * pNode)
{
    CORE_BOOLEAN isDeleted        = false;

    assert (pNode);
    if (!pNode)
        goto end;

    CoreNode *pToDelete         = this->Fetch (pNode);
    if (!pToDelete)
        goto end;

    CoreNode * pParent = pToDelete->GetParent ();
    if (pParent)
    // Child node, unparent. That removes it from the scene.
    {
        pParent->Detach (pNode);
        isDeleted                 = true;
    }
    else
    /* Top-level node, fetch iterator and erase. */
    {
        for (size_t idx = 0; idx < this->m_Nodes.size (); idx++)
        {
            CoreNode * pCurrNode = this->m_Nodes[idx];
            if (pNode == pCurrNode)
            {
                isDeleted           = true;
                this->m_Nodes.erase (this->m_Nodes.begin() + idx);
            }
        }

        assert (isDeleted);
    }

end:
    return isDeleted;
}