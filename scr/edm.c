/**
 * \file edm.c
 * \author LUcio Pintanel
 * \date 9 Aug 2016
 * \brief File containing includes, defines, and demias externados.
 *
 */

/***	SESSION INCLUDE	***********************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "edm.h"

/**********************************************************************/

/***	SESSION DEFINE, STRUCT, TYPEDEF	*******************************/
#define VERSAO_MAX _VERSAO_MAX  /*!< Definicao de versao Maxima, valor da definicao vem via Makefile */
#define VERSAO_MIN _VERSAO_MIN  /*!< Definicao de versao Minima, valor da definicao vem via Makefile */
#define VERSAO_NRO _VERSAO_NRO  /*!< Definicao de versao Númeor, valor da definicao vem via Makefile */

#define MAX_TAM 256UL /*!< Define o tamanho da tabela de valores para calculo mais rápido do CRC */

//16 BITS
#define CRC16_CITT_POLY     0x1021UL    /*!< Definicao do polinomio padrao para calculo de CRC16 CITT */
#define CRC16_XMODEM_POLY   0x8408UL    /*!< Definicao do polinomio padrao para calculo de CRC16 XMODEM */
#define CRC16_ARC_POLY     0x8005UL     /*!< Definicao do polinomio padrao para calculo de CRC16 ARC */
#define CRC16_DNP     0xA6BCUL          /*!< Definicao do polinomio padrao para calculo de CRC16 DNP */
#define CRC32      0x04C11DB7UL         /*!< Definicao do polinomio padrao para calculo de CRC32 */

#define BITMASK(X) (1L << (X))  /*!< Definicao da macro de calculo de mascara de bit */
#define LOCAL static    /*!< Definicao do modificador static */

#ifndef DONE_STYLE

typedef unsigned long ulong;    /*!< Definicao de tipo */
typedef unsigned bool;          /*!< Definicao de tipo */
typedef unsigned char * p_ubyte_;   /*!< Definicao de tipo */

#ifndef TRUE
#define FALSE 0 /*!< Definicao de falso */
#define TRUE  1 /*!< Definicao de verdadeiro */
#endif

/* Change to the second definition if you don't have prototypes. */
#define P_(A) A
/* #define P_(A) () */

/* Uncomment this definition if you don't have void. */
/* typedef int void; */

#endif

/**
* \breif CRC Model Abstract Type 
*  The following type stores the context of an executing instance of the
* model algorithm. Most of the fields are model parameters which must be 
*/

typedef struct {
    uint8_t cm_width; /*!< Parameter: Width in bits [8,32].		    */
    ulong cm_poly; /*!< Parameter: The algorithm's polynomial.		    */
    ulong cm_init; /*!< Parameter: Initial register value.		    */
    bool cm_refin; /*!< Parameter: Reflect input bytes?			    */
    bool cm_refot; /*!< Parameter: Reflect output CRC?			    */
    ulong cm_xorot; /*!< Parameter: XOR this to output CRC.		    */
} stParamCtrl_t;

/**
* \brief set before the first initializing call to cm_ini. 
*/
struct cm_t {
    uint8_t cm_width; /*!< Parameter: Width in bits [8,32].		    */
    ulong cm_poly; /*!< Parameter: The algorithm's polynomial.		    */
    ulong cm_init; /*!< Parameter: Initial register value.		    */
    bool cm_refin; /*!< Parameter: Reflect input bytes?			    */
    bool cm_refot; /*!< Parameter: Reflect output CRC?			    */
    ulong cm_xorot; /*!< Parameter: XOR this to output CRC.		    */
    ulong cm_reg; /*!< Context: Context during execution.		    */
    ulong crcTable[MAX_TAM]; /*!< Value: tabela com valores para otimizar o codigo.    */
};

/**********************************************************************/

/***	SESSION VARIAVEIS GLOBAIS	*******************************/

/**********************************************************************/

/***	SESSION PROTOTIPO DE FUNCOES	*******************************/
LOCAL ulong reflect P_((ulong v, int b));
LOCAL ulong widmask P_((p_cm_t));

/**********************************************************************/

/***	SESSION FUNCOES	***********************************************/

/* 
 * \brief Get library version. Function returns version and build number of edm 
 * library. Return value is char pointer. Argument min is flag for output 
 * format. If min is 1, function returns version in full  format, if flag 
 * is 0 function returns only version numbers, For examle: 1.3.0
-*/
const char* libEdmVersion(int min)
{
    static char verstr[MAX_TAM];

    /* Version short */
    if (min) sprintf(verstr, "%d.%d.%d",
	    VERSAO_MAX, VERSAO_MIN, VERSAO_NRO);

	/* Version long */
    else sprintf(verstr, "%d.%d build %d (%s)",
	    VERSAO_MAX, VERSAO_MIN, VERSAO_NRO, __DATE__);

    return verstr;
}

/**
 * \brief funcao que calcula lrc, Longitudinal redundancy check
 * @param data ponteiro para o vetor com os dados
 * @param size tamanho do vetor passado por parametro
 * @return sum valor calculado
 */
uint8_t libEdmCksData(uint8_t* data, const uint32_t size)
{
    uint8_t sum = 0;
    uint32_t l = size;
    while (--l) {
	sum ^= *data;
	data++;
    }
    return sum;
}

/**
* \brief Returns the value v with the bottom b [0,32] bits reflected.
* Example: reflect(0x3e23L,3) == 0x3e26
*/
LOCAL ulong reflect(v, b)

ulong v;
int b;
{
    int i;
    ulong t = v;
    for (i = 0; i < b; i++) {
	if (t & 1L)
	    v |= BITMASK((b - 1) - i);
	else
	    v &= ~BITMASK((b - 1) - i);
	t >>= 1;
    }
    return v;
}

/**
* \brief Returns a longword whose value is (2^p_cm->cm_width)-1.
* The trick is to do this portably (e.g. without doing <<32).
*/
LOCAL ulong widmask(p_cm)
p_cm_t p_cm;
{
    return (((1L << (p_cm->cm_width - 1)) - 1L) << 1) | 1L;
}

/******************************************************************************/

void cm_ini(p_cm)
p_cm_t p_cm;
{
    p_cm->cm_reg = p_cm->cm_init;
}

/******************************************************************************/

void cm_nxt(p_cm, ch)
p_cm_t p_cm;
int ch;
{
    int i;
    ulong uch = (ulong) ch;
    ulong topbit = BITMASK(p_cm->cm_width - 1);

    if (p_cm->cm_refin) uch = reflect(uch, 8);
    p_cm->cm_reg ^= (uch << (p_cm->cm_width - 8));
    for (i = 0; i < 8; i++) {
	if (p_cm->cm_reg & topbit)
	    p_cm->cm_reg = (p_cm->cm_reg << 1) ^ p_cm->cm_poly;
	else
	    p_cm->cm_reg <<= 1;
	p_cm->cm_reg &= widmask(p_cm);
    }
}

/******************************************************************************/

void cm_blk(p_cm, blk_adr, blk_len)
p_cm_t p_cm;
p_ubyte_ blk_adr;
ulong blk_len;
{
    while (blk_len--) cm_nxt(p_cm, *blk_adr++);
}

/******************************************************************************/

ulong cm_crc(p_cm)
p_cm_t p_cm;
{
    if (p_cm->cm_refot)
	return p_cm->cm_xorot ^ reflect(p_cm->cm_reg, p_cm->cm_width);
    else
	return p_cm->cm_xorot ^ p_cm->cm_reg;
}

/******************************************************************************/

/**
 * \brief funcao que inicializa a tabela para facilitar o calculo do crc
 * \author Lucio Pintanel
 * \date que foi criado 17/10/16 19:06
 * @param p_cm - estrutura de controle para calculo de crc
 * @param index - valor do indece passada por parametro
 * @return lresult & widmask(p_cm) - valor a ser guardado na tabela
 */
ulong cm_tab(p_cm, index)
p_cm_t p_cm;
int index;
{
    int lindex;
    ulong lresult;
    ulong ltopbit = BITMASK(p_cm->cm_width - 1);
    ulong linbyte = (ulong) index;

    if (p_cm->cm_refin) linbyte = reflect(linbyte, 8);
    lresult = linbyte << (p_cm->cm_width - 8);
    for (lindex = 0; lindex < 8; lindex++)
	if (lresult & ltopbit)
	    lresult = (lresult << 1) ^ p_cm->cm_poly;
	else
	    lresult <<= 1;
    if (p_cm->cm_refin) lresult = reflect(lresult, p_cm->cm_width);

    return lresult & widmask(p_cm);
}

/**
 * \brief funcao que inicializa a estrutura de controle para calcular o crc
 * \author Lucio Pintanel
 * \date que foi criado 17/10/16 19:00
 * @param cm_width - define o tamanho em bits
 * @param cm_poly - polinomio para calculcar a tabela
 * @param cm_init - define o valor inicial
 * @param cm_refin - define se deve fazer reflect nos dados de entrada
 * @param cm_refot - define se deve fazer reflect no crc de saida
 * @param cm_xorot - define o valor para fazer xor com o crc de saida
 * @return ltmpStCm - ponteiro para a estrutura inicializada
 */
static p_cm_t initCrc(cm_width, cm_poly, cm_init, cm_refin, cm_refot, cm_xorot)
uint8_t cm_width; /* Parameter: Width in bits [8,32].		*/
ulong cm_poly; /* Parameter: The algorithm's polynomial.	*/
ulong cm_init; /* Parameter: Initial register value.		*/
bool cm_refin; /* Parameter: Reflect input bytes?		*/
bool cm_refot; /* Parameter: Reflect output CRC?		*/
ulong cm_xorot; /* Parameter: XOR this to output CRC.		*/
{
    p_cm_t ltmpStCm;

    /*
     * nmemb: numeros de membros
     * size: tamanho de cada membro
     */
    ltmpStCm = calloc(1, sizeof (ltmpStCm));
    if (ltmpStCm == NULL) {
	printf("Nao foi possivel alocar memoria!");
	return NULL;
    }
    ltmpStCm->cm_init = cm_init;
    ltmpStCm->cm_poly = cm_poly;
    ltmpStCm->cm_refin = cm_refin;
    ltmpStCm->cm_refot = cm_refot;
    cm_ini(ltmpStCm);
    ltmpStCm->cm_width = cm_width * 8;
    ltmpStCm->cm_xorot = cm_xorot;

    int lindex;
    for (lindex = 0; lindex < MAX_TAM; lindex++) {
	ltmpStCm->crcTable[lindex] = (ulong) cm_tab(ltmpStCm, lindex);
    }

    return ltmpStCm;
}

/**
 * \brief funcao que inicializa crc16 com padrao CITT
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitCRC16CITT(void)
{
    /*
     * Exemplo:
     *	    Name   : "CRC-16/CITT"
     *	    Width  : 16
     *	    Poly   : 1021
     *	    Init   : FFFF
     *	    RefIn  : False
     *	    RefOut : False
     *	    XorOut : 0000
     *	    Check  : ?
     */
    return initCrc((16 / 8), CRC16_CITT_POLY, 0xFFFF, TRUE, TRUE, 0x0000);
}

/**
 * \brief funcao que calcula o crc dos dados do vetor passado como parametro, 
 * preenchendo a variavel passada por referencia
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:13
 * @param p_cm - ponteiro da estrutura de controle usado para calculo
 * @param vet - ponteiro para o vetor com os dados
 * @param sizeVet - informa o tamanho do vetor passado como parametro
 * @param crc16 - ponteiro para guardar o valor do crc calculado
 * @return 0 -se realizou a rotina com sucesso
 * @return -1 - se o ponteiro do vetor nao foi inicializado
 */
int libEdmGetCrc16CITT_r(p_cm, vet, sizeVet, crc16)
p_cm_t p_cm;
const uint8_t* vet;
uint32_t sizeVet;
uint16_t* crc16;
{
    if (NULL == p_cm) {
	return -1;
    }

    p_cm_t lcitt = p_cm;
    while (--sizeVet) {
	cm_nxt(lcitt, *vet++);
    }

    *crc16 = cm_crc(lcitt);
    return sizeVet;
}

/**
 * \brief funcao que inicializa crc16 com padrao XModem
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitXModem(void)
{
    /*
     * Exemplo:
     *	    Name   : "XMODEM"
     *	    Width  : 16
     *	    Poly   : 8408
     *	    Init   : 0000
     *	    RefIn  : True
     *	    RefOut : True
     *	    XorOut : 0000
     */
    return initCrc((16 / 8), CRC16_CITT_POLY, 0x0000, FALSE, FALSE, 0x0000);
}

/**
 * \brief funcao que calcula o crc dos dados do vetor passado como parametro, 
 * preenchendo a variavel passada por referencia
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:13
 * @param p_cm - ponteiro da estrutura de controle usado para calculo
 * @param vet - ponteiro para o vetor com os dados
 * @param sizeVet - informa o tamanho do vetor passado como parametro
 * @param crc16 - ponteiro para guardar o valor do crc calculado
 * @return 0 -se realizou a rotina com sucesso
 * @return -1 - se o ponteiro do vetor nao foi inicializado
 */
int libGetCrcXModem_r(p_cm, vet, sizeVet, crc16)
p_cm_t p_cm;
const uint8_t* vet;
uint32_t sizeVet;
uint16_t* crc16;
{
    if (NULL == p_cm) {
	return -1;
    }

    p_cm_t lxmodem = p_cm;
    while (--sizeVet) {
	cm_nxt(lxmodem, *vet++);
    }

    *crc16 = cm_crc(lxmodem);
    return sizeVet;
}

/**
 * \brief funcao que inicializa crc16 com padrao Arc
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitArc(void)
{
    /*
     * Exemplo:
     *	    Name   : "ARC"
     *	    Width  : 16
     *	    Poly   : 8005
     *	    Init   : 0000
     *	    RefIn  : True
     *	    RefOut : True
     *	    XorOut : 0000
     */
    return initCrc((16 / 8), CRC16_ARC_POLY, 0x0000, TRUE, TRUE, 0x0000);
}

/**
 * \brief funcao que calcula o crc dos dados do vetor passado como parametro, 
 * preenchendo a variavel passada por referencia
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:13
 * @param p_cm - ponteiro da estrutura de controle usado para calculo
 * @param vet - ponteiro para o vetor com os dados
 * @param sizeVet - informa o tamanho do vetor passado como parametro
 * @param crc16 - ponteiro para guardar o valor do crc calculado
 * @return 0 -se realizou a rotina com sucesso
 * @return -1 - se o ponteiro do vetor nao foi inicializado
 */
int libEdmGetCrcArc_r(p_cm, vet, sizeVet, crc16)
p_cm_t p_cm;
const uint8_t* vet;
uint32_t sizeVet;
uint16_t* crc16;
{
    if (NULL == p_cm) {
	return -1;
    }

    p_cm_t larc = p_cm;
    while (--sizeVet) {
	cm_nxt(larc, *vet++);
    }

    *crc16 = cm_crc(larc);
    return sizeVet;
}

/**
 * \brief funcao que inicializa crc16 com padrao Arc
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitModbus(void)
{
    /*
     * Exemplo:
     *	    Name   : "MODBUS"
     *	    Width  : 16
     *	    Poly   : 8005
     *	    Init   : 0000
     *	    RefIn  : True
     *	    RefOut : True
     *	    XorOut : 0000
     */
    return initCrc((16 / 8), CRC16_ARC_POLY, 0xFFFF, TRUE, TRUE, 0x0000);
}

/**
 * \brief funcao que calcula o crc dos dados do vetor passado como parametro, 
 * preenchendo a variavel passada por referencia
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:13
 * @param p_cm - ponteiro da estrutura de controle usado para calculo
 * @param vet - ponteiro para o vetor com os dados
 * @param sizeVet - informa o tamanho do vetor passado como parametro
 * @param crc16 - ponteiro para guardar o valor do crc calculado
 * @return 0 -se realizou a rotina com sucesso
 * @return -1 - se o ponteiro do vetor nao foi inicializado
 */
int libEdmGetCrcModbus_r(p_cm, vet, sizeVet, crc16)
p_cm_t p_cm;
const uint8_t* vet;
uint32_t sizeVet;
uint16_t* crc16;
{
    if (NULL == p_cm) {
	return -1;
    }

    p_cm_t lmodbus = p_cm;
    while (--sizeVet) {
	cm_nxt(lmodbus, *vet++);
    }

    *crc16 = cm_crc(lmodbus);
    return sizeVet;
}

/**
 * \brief funcao que inicializa crc32
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitCRC32(void)
{
    /*
     * Name   : "CRC-32"
     * Width  : 32
     * Poly   : 04C11DB7
     * Init   : FFFFFFFF
     * RefIn  : True
     * RefOut : True
     * XorOut : FFFFFFFF
     * Check  : CBF43926
     */
    return initCrc((32 / 8), CRC32, 0xFFFFFFFF, TRUE, TRUE, 0xFFFFFFFF);
}

/**
 * \brief funcao que calcula o crc dos dados do vetor passado como parametro, 
 * preenchendo a variavel passada por referencia
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:13
 * @param p_cm - ponteiro da estrutura de controle usado para calculo
 * @param vet - ponteiro para o vetor com os dados
 * @param sizeVet - informa o tamanho do vetor passado como parametro
 * @param crc32 - ponteiro para guardar o valor do crc calculado
 * @return 0 -se realizou a rotina com sucesso
 * @return -1 - se o ponteiro do vetor nao foi inicializado
 */
int libEdmGetCrc32_r(p_cm, vet, sizeVet, crc32)
p_cm_t p_cm;
const uint8_t* vet;
uint32_t sizeVet;
uint32_t* crc32;
{
    if (NULL == p_cm) {
	return -1;
    }

    p_cm_t lcrc32 = p_cm;
    while (--sizeVet) {
	cm_nxt(lcrc32, *vet++);
    }

    *crc32 = cm_crc(lcrc32);
    return sizeVet;
}

void libEdmInfoCrc(p_cm_t cm_t)
{
    printf("cm_init: 0x%04lX\n", cm_t->cm_init);
    printf("cm_poly: 0x%04lX\n", cm_t->cm_poly);
    printf("cm_refin: 0x%02d\n", cm_t->cm_refin);
    printf("cm_refot: 0x%02d\n", cm_t->cm_refot);
    printf("cm_width: 0x%02u\n", cm_t->cm_width);
    printf("cm_xorot: 0x%04lX\n", cm_t->cm_xorot);
    //    int lindex;
    //    for (lindex = 0; lindex < MAX_TAM; lindex++) {
    //	printf("0x%08xl", cm_t->crcTable[lindex]);
    //	if (lindex != 255) printf(",");
    //	if (((lindex + 1) % 4) == 0) printf("\n");
    //    }
}

/* end EDM_C */