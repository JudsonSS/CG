/**********************************************************************************
// Vertex (Arquivo de Sombreamento)
//
// Criação:     22 Jul 2020
// Atualização: 25 Set 2021
// Compilador:  D3DCompiler
//
// Descrição:   Um vertex shader simples que apenas passa a posição e cor
//              do vértice para frente sem realizar nenhuma transformação.
//
**********************************************************************************/

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

    // apenas adiciona a componente w = 1 em cada vértice
    vout.PosH = float4(vin.PosL, 1.0f);

    // apenas passa a cor do vértice para o pixel shader
    vout.Color = vin.Color;

    return vout;
}