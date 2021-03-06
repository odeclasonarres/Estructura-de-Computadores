//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval
	
#define TEST		0
#define COPY_PASTE_CALC 1


#if ! TEST
	#define NBITS 20
	#define SIZE (1<<NBITS) 	// tamaño suficiente para tiempo apreciable 2^20
	unsigned lista[SIZE];
	#define RESULT (NBITS*(1<<NBITS-1))
#else
	/*  * /
	#define SIZE 4
	unsigned list[SIZE]={0x80000000,0x00100000, 0x00000800,0x00000001};
	#define RESULT 4
	/* */
	#define SIZE 8
	unsigned list[SIZE]={0x7fffffff, 0xffefffff,0xfffff7ff,  0xfffffffe,
			    0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
	#define RESULT 156
	/* * /
	#define SIZE 8
	unsigned list[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0,
			     0xffffffff,0x122345678, 0x9abcdef0, 0xcafebeef};
	#define RESULT 116
	*/	
#endif

int resultado=0;

int popcount1(int* array, int len){ //Transparencia 43 tema 2.2
    int  i,j,result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
	for(j=0;j< 8*sizeof(unsigned);j++){
		result += x&0x1;
		x>>=1; 
	}
    }
    return result;
}

int popcount2(unsigned* array, int len){ //Transparencia 38 tema 2.2
    int  i, result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
	do{
		result += x&0x1;
		x>>=1; 
	}while(x);
    }
    return result;
}

int popcount3(unsigned* array, int len){
     int  i, result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
        asm(
		"\n" 
		"ini3:		\n\t"
			"shr %[x]	\n\t"
			"adc $0x0, %[r]	\n\t"
			"test %[x], %[x]\n\t"
			"jnz ini3"
     			:[r]"+r"(result)			
     			:[x]"r"(x)
	);			
    }	
}

int popcount4(unsigned* array, int len){
	int i,j,val, result=0;
	unsigned x;
	for(i=0;i<len;i++){
		val=0;
		x=array[i];
		for(j=0;j<8;j++){
			val+=x&0x1010101;
			x>>=1;
		}
		val+=(val>>16);	
		val+=(val>>8);	
		result+=(val&0xFF);		
	}
	return result;
}

int popcount5(unsigned* array, int len){
	int i, val=0, result=0;
	int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
	int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
	if (len & 0x3) printf("Leyendo 128 bits pero len no multiplo de 4?\n");
	for (i=0;i<len;i+=4){
		asm(
			"movdqu     %[x], %%xmm0 \n\t"
			"movdqa   %%xmm0, %%xmm1 \n\t"
			"movdqu     %[m], %%xmm6 \n\t"
			"psrlw       $4 , %%xmm1 \n\t"
			"pand     %%xmm6, %%xmm0 \n\t"
			"pand     %%xmm6, %%xmm1 \n\t" 
			"movdqu     %[l], %%xmm2 \n\t" 
			"movdqa   %%xmm2, %%xmm3 \n\t" 
			"pshufb   %%xmm0, %%xmm2 \n\t" 
			"pshufb   %%xmm1, %%xmm3 \n\t" 
			"paddb    %%xmm2, %%xmm3 \n\t" 
			"pxor     %%xmm0, %%xmm0 \n\t" 
			"psadbw   %%xmm0, %%xmm3 \n\t" 
			"movhlps  %%xmm3, %%xmm0 \n\t" 
			"paddd    %%xmm3, %%xmm0 \n\t" 
			"movd     %%xmm0, %[val] \n\t"
			:[val]"=r" (val)
			:[x]"m"(array[i]),[m]"m" (SSE_mask[0]),[l]"m"(SSE_LUTb[0])
		);
		result +=val;
	}
	return result;
}

void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+(tv2.tv_usec-tv1.tv_usec);
    
#if COPY_PASTE_CALC
    printf("%ld" "\n", tv_usecs);
#else
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
#endif
}


int main(){
#if ! TEST
    int i;			// inicializar array solo para ejemplo grande
    for (i=0; i<SIZE; i++)	// se queda en cache
	 lista[i]=i;		//unsigned lista[SIZE]=va
#endif

    crono(popcount1, "popcount1 (en lenguaje C  -for)");
    crono(popcount2, "popcount2 (en lenguaje C  -while)");
    crono(popcount3, "popcount3 (bloque asm entero)");
    crono(popcount4, "popcount4 (en C, mascara)");
    crono(popcount5, "popcount5 (SSSE3)");
    	
#if ! COPY_PASTE_CALC
    printf("calculado = %d\n", RESULT);
#endif
    exit(0);
}

