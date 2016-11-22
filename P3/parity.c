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
	#define RESULT (1<<NBITS-1)
#else
	/*  * /
	#define SIZE 4
	unsigned list[SIZE]={0x80000000,0x00100000, 0x00000800,0x00000001};
	#define RESULT 4
	/* */
	#define SIZE 8
	unsigned list[SIZE]={0x7fffffff, 0xffefffff,0xfffff7ff,  0xfffffffe,
			    0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
	#define RESULT 8
	/* * /
	#define SIZE 8
	unsigned list[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0,
			     0xffffffff,0x122345678, 0x9abcdef0, 0xcafebeef};
	#define RESULT 2
	*/	
#endif

int resultado=0;

int parity1(int* array, int len){ 
    int  i,j, val,result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
	val=0;
	for(j=0;j<sizeof(unsigned);j++){
		val ^= x&0x1;
		x>>=1; 
	}
	result+=val;
    }
    return result;
}

int parity2(unsigned* array, int len){ 
    int  i,val,result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
	val=0;
	do{
		val^= x&0x1;
		x>>=1; 
	}while(x);
	result+=val;
    }
    return result;
}

int parity3(int* array, int len){
     int  i, result=0;
    unsigned x;
    for (i=0; i<len; i++){
	x=array[i];
        asm("mov 8(%%ebp), %%ebx	\n"  // array
"	mov 12(%%ebp), %%ecx	\n"  // len
"				\n"
"	mov $0, %%eax		\n"  // retval
"	mov $0, %%edx		\n"  // index
"bucle:				\n"
"	add (%%ebx,%%edx,4),%%eax\n"
"	inc       %%edx		\n"
"	cmp %%edx,%%ecx		\n"
"	jne bucle		\n"
     : 				// output
     : 				// input
     :	"ebx"			// clobber
    );
    }	
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

    crono(parity1, "parity1 (en lenguaje C  -for)");
    crono(parity2, "parity2 (en lenguaje C  -while)");
    crono(parity3, "parity3 (bloque asm entero)");
    crono(parity4, "parity4 (bloque asm entero)");
    crono(parity5, "parity5 (bloque asm entero)");
    crono(parity6, "parity6 (bloque asm entero)");
    //Hay que añadir más    
    	
#if ! COPY_PASTE_CALC
    printf("calculado = %d\n", RESULT);
#endif
    exit(0);
}				

