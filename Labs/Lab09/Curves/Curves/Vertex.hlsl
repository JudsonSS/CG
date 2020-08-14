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