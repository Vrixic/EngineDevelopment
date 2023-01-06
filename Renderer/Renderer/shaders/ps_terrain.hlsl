
struct PSIn
{
    float4 pos : SV_POSITION;
    float3 nrm : NORMAL;
    float2 uv : COLOR;
};

float4 main(PSIn inputVert) : SV_Target
{
    return float4(inputVert.nrm, 1.0f);
}