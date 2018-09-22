
#include "stdafx.h"
#include "shader11.h"


cShader11::cShader11()
{
}

cShader11::~cShader11()
{
	Clear();
}


bool cShader11::Create(ID3D11Device *device
	, const wchar_t *vertexShaderFileName
	, const wchar_t *pixelShaderFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;

	result = D3DCompileFromFile(vertexShaderFileName, NULL, NULL
		, "VS"
		, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, vertexShaderFileName);
		else
			MessageBox(NULL, vertexShaderFileName, L"Missing Shader File", MB_OK);
		return false;
	}

	result = D3DCompileFromFile(pixelShaderFileName, NULL, NULL
		, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, pixelShaderFileName);
		else
			MessageBox(NULL, pixelShaderFileName, L"Missing Shader File", MB_OK);
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer()
		, vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer()
		, pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);
	SAFE_RELEASE(errorMessage);
	return true;
}


void cShader11::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const wchar_t* shaderFilename)
{
	string errMsg;
	errMsg = "Error compiling shader.\n";
	errMsg += (char*)(errorMessage->GetBufferPointer());
	MessageBoxA(NULL, errMsg.c_str(), "Error", MB_OK);
	return;
}

void cShader11::Clear()
{
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_layout);
}
