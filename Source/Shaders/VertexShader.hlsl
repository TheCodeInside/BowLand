#include "DirectionalLight.hpp"

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    DirectionalLight light0;
    DirectionalLight light1;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
    // Data type
    //  |
    //  |   Name          Semantic
    //  |    |                |
    //  v    v                v
    float3 position		: SV_POSITION;
    float2 uv           : TEXCOORD;
    float3 normal       : NORMAL;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
    // Data type
    //  |
    //  |   Name          Semantic
    //  |    |                |
    //  v    v                v
    float4 position		: SV_POSITION;
    float2 uv           : TEXCOORD;
    float3 normal       : NORMAL;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
    // Set up output struct
    VertexToPixel output;
    
    // The vertex's position (input.position) must be converted to world space,
    // then camera space (relative to our 3D camera), then to proper homogenous 
    // screen-space coordinates.  This is taken care of by our world, view and
    // projection matrices.  
    //
    // First we multiply them together to get a single matrix which represents
    // all of those transformations (world to view to projection space)
    matrix worldViewProj = mul(mul(world, view), projection);

    // Then we convert our 3-component position vector to a 4-component vector
    // and multiply it by our final 4x4 matrix.
    //
    // The result is essentially the position (XY) of the vertex on our 2D 
    // screen and the distance (Z) from the camera (the "depth" of the pixel)
    output.position = mul( float4( input.position, 1.0f ), worldViewProj );

    // Pass the UV and normal through
    output.uv = input.uv;
    output.normal = normalize( mul( input.normal, (float3x3)world ) );

    // Whatever we return will make its way through the pipeline to the
    // next programmable stage we're using (the pixel shader for now)
    return output;
}