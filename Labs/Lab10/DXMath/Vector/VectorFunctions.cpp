#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// sobrecarrega operador << para usar cout com XMVECTOR
ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);

	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}

int main()
{
	cout.setf(ios_base::boolalpha);

	// verifica suporte a SSE2
	if (!XMVerifyCPUSupport())
	{
		cout << "DirectX Math não é suportado pela CPU" << endl;
		return 0;
	}

	XMVECTOR n = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	XMVECTOR v = XMVectorSet(-2.0f, 1.0f, -3.0f, 0.0f);
	XMVECTOR w = XMVectorSet(0.707f, 0.707f, 0.0f, 0.0f);

	// soma: XMVECTOR operator + 
	XMVECTOR a = u + v;

	// subtração: XMVECTOR operator - 
	XMVECTOR b = u - v;

	// multiplicação por escalar: XMVECTOR operator * 
	XMVECTOR c = 10.0f * u;

	// comprimento do vetor: ||u||
	XMVECTOR L = XMVector3Length(u);

	// normalização do vetor: u / ||u||
	XMVECTOR d = XMVector3Normalize(u);

	// produto escalar: u dot v
	XMVECTOR s = XMVector3Dot(u, v);

	// produto vetorial: u x v
	XMVECTOR e = XMVector3Cross(u, v);

	
	// o vetor n já está normalizado?
	bool equal = XMVector3Equal(n, XMVector3Normalize(n));
	
	// o ângulo entre os vetores n e w deve ser 45 graus
	XMVECTOR angleVec = XMVector3AngleBetweenVectors(n, w);
	float angleRadians = XMVectorGetX(angleVec);
	float angleDegrees = XMConvertToDegrees(angleRadians);

	cout << "u                   = " << u << endl;
	cout << "v                   = " << v << endl;
	cout << "w                   = " << w << endl;
	cout << "n                   = " << n << endl;
	cout << "a = u + v           = " << a << endl;
	cout << "b = u - v           = " << b << endl;
	cout << "c = 10 * u          = " << c << endl;
	cout << "d = u / ||u||       = " << d << endl;
	cout << "e = u x v           = " << e << endl;
	cout << "L = ||u||           = " << L << endl;
	cout << "s = u.v             = " << s << endl;
	cout << "n normalizado       = " << equal << endl;
	cout << "angle               = " << angleDegrees << endl;

	return 0;
}
