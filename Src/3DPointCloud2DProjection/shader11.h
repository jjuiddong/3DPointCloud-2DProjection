//
// 2017-07-27, jjuiddong
// Dx11 Shader class
//
#pragma once


using namespace common;

class cShader11
{
public:
	cShader11();
	virtual ~cShader11();
	bool Create(ID3D11Device *device
		, const wchar_t *vertexShaderFileName
		, const wchar_t *pixelShaderFileName);
	void Clear();


protected:
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, const wchar_t* shaderFilename);


public:
	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D11InputLayout *m_layout;
};
