#pragma once
#include "CoreTypes.h"
#include <string>
#include "Math.h"
#include "MaterialDescriptor.h"

using namespace std;

class Material
{
private:
	CORE_ID		m_Identifier;
	string		m_Name;
	FLOAT4		m_Ambient;
	FLOAT4		m_Diffuse;
	FLOAT4		m_Specular;
	CORE_REAL	m_SpecularPower;
public:
	Material ();
	Material (const string & name, CORE_ID identifier);

	FLOAT4		GetAmbient ()		const;
	FLOAT4		GetDiffuse ()		const;
	FLOAT4		GetSpecular ()		const;
	CORE_REAL	GetSpecularPower () const;
	CORE_ID		GetIdentifier ()	const;

	void SetAmbient (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetDiffuse (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetSpecular (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a);
	void SetSpecularPower (CORE_REAL specularPower);

	static CORE_ERROR FromDescriptor (MaterialAssetDescriptor * pDesc, Material * pMaterial);
};