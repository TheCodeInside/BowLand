#include "DefaultShaderCommon.hlsli"

Texture2D    DiffuseMap     : register( t0 );
Texture2D    NormalMap      : register( t1 );
Texture2D    ShadowMap      : register( t2 );
SamplerState TextureSampler : register( s0 );
SamplerComparisonState ShadowSampler : register( s1 );

/// <summary>
/// Gets the color to be applied from a directional light.
/// </summary>
/// <param name="light">The light.</param>
float4 GetDirectionalLightColor( DirectionalLight light, float3 normal )
{
    float3 lightDirection = normalize( -light.Direction );
    float  lightAmount    = saturate( dot( normal, lightDirection ) );
    return light.DiffuseColor * lightAmount;
}

/// <summary>
/// Gets the normal from the normal map.
/// </summary>
/// <param name="normal">The current normal.</param>
/// <param name="tangent">The current tangent.</param>
/// <param name="uv">The UV coordinates.</param>
float3 GetNormalFromMap( float2 uv, float3 normal, float3 tangent )
{
    // Get the normal from the map
    float3 fromMap = NormalMap.Sample( TextureSampler, uv ).rgb * 2.0 - 1.0;

    // Calculate the TBN matrix to go from tangent space to world space
    float3 N = normal;
    float3 T = normalize( tangent - N * dot( tangent, N ) );
    float3 B = cross( T, N );
    float3x3 TBN = float3x3( T, B, N );

    return normalize( mul( fromMap, TBN ) );
}

/// <summary>
/// The entry point for the pixel shader.
/// </summary>
/// <param name="input">The pixel shader input data from the vertex shader.</param>
float4 main( VertexToPixel input ) : SV_TARGET
{
    input.Normal = normalize( input.Normal );
    input.Tangent = normalize( input.Tangent );

    // Re-set the normal
    input.Normal = lerp( input.Normal,
                         GetNormalFromMap( input.UV, input.Normal, input.Tangent ),
                         UseNormalMap );

    // Calculate the color from the directional light
    float4 dirLightColor = GetDirectionalLightColor( Light, input.Normal );

    // Get the texture color
    float4 textureColor = DiffuseMap.Sample( TextureSampler, input.UV );



    // Calculate this pixel's UV on the shadow map
    // This is where we'll sample to compare depths
    float2 shadowUV = input.ShadowPosition.xy / input.ShadowPosition.w * 0.5 + 0.5;
    shadowUV.y = 1.0f - shadowUV.y;

    // Calculate this pixel's depth from the light
    float depthFromLight = input.ShadowPosition.z / input.ShadowPosition.w - 0.001;

    // Sample the shadow map itself
    float shadowAdj = ShadowMap.SampleCmpLevelZero( ShadowSampler, shadowUV, depthFromLight );



    // Return the final lighted and textured color
    float4 litColor = ( dirLightColor + AmbientColor ) * textureColor;
    return litColor * shadowAdj;
}