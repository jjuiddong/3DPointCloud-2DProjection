//
// 2017-07-27, jjuiddong
// Upgrade DX9 - DX11
// VertexBuffer Class
//
#pragma once


using namespace common;

class cRenderer;

class cVertexBuffer
{
public:
	cVertexBuffer();
	cVertexBuffer(const cVertexBuffer &rhs);
	virtual ~cVertexBuffer();

	bool Create(ID3D11Device *device, const int vertexCount, const int sizeofVertex
		, const D3D11_USAGE usage= D3D11_USAGE_DEFAULT);
	bool Create(ID3D11Device *device, const int vertexCount, const int sizeofVertex
		, const void *vertices
		, const D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

	void* Lock(ID3D11DeviceContext *devContext, const D3D11_MAP flag = D3D11_MAP_WRITE_DISCARD);
	void Unlock(ID3D11DeviceContext *devContext);
	void Bind(ID3D11DeviceContext *devContext) const;
	void Clear();

	int GetSizeOfVertex() const;
	int GetVertexCount() const;

	cVertexBuffer& operator=(const cVertexBuffer &rhs);


public:
	ID3D11Buffer *m_vtxBuff;
	int m_sizeOfVertex; // stride
	int m_vertexCount;
};

	
inline int cVertexBuffer::GetSizeOfVertex() const { return m_sizeOfVertex; }
inline int cVertexBuffer::GetVertexCount() const { return m_vertexCount; }

