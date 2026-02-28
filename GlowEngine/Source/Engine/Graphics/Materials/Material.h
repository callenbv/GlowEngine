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
    class Material
    {

    public:
        
        std::string getName() { return name; }

        float specular = 0.f;
        float refraction = 1.f;
        float dissolve = 1.f;

        XMVector ambientColor = { 0.f,0.f,0.f };
        XMVector diffuseColor = { 0.f,0.f,0.f };
        XMVector specularColor = { 0.f,0.f,0.f };

    private:
        std::string name;
    };
}