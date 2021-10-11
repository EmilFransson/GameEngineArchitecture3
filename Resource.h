#pragma once

class Resource
{
public:
	Resource() noexcept = default;
	virtual ~Resource() noexcept = default;
protected:
	GUID m_GUID;
};