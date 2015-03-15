
Texture2D<float4> tTexture;
sampler sSampler;

float4 main( float2 uv : TEXCOORD0 ) : SV_Target
{
    return tTexture.Sample(sSampler, uv);
}