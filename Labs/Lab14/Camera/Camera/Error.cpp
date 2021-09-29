/**********************************************************************************
// Error (Código Fonte)
//
// Criação:     25 Jan 2020
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Uma classe para tratamento de erros
//
**********************************************************************************/

#include "Error.h"              // tratamento de erros
#include <comdef.h>             // definições COM para C++
#include <sstream>              // buffer string para texto
using std::stringstream;        // habilita o uso sem o prefixo std::

// -------------------------------------------------------------------------------

Error::Error() 
    : hrCode(S_OK), lineNum(-1)
{
    // nomes da função e do arquivo vazios
}

// -------------------------------------------------------------------------------

Error::Error(HRESULT hr, const string & func, const string & file, int line) 
    : hrCode(hr), funcName(func), lineNum(line)
{
    // posição da última barra no nome do arquivo
    size_t pos = file.find_last_of('\\');

    // se a barra foi encontrada
    if (pos != string::npos)
    {
        // guarda apenas nome do arquivo e não o caminho completo
        fileName = file.substr(pos + 1);
    }
}

// -------------------------------------------------------------------------------

string Error::ToString() const
{
    // pega a descrição do código de erro COM em formato string
    _com_error err(hrCode);
    
    stringstream text;
    text << funcName 
         << " falhou em " << fileName 
         << ", linha " << std::to_string(lineNum)
         << ":\n" << err.ErrorMessage();

    return text.str();
}

// -------------------------------------------------------------------------------
