/**********************************************************************************
// Error (Arquivo de Cabeçalho)
//
// Criação:     25 Jan 2020
// Atualização: 04 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:	Uma classe para tratamento de erros
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
    HRESULT hres_code;
    string func_name;
    string file_name;
    int line_num;
    
public:
    Error();
    Error(HRESULT hr, const string & func, const string & file, int line);
    string ToString() const;
};

// ---------------------------------------------------------------------------------

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                               \
{                                                                      \
    HRESULT hr = (x);                                                  \
    if(FAILED(hr)) { throw Error(hr, __func__, __FILE__, __LINE__); }  \
}
#endif

// ---------------------------------------------------------------------------------

#endif