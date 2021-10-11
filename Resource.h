#pragma once

class Resource
{
public:
	Resource() noexcept;
	virtual ~Resource() noexcept = default;
protected:
	GUID m_GUID;
};