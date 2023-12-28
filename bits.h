#ifndef _BITS_H_
#define _BITS_H_

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//flags
static void  __fastcall SetFlag(register void* _Data, register size_t _FlagPosition){
	((char*)_Data)[_FlagPosition / 8] = ((char*)_Data)[_FlagPosition / 8] | (1 << _FlagPosition % 8);
}

static void __fastcall UnsetFlag(register void* _Data, register size_t _FlagPosition){
	((char*)_Data)[_FlagPosition / 8] = ((char*)_Data)[_FlagPosition / 8] & ~(1 << _FlagPosition % 8);
}

static unsigned char __fastcall GetFlag(register void* _Data, register size_t _FlagPosition){
	return 1 & (((char*)_Data)[_FlagPosition / 8] >> (_FlagPosition % 8));
}

static void __fastcall ChangeFlag(register void* _Data, register size_t _FlagPosition){
	((char*)_Data)[_FlagPosition / 8] = ((char*)_Data)[_FlagPosition / 8] ^ (1 << _FlagPosition % 8);
}





#endif // !_BITS_H_





