/**********************************************************************************
// Timer (Código Fonte)
// 
// Criação:		02 Abr 2011
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Usa um contador de alta precisão para medir o tempo.
//
**********************************************************************************/

#include "Timer.h"
#include "Types.h"


// ------------------------------------------------------------------------------

LARGE_INTEGER Timer::freq = { 0,0 };	// freqüência do contador

// ------------------------------------------------------------------------------

Timer::Timer()
{
	// pega freqüência do contador de alta resolução
	QueryPerformanceFrequency(&freq);

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
		//      <------ elapsed ------><----- stoped --->
		// ----|-----------------------|-----------------|----> time
		//    start                  stop              now
		//
		
		// tempo transcorrido antes da pausa
		llong elapsed = end.QuadPart - start.QuadPart;
		
		// leva em conta o tempo transcorrido antes da
		// parada como se ela nunca tivesse acontecido
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

float Timer::Reset()
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

		// reinicia a contagem do tempo
		start = end;
	}

	// converte tempo para segundos
	return float(elapsed / double(freq.QuadPart));	
}

// ------------------------------------------------------------------------------

float Timer::Elapsed()
{
	llong elapsed;

	if (stoped)
	{
		// pega tempo transcorrido até a parada do timer
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
	return float(elapsed / double(freq.QuadPart));
}

// -------------------------------------------------------------------------------