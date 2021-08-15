/**********************************************************************************
// Vertex (Arquivo de Sombreamento)
//
// Cria��o:     22 Jul 2020
// Atualiza��o: 14 Ago 2021
// Compilador:  D3DCompiler
//
// Descri��o:   Um vertex shader simples que apenas passa a posi��o e cor
//              do v�rtice para frente sem realizar nenhuma transforma��o.
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

    // transforma a posi��o para o espa�o homog�neo de recorte (clip space)
    vout.PosH = float4(vin.PosL, 1.0f);

    // apenas passa a cor do v�rtice para o pixel shader
    vout.Color = vin.Color;

    return vout;
}