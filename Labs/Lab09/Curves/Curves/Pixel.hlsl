struct pixelIn
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(pixelIn pIn) : SV_TARGET
{
	return pIn.Color;
}