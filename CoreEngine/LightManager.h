#pragma once
#include "CoreTypes.h"
#include "BasicLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include <assert.h>
#include <vector>

using std::vector;
class LightManager
{
private:
	vector<PointLight *> m_PointLights;
	vector<DirectionalLight *> m_DirectionalLights;

public:
	void AddPointLight(PointLight * pLight);
	void RemovePointLight(PointLight * pLight);

	void AddDirectionalLight(DirectionalLight * pLight);
	void RemoveDirectionalLight(DirectionalLight * pLight);

	void ClearAllLights();
};