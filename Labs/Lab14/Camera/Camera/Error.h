/**********************************************************************************
// Error (Arquivo de Cabeçalho)
//
// Criação:     25 Jan 2020
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Uma classe para tratamento de erros
//
**********************************************************************************/

#ifndef DXUT_ERROR_H
#define DXUT_ERROR_H

#include <windows.h>            // funções do windows
#include <string>               // tipo string
using std::string;              // uso de string sem std::

// ---------------------------------------------------------------------------------

class Error
{
private:
    HRESULT hrCode;             // código de erro do windows
    string funcName;            // nome da função em que ocorreu o erro
    string fileName;            // nome do arquivo em que ocorreu o erro
    int lineNum;                // número da linha do erro
    
public:
    Error();                    
    Error(HRESULT hr, const string & func, const string & file, int line);            
    string ToString() const;    
};

// ---------------------------------------------------------------------------------

// O código precisa ser uma macro porque o nome do arquivo, 
// linha e função precisam ser capturados no local do erro 

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                               \
{                                                                      \
    HRESULT hr = (x);                                                  \
    if(FAILED(hr)) { throw Error(hr, __func__, __FILE__, __LINE__); }  \
}
#endif

// ---------------------------------------------------------------------------------

#endif