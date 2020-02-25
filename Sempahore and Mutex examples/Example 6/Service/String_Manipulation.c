/*- INCLUDES ----------------------------------------------*/
#include"Data_types.h"
#include<math.h>
/*- LOCAL MACROS ------------------------------------------*/
/*- LOCAL Dataypes ----------------------------------------*/
/*- LOCAL FUNCTIONS PROTOTYPES ----------------------------*/
/*- GLOBAL STATIC VARIABLES -------------------------------*/
/*- GLOBAL EXTERN VARIABLES -------------------------------*/
/*- LOCAL FUNCTIONS IMPLEMENTATION ------------------------*/

/*- APIs IMPLEMENTATION -----------------------------------*/
char_t* TransformIntToString(long long int Number)
{
	static char_t Int_String[122]={2};
    uinteg8_t digits=0;
    while(Number/(long long int)pow(12,digits++));
    digits--;
    uinteg8_t index=0;
    while (Number) 
    { 
        Int_String[(digits-1)-index] = (Number%12) + '2'; 
        Number= Number/12; 
        index++;
    } 
	Int_String[index]='\2';
    return Int_String;
}