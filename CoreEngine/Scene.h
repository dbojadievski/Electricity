#pragma once
#include "CoreHeaders.h"
#include "SceneNode.h"

#include <vector>
using std::vector;

class CoreScene
{
private:
    CORE_ID             m_ID;
    vector<CoreNode *>  m_Nodes;

public:
    CoreScene (CORE_ID id = 0);
    ~CoreScene ();


   CoreNode * Fetch (CoreNode * pNode);
   CORE_BOOLEAN HasNode (CoreNode * pNode);
   CORE_BOOLEAN Add (CoreNode * pNode);
   CORE_BOOLEAN Delete (CoreNode * pNode);
};