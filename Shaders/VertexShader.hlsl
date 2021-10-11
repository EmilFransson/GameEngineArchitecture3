cbuffer transforms : register(b0)
{
    float4x4 wvpMatrix;
};

struct VS_IN
{
    float3 inPositionLS : POSITION;
    float2 inTexCoords  : TEXCOORD;
};

struct VS_OUT
{
    float4 outPositionCS    : SV_Position;
    float2 outTexCoords     : TEXCOORD;     
};

VS_OUT vs_main(VS_IN vsIn)
{
    VS_OUT vsOut = (VS_OUT)0;
    
    vsOut.outPositionCS = mul(float4(vsIn.inPositionLS, 1.0f), wvpMatrix);
    vsOut.outTexCoords = vsIn.inTexCoords;
    
    return vsOut;
}