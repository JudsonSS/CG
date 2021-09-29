/**********************************************************************************
// Vertex (Arquivo de Sombreamento)
//
// Cria��o:     22 Jul 2020
// Atualiza��o: 25 Set 2021
// Compilador:  D3DCompiler
//
// Descri��o:   Um vertex shader simples que apenas passa a posi��o e cor
//              do v�rtice para frente sem realizar nenhuma transforma��o.
//
**********************************************************************************/

cbuffer cbPerObject : register(b0)
{
    float4x4 WorldViewProj;
};

struct VertexIn
{
    float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;

    // transforma para espa�o homog�neo de recorte
    vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProj);

    // apenas passa a cor do v�rtice para o pixel shader
    vout.Color = vin.Color;

    return vout;
}