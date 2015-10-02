#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition - You will eventually ADD TO this!
// --------------------------------------------------------
struct Vertex
{
    DirectX::XMFLOAT3 Position;	    // The position of the vertex
    DirectX::XMFLOAT2 UV;           // The texture coordinates of the vertex
    DirectX::XMFLOAT3 Normal;       // The color of the vertex
};
