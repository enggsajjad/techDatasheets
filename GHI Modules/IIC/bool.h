/***************************************************************************
 *                                                                          
 *                                   B O O L
 *
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: bool.h
 *                                                                          
 ****************************************************************************/
#ifndef _BOOL_H_
#define _BOOL_H_

/*--- a typedef and enum here would be better form, but by deffinition  ---*/
/*--- an enum is an integer, and since we have bit addressable register ---*/
/*--- lets use them                                                     ---*/
#define bool  bit
#define TRUE  1
#define FALSE 0 

#endif
