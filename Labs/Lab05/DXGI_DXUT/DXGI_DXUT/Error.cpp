/**********************************************************************************
// Error (C�digo Fonte)
//
// Cria��o:     25 Jan 2020
// Atualiza��o: 04 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Uma classe para tratamento de erros
//
**********************************************************************************/

#include "Error.h"              // tratamento de erros
#include <comdef.h>             // defini��es COM para C++
#include <sstream>              // buffer string para texto
using std::stringstream;        // habilita o uso sem o prefixo std::

// -------------------------------------------------------------------------------

Error::Error() 
    : hres_code(S_OK), line_num(-1)
{
    // nomes da fun��o e do arquivo vazios
}

// -------------------------------------------------------------------------------

Error::Error(HRESULT hr, const string & func, const string & file, int line) 
    : hres_code(hr), func_name(func), line_num(line)
{
    // posi��o da �ltima barra no nome do arquivo
    size_t pos = file.find_last_of('\\');

    // se a barra foi encontrada
    if (pos != string::npos)
    {
        // guarda apenas nome do arquivo e n�o o caminho completo
        file_name = file.substr(pos + 1);
    }
}

// -------------------------------------------------------------------------------

string Error::ToString() const
{
    // pega a descri��o do c�digo de erro COM em formato string
    _com_error err(hres_code);
    
    stringstream text;
    text << func_name 
         << " falhou em " << file_name 
         << ", linha " << std::to_string(line_num)
         << ":\n" << err.ErrorMessage();

    return text.str();
}

// -------------------------------------------------------------------------------
