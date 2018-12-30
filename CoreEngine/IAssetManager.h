#pragma once
#include "CoreHeaders.h"
#include "IEngineSystem.h"
#include "AssetDescriptor.h"
#include "Mesh.h"

class IAssetManager : IEngineSystem
{
public:

    virtual CoreMesh * GetMesh (AssetDescriptor * pDescriptor) = 0;
};