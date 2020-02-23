#include "MaterialDescriptor.h"

MaterialAssetDescriptor::MaterialAssetDescriptor ()
{
	this->m_Ambient			= FLOAT4 (0, 0, 0, 1);
	this->m_Diffuse			= FLOAT4 (0, 0, 0, 1);
	this->m_Specular		= FLOAT4 (0, 0, 0, 1);
	this->m_SpecularPower	= 0.0;

	this->m_Name			= "";
	this->m_Path			= "";
	this->m_Identifier		= 0;

	this->m_AssetType		= ASSET_TYPE_MATERIAL;
}

MaterialAssetDescriptor::MaterialAssetDescriptor (const string & name, CORE_ID identifier) :MaterialAssetDescriptor ()
{
	this->m_Name			= name;
	this->m_Identifier		= identifier;
	this->m_Path			= "";
}

FLOAT4 
MaterialAssetDescriptor::GetAmbient () const
{
	return this->m_Ambient;
}

FLOAT4
MaterialAssetDescriptor::GetDiffuse () const
{
	return this->m_Diffuse;
}

FLOAT4
MaterialAssetDescriptor::GetSpecular () const
{
	return this->m_Specular;
}

CORE_REAL
MaterialAssetDescriptor::GetSpecularPower () const
{
	return this->m_SpecularPower;
}

void
MaterialAssetDescriptor::SetAmbient (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a)
{
	this->m_Ambient.x	= r;
	this->m_Ambient.y	= g;
	this->m_Ambient.z	= b;
	this->m_Ambient.w	= a;
}

void
MaterialAssetDescriptor::SetDiffuse (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a)
{
	this->m_Diffuse.x	= r;
	this->m_Diffuse.y	= g;
	this->m_Diffuse.z	= b;
	this->m_Diffuse.w	= a;
}

void
MaterialAssetDescriptor::SetSpecular (CORE_REAL r, CORE_REAL g, CORE_REAL b, CORE_REAL a)
{
	this->m_Specular.x = r;
	this->m_Specular.y = g;
	this->m_Specular.z = b;
	this->m_Specular.w = a;
}

void
MaterialAssetDescriptor::SetSpecularPower (CORE_REAL specularPower)
{
	this->m_SpecularPower	= specularPower;
}

CORE_ASSET_TYPE
MaterialAssetDescriptor::GetAssetType () const
{
	return ASSET_TYPE_MATERIAL;
}
