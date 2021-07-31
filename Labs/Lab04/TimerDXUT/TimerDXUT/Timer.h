/**********************************************************************************
// Timer (Arquivo de Cabeçalho)
// 
// Criação:		02 Abr 2011
// Atualização:	31 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Usa um contador de alta precisão para medir o tempo
//
**********************************************************************************/

#ifndef DXUT_TIMER_H
#define DXUT_TIMER_H

// -------------------------------------------------------------------------------

#include <windows.h>			// acesso ao contador de alta precisão do Windows

// -------------------------------------------------------------------------------

class Timer
{
private:
	LARGE_INTEGER start, end;	// valores de início e fim do contador
	LARGE_INTEGER freq;			// frequência do contador
	bool stoped;				// estado da contagem
	
public:
	Timer();					// construtor

	void  Start();				// inicia/retoma contagem do tempo
	void  Stop();				// pára contagem do tempo
	float Reset();				// reinicia contagem e retorna tempo transcorrido
	float Elapsed();			// retorna tempo transcorrido em segundos	
	bool  Elapsed(float secs);	// verifica se transcorreu "secs" segundos
}; 

// -------------------------------------------------------------------------------

// Funções Inline

inline bool Timer::Elapsed(float secs)
{ return (Elapsed() >= secs ? true : false); }

// -------------------------------------------------------------------------------

#endif