#include "HemisphericLight.h"
size_t 
HemisphericLight::GetSize ()
{
	size_t size = 0;
	size += sizeof (m_Pad);
	size += sizeof (m_AmbientUp);
	size += sizeof (m_AmbientDown);

	return size;
}

HemisphericLight::HemisphericLight ()
{
	this->m_Pad.x = 0;
	this->m_Pad.y = 0;

	this->m_AmbientDown.x = 0;
	this->m_AmbientDown.y = 0;
	this->m_AmbientDown.z = 0;

	this->m_AmbientUp.x = 0;
	this->m_AmbientUp.y = 0;
	this->m_AmbientUp.z = 0;
}