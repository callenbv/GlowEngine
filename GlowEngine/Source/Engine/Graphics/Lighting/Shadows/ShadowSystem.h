/*
/
// filename: ShadowSystem.h
// author: Callen Betts
// brief: defines ShadowSystem class
/
*/

#pragma once
#include "Engine/Graphics/Buffers/ConstantBuffer.h"

namespace Lighting
{
	class ShadowSystem : public Systems::System
	{

	public:

		ShadowSystem(ID3D11Device* device_, ID3D11DeviceContext* deviceContext);

		void DrawShadow(const Vector3D& position, Vector3D scale);

	private:

		ID3D11DeviceContext* context;
		ID3D11Device* device;
		ID3D11Buffer* transformBuffer;
		Graphics::ConstantBuffer<cbPerObject>* buffer;
		int shadowIndexCount;

		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		std::vector<Vertex> vertices;
		std::vector<int> indices;
		float angleInRadians = DirectX::XMConvertToRadians(90.0f);

	};
}