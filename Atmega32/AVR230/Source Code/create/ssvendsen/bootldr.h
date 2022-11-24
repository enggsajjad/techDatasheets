/* bootldr.h                                       */
/* ------                                        */
/*                                               */
/* This file contains the definitions for the    */
/* project. It is to be compiled with the        */
/* 'bootldr' project.                            */

#define PAGE_SIZE 128
#define MEM_SIZE 14336
#define SIGNATURE 0x08192A3B
#define BUFFER_SIZE 148
#define INITIALVECTOR_HI 0x00112233
#define INITIALVECTOR_LO 0x44556677
#define _3DES
