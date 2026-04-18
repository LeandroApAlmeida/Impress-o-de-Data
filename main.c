#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <time.h>


#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"




// Esta função imprime a data atual do sistema usando a linguagem C padrão (ANSI C) 
// (multiplataforma).

void printDateTimeAnsiC() {
	
	// Armazena o tempo em milissegundos desde 01/01/1970.
    time_t currentTime;
    
    // Struct que contém os campos da data (dia, mês, ano, etc.)
	struct tm *info_tempo;

    // Obtém o tempo atual do sistema.
	time(&currentTime);

    // Converte para a hora local (ajustando para o fuso horário local).
	info_tempo = localtime(&currentTime);
    
    // Imprime a data e a hora atual.

    printf("FUNCAO 1: DATA E HORA LOCAL EM ANSI C (CODIGO MULTIPLATAFORMA)\n\n");

    printf(
		YELLOW"[ -- Data/Hora local: %02d/%02d/%04d %02d:%02d:%02d -- ]"RESET, 
        info_tempo->tm_mday, 
        info_tempo->tm_mon + 1, 
        info_tempo->tm_year + 1900,
        info_tempo->tm_hour, 
        info_tempo->tm_min, 
        info_tempo->tm_sec
    );
    
}




// Esta função imprime a data atual do sistema usando chamada à API do Windows (funciona
// somente em sistema Windows).

void printDateTimeWin32Api() {

    // Struct que contém os campos de uma data (dia, mês, ano, horas, minutos, segundos,
	// milisegundos).
    SYSTEMTIME st;

    // Chamada à API do Windows para obter o tempo atual do sistema.
    GetLocalTime(&st);
    
    // Imprime a data e a hora atual.
    
    printf("FUNCAO 2: DATA E HORA LOCAL VIA API WIN32 (WINDOWS)\n\n");

    printf(
		YELLOW"[ -- Data/Hora local: %02d/%02d/%04d %02d:%02d:%02d -- ]"RESET, 
        st.wDay,
		st.wMonth,
		st.wYear,
		st.wHour,
		st.wMinute,
		st.wSecond
    );

    return 0;
}




// Esta função imprime a data atual do sistema usando chamada à API do Windows usando código
// assembly (funciona somente em sistemas Windows).

void printDateTimeWin32ApiAsm() {
	
	// Executa o assembly (na sintaxe AT&T do DEV C++) que vai fazer a chamada à API do
	// Windows em baixo nível para obter a data. Estou executando assembly para Windows
	// x64, para processadores de 64 bits, e também para Windows x86, para processadores
	// de 32 bits.
    
    // Struct que contém os campos de uma data (dia, mês, ano, horas, minutos, segundos,
	// milisegundos).
	SYSTEMTIME st;
	
	// Ao gerar o código binário, o compilador vai detectar para qual arquitetura ele 
	// será gerado (Windows x64 ou Windows x86) para ler o respectivo assembly.

    #ifdef _WIN64
    
        // Assembly para Windows x64 (processadores de 64 bits)
        
        __asm__ (
            
			"lea %0, %%rcx\n\t"      // O compilador substitui %0 pelo endereço da struct
			                         // "st" e coloca no registrador RCX. Em x64, o primeiro
									 // argumento de uma função deve estar obrigatoriamente
									 // no registrador RCX. 
            
			"sub $32, %%rsp\n\t"     // Reserva o Shadow Space. É uma exigência do Windows
			                         // que se reserve 32 bytes na pilha antes de qualquer
									 // chamada (call), mesmo que a função não precise 
									 // deles.
            
			"call GetLocalTime\n\t"  // Chama a função GetLocalTime da API do Windows.
            
			"add $32, %%rsp\n\t"     // Após a função retornar, "limpa" a pilha, devolvendo
			                         // os 32 bytes reservados ao Shadow Space.
			                         
			                         // Lista de operandos para o compilador GCC:
			                         // -----------------------------------------
            
			: "=m" (st)              // Lista de saídas: %0 corresponde a "st".
            
			:                        // Lista de entradas: vazia
            
			: "rcx", "memory"        // Clobber list: Avisa o compilador GCC que vai alterar
			                         // o registrador RCX e a memória.
        
		);
        
    #else
    
        // Assembly para Windows x86 (processadores de 32 bits)
    
	    __asm__ (
    
	        "push %0\n\t"              // Coloca o valor de %0 no topo da pilha (Stack).
			                           // O compilador substitui %0 pelo endereço da 
									   // struct "st".
    
	        "call _GetLocalTime@4\n\t" // Chama a função GetLocalTime da API do Windows,
			                           // conforme o padrão do stdcall 32 bits.
			                           
			                           // Lista de operandos para o compilador GCC:
			                           // -----------------------------------------
    
	        :                          // Lista de saídas: vazia
    
	        : "r" (&st)                // Lista de entradas: %0 corresponde a "st".
    
	        : "memory"                 // Clobber list: Avisa o compilador GCC que vai alterar
			                           // a memória.
    
	    );
    
	#endif

    // Imprime a data e a hora atual.

    printf("FUNCAO 3: DATA E HORA LOCAL VIA API WIN32 E ASSEMBLY (WINDOWS)\n\n");

    printf(
		YELLOW"[ -- Data/Hora local: %02d/%02d/%04d %02d:%02d:%02d -- ]"RESET,
	 	st.wDay,
		st.wMonth,
		st.wYear,
		st.wHour,
		st.wMinute,
		st.wSecond
	);
    
}




// Esta função obtém a data do sistema lendo-a diretamente da memória do computador e 
// imprime no terminal, no tempo UTC e tempo local.
//
//   ------------------------------------------------------------------------------
//   Nota: Antes de imprimir a data/hora no tempo UTC e local, será impresso uma 
//   tabela com os valores lidos da memória, em cor verde, para que possam repetir 
//   o cálculo que eu utilizei dentro desta função para conferir.
//   ------------------------------------------------------------------------------
//
// Este modo mais direto de obter a data depende de cálculos para converter a data do
// formato de representação utilizado pelo Windows para o formato utilizado pelo Unix, 
// para daí se usar as funções padrão das bibliotecas em C que extraem os campos desta 
// (ano, mês, dia, hora, etc), para imprimi-los no terminal. 
//
// No Windows, o valor 0x00000...000 do inteiro sem sinal de 64 bits que guarda a data 
// representa o dia 01/01/1601 às 00:00:00, e a contagem de tempo é feita pelo número 
// de intervalos de 100 nanossegundos que se passaram desde esse instante (quem tiver 
// curiosidade de saber o  porquê o Windows conta a data a partir de 1601, consulte a 
// internet). Porém, as bibliotecas  padrão em C contam o tempo no padrão Unix Epoch, 
// dos sistemas UNIX. No Unix Epoch, a faixa positiva do inteiro com sinal que guarda a
// data se inicia no dia 01/01/1970 às 00:00:00 (UTC) e a contagem do tempo é feita pelo
// número de segundos que se passaram a partir deste instante (datas anteriores a 1970
// são representadas por valores na faixa negativa do inteiro assinado). 
// 
// Na memória do computador, a data estará representada no formato como o Windows utiliza,
// chamado de Windows FILETIME, e estará disposta para acesso de somente leitura numa 
// área compartilhada com todos os processos denominada de KUSER_SHARED_DATA. A área de memória
// KUSER_SHARED_DATA contém inúmeros dados sobre o sistema Windows, e entre estes dados está
// a data atual, atualizada pelo sistema ao toque de um "relógio" a cada 15,625 milissegundos,
// e o time zone, a diferença entre o UTC e a hora local, que só é escrito no boot ou por
// alguma mudança no fuso horário feita pelo usuário ou pelo próprio Windows. São os dois
// campos de interesse nos cálculos realizados nesta função.
//
// KUSER_SHARED_DATA está no endereço (virtual) fixo 0x7FFE0000, mapeado no espaço de 
// memória de todos os programas do Windows (todos apontam para o mesmo endereço na memória
// física). A data atual do sistema estará no offset 0x014 desta área de memória, denominado
// SystemTime, com 12 bytes. O time zone do local está no offset 0x020, denominado TimeZoneBias,
// também com 12 bytes.
//
// Traduzindo para endereços absolutos de SystemTime e TimeZoneBias com relação ao endereço
// fixo de KUSER_SHARED_DATA temos:
//
//
//   0x7FFE0000      0x7FFE0014                         0x7FFE0020
//   -------------------------------------------------------------------------------------
//   |      ...      | SystemTime (12B) |      ...      | TimeZoneBias (12B) |      ...
//   -------------------------------------------------------------------------------------
//
//   |............................ KUSER_SHARED_DATA .....................................
//
//
//   * SystemTime (12 bytes): Endereço 0x7FFE0014 (offset 0x014).
//
//   * TimeZoneBias (12 bytes): Endereço 0x7FFE0020 (offset 0x020).
//
//
// Para escrever a data do sistema em SystemTime pela primeira vez durante o boot, o Windows
// faz a leitura do RTC (Real-Time Clock), um chip da placa-mãe que mantém a data atualizada,
// mesmo quando o computador está desligado da tomada. O Windows não fica consultando o
// RTC o tempo todo, pois o acesso ao barramento custa muito caro em termos de número de
// ciclos de instrução. Ele recupera o valor da data durante o boot, ao sair da suspensão,
// ou ajustar a data com o serviço W32Time, copia este valor na memória em SystemTime, e
// a partir disso, ele usa um chip chamado de TSC (Time Stamp Counter), que fica dentro
// da CPU, junto com HPET (High Precision Event Timer), um chip que fica na placa-mãe 
// usado para calibrar o TSC, para ficar atualizando esta data a um intervalo pré-definido
// de 15,625 milissegundos (64 Hz). 
//
// O kernel configura um timer de hardware (geralmente o APIC Timer local de cada núcleo
// da CPU) para marcar este intervalo de 15,625 ms. A cada tick, o hardware lança uma  
// interrupção. O Windows interrompe o que quer que esteja fazendo e executa o Clock 
// Interrupt Handler. Nesse  momento, o kernel lê o TSC para saber quantos ciclos passaram
// desde a última interrupção e atualiza a data em SystemTime a partir deste cálculo. Como
// o valor de data é de 64 bits e a memória pode ser lida por vários programas ao mesmo
// tempo, o  Windows grava a data em uma ordem específica para evitar que o programa leia
// "metade do valor antigo" e "metade do valor novo" da data, causando inconsistência.
// Para entender a ordem de gravação da data, analizemos com mais detalhes a memória em
// SystemTime.
//
// Como visto anteriormente, ele ocupa 12 bytes, iniciando a partir do endereço 0x7FFE0014
// em KUSER_SHARED_DATA. É composto por três slots de 32 bits cada (4 bytes):
//
//
//               0x7FFE0014        0x7FFE0018         0x7FFE001C
//               -------------------------------------------------------
//               | LowPart (32b)   | High1Time (32b) | High2Time (32b) |     
//               -------------------------------------------------------
//
//               | ................... SystemTime .....................|
//
//
//   * LowPart (32 bits): Endereço 0x7FFE0014. São os bits baixos da data (dígitos menos 
//     significativos).
//
//   * High1Time (32 bits): Endereço 0x7FFE0018. São os bits altos da data (dígitos mais 
//     significativos). 
//
//   * High2Time (32 bits): Endereço 0x7FFE001C. É a cópia dos 32 bits altos da data.
//
//
// Considerando a data inicial 0x01D17E3F80824000 (Valor numérico de 16/03/2026, 00:00:00
// em Windows FILETIME), o valor em SystemTime é:
//
//
//                   Endereço         Conteúdo           Slot
//                   ---------------------------------------------
//                   0x7FFE0014       00 40 82 80        LowPart
//                   0x7FFE0018       3F 7E D1 01        High1Time
//                   0x7FFE001C       3F 7E D1 01        High2Time
//
//   * A ordem dos bytes é gravada "invertida" pois a arquitetura Intel/AMD armazena o 
//     byte menos significativo no endereço de memória mais baixo (Little-Endian).
//
//
// Ao mudar a data, a ordem de gravação destes slots pelo Windows é a seguinte:
//
//
//    1. High2Time: O kernel grava primeiro o novo valor da parte alta da data no terceiro
//       slot de SystemTime.
//
//    2. LowPart: O kernel grava a nova parte baixa da data no primeiro slot.
//
//    3. High1Time: O kernel grava o novo valor da parte alta no segundo slot.
//
//
// Supondo que a próxima data calculada 15.625 ms depois (próxima interrupção) seja 
// 0x01D17E3F8084A25A. A memória em SystemTime será escrita desta forma para atualizar 
// o novo valor calculado:
//
//
//    1. High2Time = 3F 7E D1 01
//
//    2. LowPart = 5A A2 84 80
//
//    3. High1Time = 3F 7E D1 01
//
//
// Se houver uma interrupção e o kernel começar a gravar enquanto o programa está lendo,
// o High2Time será atualizado primeiro. Quando o programa terminar de ler e comparar, 
// High1Time (antigo) poderá ser diferente de High2Time (novo). Isso só ocorre em um tick
// a cada 7 minutos. Dessa forma, criamos um loop do-while que detecta a diferença e tenta
// ler novamente até pegar o valor estável. 
//
// Para implementar esta lógica, o programa deve ler SystemTime na seguinte ordem dentro
// do loop:
//
//
//    1. Lê High1Time.
//
//    2. Lê LowPart.
//
//    3. Lê High2Time.
//
//
// Apesar de o valor de time zone ser escrito durante o boot, com a leitura do registro
// do Windows, e só mudar se o usuário trocar o fuso horário, usamos a mesma lógica para 
// a leitura de seus slots, que são os mesmos de SystemTime, e são atualizados pelo kernel 
// na exata ordem deste, devendo a leitura dos slots no loop do-while do programa ser 
// realizada exatamente como feita para obter a data.
//
// Obtido o valor da data e do time zone de SystemTime e TimeZoneBias, respectivamente, 
// a próxima etapa consiste em realizar algumas conversões e imprimir a data recuperada
// em tempo UTC e tempo local (dependente de fuso horário). A lógica matemática aplicada 
// nestas conversões é a seguinte:
//
// 1. Recomposição de 64 bits: Como o Windows divide o tempo em dois slots de 32 bits para
// manter a compatibilidade com sistemas legados e evitar leituras inconsistentes, o código
// realiza um deslocamento de bits (<< 32) no valor alto (High) e o combina com o valor 
// baixo (Low) usando o operador OR (|). Isso resulta em um valor de 64 bits que representa
// o total de Ticks (intervalos de 100 nanossegundos) desde 1º de janeiro de 1601 (lembre-se,
// a data em SystemTime usa a representação Windows FILETIME).
//
// 2. Cálculo do Tempo Local (Bias): O Windows armazena o fuso horário como um "Bias" 
// (deslocamento). Diferente de outras APIs que somam o fuso, o kernel do Windows define
// o Bias como a quantidade que deve ser subtraída do UTC para obter o tempo local 
// (Local = UTC - Bias). Por exemplo, se o fuso é UTC-3, o Bias será um valor positivo 
// equivalente a 3 horas; ao subtraí-lo do UTC, chegamos ao horário local correto.
//
// 3. Conversão de Windows FILETIME para Unix Epoch: Divide-se o valor por 10.000.000 para
// converter a unidade de "centenas de nanossegundos" para "segundos", depois subtrai-se
// a constante de 11.644.473.600 segundos, que representa o tempo decorrido entre a época
// do Windows (1601) e a época do Unix (1970).
//
// O resultado final é o valor em Unix Epoch, que pode ser usado por funções padrão da 
// biblioteca C, como gmtime, para extrair o dia, mês, ano e horário."

void printDateTimeMemory() {
	
	// Struct que representa os 3 slots de 32 bits de SystemTime e TimeZoneBias.
	
	typedef struct {
		
	    uint32_t LowPart;    // LowPart (32 bits baixos)  
	    
		int32_t  High1Time;  // High1Time (32 bits altos):
	    
		int32_t  High2Time;  // High2Time (Cópia dos 32 bits altos):
	
	} KSYSTEM_TIME;
	
    // Endereços em KUSER_SHARED_DATA (Mapeada em 0x7FFE0000):
    
    // Ponteiro para o endereço de memória de SystemTime (Offset 0x014).
	volatile KSYSTEM_TIME* pSystemTime = (KSYSTEM_TIME*)0x7FFE0014;
	
	// Ponteiro para o endereço de memória de TimeZoneBias (Offset 0x020).
    volatile KSYSTEM_TIME* pTimeZoneBias = (KSYSTEM_TIME*)0x7FFE0020;

    uint32_t low, bLow;
    int32_t  h1, h2, bh1, bh2;
    
    // Faz a leitura de SystemTime:
	//
    //    1. Lê High1Time.
	//
	//    2. Lê LowPart.
	//
	//    3. Lê High2Time.
	// 
	// Se High1Time != High2Time, significa que o Windows atualizou a hora enquanto
	// estava lendo. O programa deve descartar o valor e ler de novo, até que ambos os
	// valores (High1Time e High2Time) sejam iguais.
	
	do {

	    h1 = pSystemTime->High1Time;
	
	    _ReadBarrier();
	    MemoryBarrier();

	    low = pSystemTime->LowPart;
	
	    _ReadBarrier();
	    MemoryBarrier();

	    h2 = pSystemTime->High2Time;
	
	} while (h1 != h2);
	
	// Faz a leitura de TimeZoneBias:
	//
    //    1. Lê High1Time.
	//
	//    2. Lê LowPart.
	//
	//    3. Lê High2Time.
	// 
	// Se High1Time != High2Time, significa que o Windows atualizou a time zone enquanto
	// estava lendo. O programa deve descartar o valor e ler de novo, até que ambos os 
	// valores (High1Time e High2Time) sejam iguais.

    do {
    	
        bh1 = pTimeZoneBias->High1Time;
        
        _ReadBarrier();
	    MemoryBarrier();
    
	    bLow = pTimeZoneBias->LowPart;
	    
	    _ReadBarrier();
	    MemoryBarrier();
    
	    bh2 = pTimeZoneBias->High2Time;
    
	} while (bh1 != bh2);

    // Obtém os 64 bits da data em formato Windows FILETIME a partir da junção dos 2 
	// slots de 32 bits High1Time e LowPart, respectivamente:
    //
    //     T_utc = (H_system * 2^32) + L_system
    //     T_bias = (H_bias * 2^32) + L_bias
    //     T_local = T_utc - T_bias
    //
    // Onde:
    //
	//     T: Representa o tempo em Windows FILETIME.
	//
	//     H e L: São as partes High (High1Time) e Low (LowPart) lidas de SystemTime.
    
	uint64_t utcTicks = ((uint64_t)h1 << 32) | low;
    
	uint64_t biasTicks = ((uint64_t)bh1 << 32) | bLow;
    
	uint64_t localTicks = utcTicks - biasTicks;

	// Converte o formato de data Windows FILETIME (que começa em 1601) para o formato
	// Unix Epoch (que começa em 1970).
	//
	//     U_utc = (T_utc / 10000000) - DIFF_1601_TO_1970
	//     U_local = (T_local / 10000000 - DIFF_1601_TO_1970
	// 
	// Onde:
	//
	//     10000000 é o divisor para converter centenas de nanossegundos (Windows) em 
	//     segundos (Unix).
	//
	//     DIFF_1601_TO_1970 = 11.644.473.600 segundos passados entre 01/01/1601 (Windows
	//     FILETIME) e 01/01/1970 (Unix Epoch).

	const uint64_t DIFF_1601_TO_1970 = 11644473600ULL; 
    
    time_t unixUTC = (time_t)((utcTicks / 10000000) - DIFF_1601_TO_1970);
    
	time_t unixLocal = (time_t)((localTicks / 10000000) - DIFF_1601_TO_1970);
	
	// Obtém a data em formato UTC:

    struct tm *info_utc = gmtime(&unixUTC);    
    struct tm tm_utc = *info_utc; 
    
    // Obtém a data em formato local:
    
    struct tm *info_local = gmtime(&unixLocal);    
    struct tm tm_local = *info_local;

	// Imprime a data e a hora atual. Antes de imprimir a data, exibe os valores recuperados
	// com a leitura de SystemTime (Higth1Time e LowPart) e TimeZoneBias.

    printf("FUNCAO 4: DATA E HORA VIA ACESSO A KUSER_SHARED_DATA (WINDOWS)\n\n");

	printf(GREEN"--------------------------------------------------------------\n"RESET);
	printf(GREEN"Endereco      Descricao            Dados\n"RESET);
	printf(GREEN"--------------------------------------------------------------\n"RESET);
    printf(GREEN"0x7FFE0000    KUSER_SHARED_DATA    \n"RESET);
    printf(GREEN"0x7FFE0014    SystemTime UTC       H: 0x%08X L: 0x%08X\n"RESET, h1, low);
    printf(GREEN"0x7FFE0020    Time Zone Bias       Ticks: %llu\n"RESET, biasTicks);
    printf(GREEN"--------------------------------------------------------------\n\n"RESET);

    printf(
		YELLOW"[ -- Data/Hora UTC:   %02d/%02d/%04d %02d:%02d:%02d -- ]\n"RESET, 
        tm_utc.tm_mday, 
		tm_utc.tm_mon + 1, 
		tm_utc.tm_year + 1900,
        tm_utc.tm_hour, 
		tm_utc.tm_min,
		tm_utc.tm_sec
	);

    printf(
		YELLOW"[ -- Data/Hora local: %02d/%02d/%04d %02d:%02d:%02d -- ]\n"RESET, 
        tm_local.tm_mday, 
		tm_local.tm_mon + 1,
		tm_local.tm_year + 1900,
        tm_local.tm_hour,
		tm_local.tm_min,
		tm_local.tm_sec
	);

}




// Ponto de entrada do programa.

int main() {
	
	printDateTimeAnsiC();
	
	printf("\n\n");
	
	printDateTimeWin32Api();
	
	printf("\n\n");
	
	printDateTimeWin32ApiAsm();
	
	printf("\n\n");

	printDateTimeMemory();

    return 0;
    
}
