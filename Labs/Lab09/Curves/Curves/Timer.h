/**********************************************************************************
// Timer (Arquivo de Cabeçalho)
// 
// Criação:     02 Abr 2011
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Usa um contador de alta precisão para medir o tempo
//
**********************************************************************************/

#ifndef DXUT_TIMER_H
#define DXUT_TIMER_H

// -------------------------------------------------------------------------------

#include <windows.h>                          // acesso ao contador de alta precisão do Windows
#include "Types.h"                            // tipos específicos do motor

// -------------------------------------------------------------------------------

class Timer
{
private:
    static LARGE_INTEGER freq;                // frequência do contador
    LARGE_INTEGER start, end;                 // valores de início e fim do contador
    bool stoped;                              // estado da contagem
    
public:
    Timer();                                  // construtor

    void   Start();                           // inicia/retoma contagem do tempo
    void   Stop();                            // pára contagem do tempo
    double Reset();                           // reinicia contagem e retorna tempo transcorrido
    double Elapsed();                         // retorna tempo transcorrido em segundos    
    bool   Elapsed(double secs);              // verifica se transcorreu "secs" segundos

    llong  Stamp();                           // retorna valor atual do contador
    double Elapsed(llong stamp);              // retorna tempo transcorrido desde a marca
    bool   Elapsed(llong stamp, double secs); // testa se transcorreu o tempo desde a marca
}; 

// -------------------------------------------------------------------------------

// Funções Inline

inline bool Timer::Elapsed(double secs)
{ return (Elapsed() >= secs ? true : false); }

inline bool Timer::Elapsed(llong stamp, double secs)
{ return (Elapsed(stamp) >= secs ? true : false); }

// -------------------------------------------------------------------------------

#endif