/**********************************************************************************
// Timer (Código Fonte)
// 
// Criação:     02 Abr 2011
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Usa um contador de alta precisão para medir o tempo
//
**********************************************************************************/

#include "Timer.h"

// ------------------------------------------------------------------------------
// inicialização de membros estáticos

LARGE_INTEGER Timer::freq = { };    // frequência do contador

// ------------------------------------------------------------------------------

Timer::Timer()
{
    // inicializa frequência do contador apenas na primeira instanciação 
    if (!freq.QuadPart)
    {
        // pega frequência do contador de alta resolução
        QueryPerformanceFrequency(&freq);
    }

    // zera os valores de início e fim da contagem
    ZeroMemory(&start, sizeof(start));
    ZeroMemory(&end, sizeof(end));

    // timer em funcionamento
    stoped = false;
}

// ------------------------------------------------------------------------------

void Timer::Start()
{
    if (stoped)
    {
        // retoma contagem do tempo
        //
        //      <--- elapsed ---->
        // ----|------------------|------------> time
        //    start               end     
        //
        
        // tempo transcorrida antes da parada
        llong elapsed = end.QuadPart - start.QuadPart;
        
        // leva em conta tempo já transcorrido antes da parada
        QueryPerformanceCounter(&start); 
        start.QuadPart -= elapsed;

        // retoma contagem normal
        stoped = false;
    }
    else
    {
        // inicia contagem do tempo
        QueryPerformanceCounter(&start);
    }
}

// ------------------------------------------------------------------------------

void Timer::Stop()
{
    if (!stoped)
    {
        // marca o ponto de parada do tempo
        QueryPerformanceCounter(&end);
        stoped = true;
    }
}

// ------------------------------------------------------------------------------

double Timer::Reset()
{
    llong elapsed;

    if (stoped)
    {
        // pega tempo transcorrido antes da parada
        elapsed = end.QuadPart - start.QuadPart;
        
        // reinicia contagem do tempo
        QueryPerformanceCounter(&start); 
        
        // contagem reativada
        stoped = false;
    }
    else
    {
        // finaliza contagem do tempo
        QueryPerformanceCounter(&end);

        // calcula tempo transcorrido (em ciclos)
        elapsed = end.QuadPart - start.QuadPart;

        // reinicia contador
        start = end;
    }

    // converte tempo para segundos
    return elapsed / double(freq.QuadPart);    
}

// ------------------------------------------------------------------------------

llong Timer::Stamp()
{
    QueryPerformanceCounter(&end);
    return end.QuadPart;
}

// ------------------------------------------------------------------------------

double Timer::Elapsed()
{
    llong elapsed;

    if (stoped)
    {
        // pega tempo transcorrido até a parada
        elapsed = end.QuadPart - start.QuadPart;
    }
    else
    {
        // finaliza contagem do tempo
        QueryPerformanceCounter(&end);

        // calcula tempo transcorrido (em ciclos)
        elapsed = end.QuadPart - start.QuadPart;
    }

    // converte tempo para segundos
    return elapsed / double(freq.QuadPart);
}

// -------------------------------------------------------------------------------

double Timer::Elapsed(llong stamp)
{
    llong elapsed;

    if (stoped)
    {
        // pega tempo transcorrido até a pausa
        elapsed = end.QuadPart - stamp;

    }
    else
    {
        // finaliza contagem do tempo
        QueryPerformanceCounter(&end);

        // calcula tempo transcorrido (em ciclos)
        elapsed = end.QuadPart - stamp;
    }

    // converte tempo para segundos
    return elapsed / double(freq.QuadPart);
}

// -------------------------------------------------------------------------------