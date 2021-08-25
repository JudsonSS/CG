#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// sobrecarrega operador << para usar cout com XMVECTOR
ostream& XM_CALLCONV operator<<(ostream&, FXMVECTOR);

int main()
{
	cout.setf(ios_base::boolalpha);

	// verifica suporte a SSE2
	if (!XMVerifyCPUSupport())
	{
		cout << "DirectX Math não é suportado pela CPU" << endl;
		return 0;
	}

	XMVECTOR p = XMVectorZero();
	XMVECTOR q = XMVectorSplatOne();
	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	XMVECTOR v = XMVectorReplicate(-2.0f);
	XMVECTOR w = XMVectorSplatZ(u);

	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "u = " << u << endl;
	cout << "v = " << v << endl;
	cout << "w = " << w << endl;

	return 0;
}

ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << "," << dest.y << "," << dest.z <<  ")";
	return os;
}