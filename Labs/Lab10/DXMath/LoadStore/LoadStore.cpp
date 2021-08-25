#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// sobrecarrega operador << para usar cout com XMVECTOR
ostream& XM_CALLCONV operator<<(ostream&, FXMVECTOR);

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
};

int main()
{
	// verifica suporte a SSE2
	if (!XMVerifyCPUSupport())
	{
		cout << "DirectX Math não é suportado pela CPU" << endl;
		return 0;
	}

	Vertex vertex { XMFLOAT3(2, 3, 4), XMFLOAT3(1, 1, 1) };

	XMVECTOR u = XMLoadFloat3(&vertex.pos);
	XMVECTOR v = XMLoadFloat3(&vertex.normal);
	XMVECTOR w = u + v;

	cout << "u = " << u << endl;
	cout << "v = " << v << endl;
	cout << "w = " << w << endl;
	
	return 0;
}

ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << "," << dest.y << "," << dest.z << ")";
	return os;
}