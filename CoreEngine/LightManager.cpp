#include "LightManager.h"

#include <algorithm>

void
LightManager::AddPointLight(PointLight * pLight)
{
	assert(pLight);
	this->m_PointLights.push_back(pLight);
}

void
LightManager::RemovePointLight(PointLight * pLight)
{
	assert(pLight);
	
	auto iter = std::find(this->m_PointLights.begin(), this->m_PointLights.end(), pLight);
	if (iter != this->m_PointLights.end())
		this->m_PointLights.erase(iter);
}

void
LightManager::AddDirectionalLight(DirectionalLight * pLight)
{
	assert(pLight);
	this->m_DirectionalLights.push_back(pLight);
}

void
LightManager::RemoveDirectionalLight(DirectionalLight * pLight)
{
	assert(pLight);

	auto iter = std::find(this->m_DirectionalLights.begin(), this->m_DirectionalLights.end(), pLight);
	if (iter != this->m_DirectionalLights.end())
		this->m_DirectionalLights.erase(iter);
}

void
LightManager::ClearAllLights()
{
	this->m_PointLights.clear();
	this->m_DirectionalLights.clear();
}