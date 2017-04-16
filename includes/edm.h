/**
 * @file edm.h
 * @author LUcio Pintanel
 * @date 9 Aug 2016
 * @brief File containing includes, defines, and demias externados.
 *
 */

#ifndef _EDM_H
#define _EDM_H

/***	SESSION INCLUDE	***********************************************/

/**********************************************************************/

/***	SESSION DEFINE, STRUCT, TYPEDEF	*******************************/
typedef struct cm_t* p_cm_t;

/**********************************************************************/

/***	SESSION VARIAVEIS GLOBAIS	***********************************/

/**************************************size********************************/

/***	SESSION PROTOTIPO DE FUNCOES	*******************************/
const char* libEdmVersion(int min);

/**
 * \brief funcao que calcula lrc, Longitudinal redundancy check
 * @param data ponteiro para o vetor com os dados
 * @param size tamanho do vetor passado por parametro
 * @return sum valor calculado
 */
uint8_t libEdmCksData(uint8_t*, const uint32_t);

/**
 * \brief funcao que inicializa crc16 com padrao CITT
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitCRC16CITT(void);

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
int libEdmGetCrc16CITT_r(p_cm_t, const uint8_t*, uint32_t, uint16_t*);

/**
 * \brief funcao que inicializa crc16 com padrao XModem
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitXModem(void);

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
int libGetCrcXModem_r(p_cm_t, const uint8_t*, uint32_t, uint16_t*);

/**
 * \brief funcao que inicializa crc16 com padrao Arc
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitArc(void);

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
int libEdmGetCrcArc_r(p_cm_t, const uint8_t*, uint32_t, uint16_t*);

/**
 * \brief funcao que inicializa crc16 com padrao Arc
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitModbus(void);

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
int libEdmGetCrcModbus_r(p_cm_t, const uint8_t*, uint32_t, uint16_t*);

/**
 * \brief funcao que inicializa crc32
 * \author Lucio Pintanel
 * \date que foi criado 11/10/16 23:09
 * @return p_cm_t - ponteiro com a estrutura para calculo de crc usando
 */
p_cm_t libEdmInitCRC32(void);

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
int libEdmGetCrc32_r(p_cm_t, const uint8_t*, uint32_t, uint32_t*);

void libEdmInfoCrc(p_cm_t cm_t);
/**********************************************************************/

#endif /* _EDM_H */
