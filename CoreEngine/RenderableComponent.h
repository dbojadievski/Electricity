#pragma once
#include "IComponent.h"
#include "GameObject.h"

#include "Model.h"
#include "tinyxml2.h"
#include "IAssetManager.h"
#include "MaterialDescriptor.h"

using namespace tinyxml2;
class RenderableComponent : public IComponent
{
protected:
	ModelDescriptor	*		m_pModel;
	MaterialDescriptor *	m_pMaterial;
public:
	RenderableComponent(ModelDescriptor * pModel, MaterialDescriptor * pMaterial);
	RenderableComponent();
	RenderableComponent(XMLElement * pRenderableXML, IAssetManager *pAssetManager);
	ModelDescriptor  * GetModel() const;

	virtual void Init();
	virtual void Update(float dT);
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const;
};

