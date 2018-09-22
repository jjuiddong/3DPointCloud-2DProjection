
#include "stdafx.h"
#include "vertexbuffer.h"


cVertexBuffer::cVertexBuffer() 
	: m_sizeOfVertex(0)
	, m_vertexCount(0)
	, m_vtxBuff(NULL)
{
}

// Copy Constructor
cVertexBuffer::cVertexBuffer(const cVertexBuffer &rhs)
{
	operator = (rhs);
}

cVertexBuffer::~cVertexBuffer()
{
	Clear();
}


bool cVertexBuffer::Create(ID3D11Device *device, const int vertexCount, const int sizeofVertex
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
)
{
	SAFE_RELEASE(m_vtxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeofVertex * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT: 
		bd.CPUAccessFlags = 0;
		break;
	case D3D11_USAGE_DYNAMIC:
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		assert(0); // very slow flag
		break;
	}

	if (FAILED(device->CreateBuffer(&bd, NULL, &m_vtxBuff)))
	{
		assert(0);
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	return true;
}


bool cVertexBuffer::Create(ID3D11Device *device, const int vertexCount, const int sizeofVertex
	, const void *vertices
	, const D3D11_USAGE usage //= D3D11_USAGE_DEFAULT
)
{
	SAFE_RELEASE(m_vtxBuff);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeofVertex * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT:
		bd.CPUAccessFlags = 0;
		break;
	case D3D11_USAGE_DYNAMIC:
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		assert(0); // very slow flag
		break;
	}

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	if (FAILED(device->CreateBuffer(&bd, &InitData, &m_vtxBuff)))
	{
		assert(0);
		return false;
	}

	m_vertexCount = vertexCount;
	m_sizeOfVertex = sizeofVertex;
	return true;
}


void* cVertexBuffer::Lock(ID3D11DeviceContext *devContext
	, const D3D11_MAP flag //= D3D11_MAP_WRITE_DISCARD
)
{
	RETV(!m_vtxBuff, NULL);

	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = devContext->Map(m_vtxBuff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void cVertexBuffer::Unlock(ID3D11DeviceContext *devContext)
{
	RET(!m_vtxBuff);
	devContext->Unmap(m_vtxBuff, 0);
}


void cVertexBuffer::Bind(ID3D11DeviceContext *devContext) const
{
	const UINT offset = 0;
	const UINT stride = (UINT)m_sizeOfVertex;
	devContext->IASetVertexBuffers(0, 1, &m_vtxBuff, &stride, &offset);
}


void cVertexBuffer::Clear()
{
	m_vertexCount = 0;
	m_sizeOfVertex = 0;
	SAFE_RELEASE(m_vtxBuff);
}


cVertexBuffer& cVertexBuffer::operator=(const cVertexBuffer &rhs)
{
	if (this != &rhs)
	{
		Clear();

		m_sizeOfVertex = rhs.m_sizeOfVertex;
		m_vertexCount = rhs.m_vertexCount;
		m_vtxBuff = NULL; // No Copy
	}
	return *this;
}
