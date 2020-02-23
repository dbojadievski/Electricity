#pragma once

#include "AssetDescriptor.h"
#include <string>
#include "CoreTypes.h"
#include "Math.h"

using namespace std;

class MaterialAssetDescriptor : public AssetDescriptor
{
private:
	FLOAT4		m_Ambient;
	FLOAT4		m_Diffuse;
	FLOAT4		m_Specular;
	CORE_REAL	m_SpecularPower;
public:
	MaterialAssetDescriptor ();
	MaterialAssetDescriptor (const string & name, CORE_ID identifier);

	FLOAT4		GetAmbient ()		const;
	FLOAT4		GetDiffuse ()		const;
	FLOAT4		GetSpecular ()		const;
	CORE_REAL	GetSpecularPower () const;

	void SetAmbient (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetDiffuse (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetSpecular (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetSpecularPower (CORE_REAL specularPower);
	CORE_ASSET_TYPE GetAssetType () const;
};