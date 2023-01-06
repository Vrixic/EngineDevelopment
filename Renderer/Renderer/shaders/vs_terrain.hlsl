struct VSIn
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : UV;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 nrm : NORMAL;
    float2 uv : COLOR;
};

cbuffer MVP_t
{
    matrix modeling;
    matrix view;
    matrix projection;
};


VSOut main(VSIn inputVert)
{
    VSOut outputVert;
    outputVert.pos = float4(inputVert.pos, 1);
    outputVert.nrm = inputVert.nrm;
    outputVert.uv = inputVert.uv;
    
    outputVert.pos = mul(outputVert.pos, modeling);
    outputVert.pos = mul(outputVert.pos, view);
    outputVert.pos = mul(outputVert.pos, projection);
    
    return outputVert;
}