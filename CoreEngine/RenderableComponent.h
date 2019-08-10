#pragma once
#include "IComponent.h"
#include "GameObject.h"

#include "Model.h"
#include "tinyxml2.h"
#include "IAssetManager.h"

using namespace tinyxml2;
class RenderableComponent : public IComponent
{
protected:
	ModelDescriptor	*	m_pModel;
public:
	RenderableComponent(ModelDescriptor * pModel);
	RenderableComponent();
	RenderableComponent(XMLElement * pRenderableXML, IAssetManager *pAssetManager);
	ModelDescriptor  * GetModel() const;

	virtual void Init();
	virtual void Update(float dT);
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const;
};

