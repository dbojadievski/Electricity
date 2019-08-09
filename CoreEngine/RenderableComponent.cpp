#include "RenderableComponent.h"
#include "Model.h"


RenderableComponent::RenderableComponent()
{
	this->m_Type = COMPONENT_TYPE_RENDERABLE;
	this->m_pModel = NULL;
}

RenderableComponent::RenderableComponent(ModelDescriptor * pModel) : RenderableComponent()
{
	assert(pModel);
	this->m_pModel		= pModel;
}

RenderableComponent::RenderableComponent(XMLElement * pElement, IAssetManager *pAssetManager) : RenderableComponent()
{
	assert(pElement);
	assert(pAssetManager);

	if (pElement && pAssetManager)
	{
		XMLElement * pModel = pElement->FirstChildElement("model");
		assert(pModel);
		if (pModel)
		{
			const char * pStrName = pModel->Attribute("name");
			assert(pStrName);
			if (pStrName)
			{
				ModelAssetDescriptor * pDesc = pAssetManager->GetModelDescriptor(pStrName);
				assert(pDesc);
				if (pDesc)
					this->m_pModel = pDesc;
			}
		}
	}
}

ModelDescriptor *
RenderableComponent::GetModel() const
{
	return this->m_pModel;
}

CORE_BOOLEAN
RenderableComponent::AreMultipleInstancesAllowed() const
{
	return false;
}

void
RenderableComponent::Init()
{}

void
RenderableComponent::Update(float dT)
{

}