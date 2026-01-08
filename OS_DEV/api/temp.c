#include "stdoslib.h"


int main() {
i8 * str = ".hello.i.am.za.wardo";
i8 ** ret = tokenise(str,'.');
i8 i = 6;
while (i){
printf("%s\n",ret[i]);
i --;
}
return 0;
}





