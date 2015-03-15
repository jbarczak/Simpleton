

float4x4 g_World;
float4x4 g_ViewProj;

struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 Pos : SV_POSITION;
};

VSOut main( float4 p : POSITION, float2 uv : TEXCOORD0 )
{
    VSOut o;
    o.uv  = uv;
    o.Pos = mul( g_ViewProj, mul( g_World, p ) );
    return o;
}