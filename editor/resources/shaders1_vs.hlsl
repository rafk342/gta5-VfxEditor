

struct VS_In
{
    float3 Pos : POSITION;
    float4 Color : COLOR0;
    float3 Normal : NORMAL;
};


struct VS_Out
{
    float4 Pos : SV_Position;
    float4 Color : COLOR0;
};

VS_Out main(VS_In input)
{
    VS_Out output;
    output.Pos = float4(input.Pos.x, input.Pos.y, 0.0f, 1.0f);
    output.Color = input.Color;    
    return output;
}
