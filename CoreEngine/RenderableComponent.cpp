#include "RenderableComponent.h"
#include "Model.h"


RenderableComponent::RenderableComponent()
{
	this->m_Type		= COMPONENT_TYPE_RENDERABLE;
	this->m_pModel		= NULL;
	this->m_pMaterial	= NULL;
}

RenderableComponent::RenderableComponent(ModelDescriptor * pModel, MaterialDescriptor * pMaterial) : RenderableComponent()
{
	assert(pModel);
	assert (pMaterial);
	this->m_pModel		= pModel;
	this->m_pMaterial	= pMaterial;
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

		XMLElement * pMaterial = pElement->FirstChildElement ("material");
		assert (pMaterial);
		if (pMaterial)
		{
			const char * pStrName = pMaterial->Attribute ("name");
			assert (pStrName);
			if (pStrName)
			{
				MaterialDescriptor * pDesc = pAssetManager->GetMaterialDescriptor (pStrName);
				assert (pDesc);
				if (pDesc)
					this->m_pMaterial = pDesc;
			}
		}

		this->m_Type = COMPONENT_TYPE_RENDERABLE;
	}
}

ModelDescriptor *
RenderableComponent::GetModel() const
{
	return this->m_pModel;
}

MaterialDescriptor *
RenderableComponent::GetMaterial () const
{
	return this->m_pMaterial;
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