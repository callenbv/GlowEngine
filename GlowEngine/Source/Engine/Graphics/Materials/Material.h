/*
/
// filename: Material.h
// author: Callen Betts
// brief: defines the base material class
/
*/

#pragma once

namespace Graphics
{
    class Renderer;
}

namespace Materials
{
    // material buffer for passing data to the shader
    struct MaterialBufferCPU
    {
        DirectX::XMFLOAT4 baseColor;
        DirectX::XMFLOAT3 specularColor;
        float shininess;
        float useTexture;
        DirectX::XMFLOAT3 padding; // keep 16 byte allignment
    };

    class Material
    {

    public:
        
        std::string getName() { return name; }
        void setName(std::string name_) { name = name_; }

        float shininess = 0.f;
        float specular = 0.f;
        float refraction = 1.f;
        float dissolve = 1.f;
        Textures::Texture* diffuseTexture;

        Color ambientColor = { 0.f,0.f,0.f };
        Color diffuseColor = { 0.f,0.f,0.f };
        Color specularColor = { 0.f,0.f,0.f };

        // shader resource view 
        ID3D11ShaderResourceView* diffuseSRV = nullptr;

    private:
        std::string name;
    };
}