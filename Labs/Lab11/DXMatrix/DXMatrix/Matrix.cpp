#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// sobrecarrega operador << para usar cout com XMVECTOR
ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v)
{
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);

	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}

// sobrecarrega operador << para usar cout com XMMATRIX
ostream& XM_CALLCONV operator << (ostream& os, FXMMATRIX m)
{
	for (int i = 0; i < 4; ++i)
	{
		os << left; os.width(5); os << XMVectorGetX(m.r[i]) << ' ';
		os << left; os.width(5); os << XMVectorGetY(m.r[i]) << ' ';
		os << left; os.width(5); os << XMVectorGetZ(m.r[i]) << ' ';
		os << left; os.width(5); os << XMVectorGetW(m.r[i]) << ' ';
		os << '\n';
	}
	return os;
}

int main()
{
	// verifica suporte a SSE2
	if (!XMVerifyCPUSupport())
	{
		cout << "DirectX Math não é suportado pela CPU" << endl;
		return 0;
	}

	XMMATRIX A(1.0f, 0.0f, 0.0f, 0.0f,
		       0.0f, 2.0f, 0.0f, 0.0f,
		       0.0f, 0.0f, 4.0f, 0.0f,
		       1.0f, 2.0f, 3.0f, 1.0f);

	XMMATRIX B = XMMatrixIdentity();

	// multiplicação de matrizes
	XMMATRIX C = A * B;

	// calculando a transposta
	XMMATRIX D = XMMatrixTranspose(A);

	// calculando a inversa
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX E = XMMatrixInverse(&det, A);

	XMMATRIX F = A * E;

	cout << "A = " << endl << A << endl;
	cout << "B = " << endl << B << endl;
	cout << "C = A*B = " << endl << C << endl;
	cout << "D = Transposta(A) = " << endl << D << endl;
	cout << "det = Determinante(A) = " << endl << det << endl << endl;
	cout << "E = Inversa(A) = " << endl << E << endl;
	cout << "F = A*E = " << endl << F << endl;

	return 0;
}
