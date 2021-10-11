#pragma once
#include "Resource.h"
//TO BE TOTALLY UPDATED!
class Mesh : public Resource
{
public:
	Mesh() noexcept = default;
	virtual ~Mesh() noexcept override = default;
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
};

class MeshOBJ : public Mesh 
{
public:
	MeshOBJ() noexcept = default;
	virtual ~MeshOBJ() noexcept override = default;
private:

};