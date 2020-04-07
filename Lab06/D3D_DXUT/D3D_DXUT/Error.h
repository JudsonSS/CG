/**********************************************************************************
// Error (Arquivo de Cabe�alho)
//
// Cria��o:		25 Jan 2020
// Atualiza��o:	29 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Uma classe para tratamento de erros
//
**********************************************************************************/

#ifndef DXUT_ERROR_H
#define DXUT_ERROR_H

#include <windows.h>
#include <string>
using std::string;

// ---------------------------------------------------------------------------------

class Error
{
private:
    HRESULT code;
    string function;
    string file;
    int line;
    
public:
    Error();
    Error(HRESULT hr, const string& funcName, const string& fileName, int lineNum);
    string ToString() const;
};

// ---------------------------------------------------------------------------------

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                         \
{                                                                \
    HRESULT hr = (x);                                            \
    if(FAILED(hr)) { throw Error(hr, __func__, __FILE__, __LINE__); }  \
}
#endif

// ---------------------------------------------------------------------------------

#endif