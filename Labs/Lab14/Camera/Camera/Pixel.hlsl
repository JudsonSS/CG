/**********************************************************************************
// Pixel (Arquivo de Sombreamento)
//
// Cria��o:     22 Jul 2020
// Atualiza��o: 14 Ago 2021
// Compilador:  D3DCompiler
//
// Descri��o:   Um pixel shader simples que apenas passa a cor do pixel
//              para frente sem realizar nenhuma modifica��o.
//
**********************************************************************************/

struct pixelIn
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(pixelIn pIn) : SV_TARGET
{
    return pIn.Color;
}