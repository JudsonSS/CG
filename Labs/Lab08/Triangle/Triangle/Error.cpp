/**********************************************************************************
// Error (C�digo Fonte)
//
// Cria��o:		25 Jan 2020
// Atualiza��o:	29 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Uma classe para tratamento de erros
//
**********************************************************************************/

#include "Error.h"              // tratamento de erros
#include <comdef.h>             // defini��es COM para C++
#include <sstream>              // buffer string para texto
using std::stringstream;        // habilita o uso sem o prefixo std::

// -------------------------------------------------------------------------------

Error::Error() : code(S_OK), line(-1)
{
}

// -------------------------------------------------------------------------------

Error::Error(HRESULT hr, const string& funcName, const string& fileName, int lineNum) 
    : code(hr), function(funcName), line(lineNum)
{
    // posi��o da �ltima barra no nome do arquivo
    size_t pos = fileName.find_last_of('\\');

    // se a barra foi encontrada
    if (pos != string::npos)
    {
        // guarda apenas nome do arquivo e n�o o caminho completo
        file = fileName.substr(pos + 1);
    }
}

// -------------------------------------------------------------------------------

string Error::ToString() const
{
    // pega a descri��o do c�digo de erro COM em formato string
    _com_error err(code);
    
    stringstream text;
    text << function 
         << " falhou em " << file 
         << ", linha " << std::to_string(line)
         << ":\n" << err.ErrorMessage();

    return text.str();
}

// -------------------------------------------------------------------------------
