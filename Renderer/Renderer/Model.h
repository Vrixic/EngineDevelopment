#pragma once
#include "Vrixic/GenericDefines.h"
#include <vector>
#include <d3d11.h>
#include "Vrixic/Math/Vector2D.h"
#include "Vrixic/Math/Vector3D.h"
#include "blob.h"
#include <d3dcompiler.h>

#include <string>
#include <iostream>


struct TerrainVertex
{
	Vector3D Position;
	Vector3D Normal;
	Vector2D TexCoord;
};

struct Model
{
public:
	bool CanRender = false;
	uint32 VertexCount;
	uint32 IndexCount;

	std::vector<TerrainVertex> Vertices;
	std::vector<uint32> Indices;

private:
	ID3D11Device* DeviceHandle = nullptr;

	ID3D11Buffer* VertexBufferHandle = nullptr;
	ID3D11Buffer* IndexBufferHandle = nullptr;

	ID3D11VertexShader* VertexShaderHandle = nullptr;
	ID3D11PixelShader* PixelShaderHandle = nullptr;

	ID3D11InputLayout* InputLayoutHandle = nullptr;

public:
	Model(ID3D11Device* deviceHandle, uint32 inVertexCount, uint32 inIndexCount,
		const char* inVSFilePathToCSO, const char* inPSFilePathToCSO, std::vector<TerrainVertex> verts, std::vector<uint32> indices)
	{
		DeviceHandle = deviceHandle;
		Vertices = verts;
		Indices = indices;
		InitModel(inVertexCount, inIndexCount, inVSFilePathToCSO, inPSFilePathToCSO);
	}

	Model(const Model& other) = delete;
	Model operator=(const Model& other) = delete;

	~Model()
	{
		if (VertexBufferHandle)
			VertexBufferHandle->Release();

		if (VertexShaderHandle)
			VertexShaderHandle->Release();

		if (PixelShaderHandle)
			PixelShaderHandle->Release();

		if (InputLayoutHandle)
			InputLayoutHandle->Release();

		//safe_release(IndexBufferHandle);
	}

	

	void InitModel(uint32 inVertexCount, uint32 inIndexCount, const char* inVSFilePathToCSO, const char* inPSFilePathToCSO)
	{
		if (VertexBufferHandle != nullptr)
		{
			std::cout << "\nTrying to InitModel on a model thats already initialized...\n";
			return;
		}

		VertexCount = inVertexCount;
		IndexCount = inIndexCount;

		/* Create Shaders */
		CreateShaders(DeviceHandle, &InputLayoutHandle, &VertexShaderHandle, &PixelShaderHandle, inVSFilePathToCSO, inPSFilePathToCSO);

		/* Create vertex buffer */
		D3D11_BUFFER_DESC debug_desc = {};
		debug_desc.ByteWidth = VertexCount * sizeof(TerrainVertex);
		debug_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		debug_desc.Usage = D3D11_USAGE_DEFAULT;
		debug_desc.CPUAccessFlags = 0;
		debug_desc.MiscFlags = 0;
		debug_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA debug_srd = {};
		debug_srd.pSysMem = Vertices.data();
		debug_srd.SysMemPitch = 0;
		debug_srd.SysMemSlicePitch = 0;

		DeviceHandle->CreateBuffer(&debug_desc, &debug_srd, &VertexBufferHandle);

		debug_desc.ByteWidth = VertexCount * sizeof(uint32);
		debug_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		debug_desc.Usage = D3D11_USAGE_DEFAULT;
		debug_desc.CPUAccessFlags = 0;
		debug_desc.MiscFlags = 0;
		debug_desc.StructureByteStride = 0;

		debug_srd.pSysMem = Indices.data();
		debug_srd.SysMemPitch = 0;
		debug_srd.SysMemSlicePitch = 0;

		DeviceHandle->CreateBuffer(&debug_desc, &debug_srd, &IndexBufferHandle);
	}

public:

	static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// Disable optimizations to further improve shader debugging
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* pErrorBlob = nullptr;
		hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	}

	static void CreateShaders(ID3D11Device* deviceHandle, ID3D11InputLayout** inInputLayoutHandle, ID3D11VertexShader** inVertexShaderHandle,
		ID3D11PixelShader** inPixelShaderHandle, const char* inVSFilePathToCSO, const char* inPSFilePathToCSO)
	{
		HRESULT hr;
		// Compile Vertex shader 
		ID3DBlob* pVSBlob = nullptr;
		std::string STR(inVSFilePathToCSO);
		std::wstring WSTR(STR.begin(), STR.end());
		hr = CompileShaderFromFile(WSTR.c_str(), "main", "vs_5_0", &pVSBlob);
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			std::cout << "\nerror\n";
		}

		D3D11_INPUT_ELEMENT_DESC desc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
		};
		deviceHandle->CreateInputLayout(desc, ARRAYSIZE(desc), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),  inInputLayoutHandle);

		// Create The Vertex shader
		hr = deviceHandle->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, inVertexShaderHandle);
		if (FAILED(hr))
		{
			pVSBlob->Release();
			std::cout << "\nerror\n";
		}

		// Compile pixel shader
		ID3DBlob* pPSBlob = nullptr;
		std::string STR2(inPSFilePathToCSO);
		std::wstring WSTR2(STR2.begin(), STR2.end());

		hr = CompileShaderFromFile(WSTR2.c_str(), "main", "ps_5_0", &pPSBlob);
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			std::cout << "\nerror\n";
		}

		// Create the pixel shader
		hr = deviceHandle->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, inPixelShaderHandle);
		pPSBlob->Release();
		if (FAILED(hr))
			std::cout << "\nerror\n";
	}

public:
	uint32 GetVertexCount() const
	{
		return VertexCount;
	}

	ID3D11Buffer* const* GetVertexBuffer() const
	{
		return &VertexBufferHandle;
	}

	ID3D11Buffer* GetIndexBuffer() const
	{
		return IndexBufferHandle;
	}

	ID3D11VertexShader* GetVertexShader() const
	{
		return VertexShaderHandle;
	}

	ID3D11PixelShader* GetPixelShader() const
	{
		return PixelShaderHandle;
	}

	ID3D11InputLayout* GetInputLayout() const
	{
		return InputLayoutHandle;
	}
};

