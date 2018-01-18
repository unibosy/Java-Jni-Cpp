#include <stdio.h>

#define HE (printf("%s (%d) - <%s>\n",__FILE__,__LINE__,__FUNCTION__),printf)

void test(){
}
#define CP_PTR_RETURN(PTR) \
  {                        \
    if(!PTR){              \
      HE;              \
      return;              \
    }                      \
  }                        \

#define CP_PTR_RETURN_BOOL(PTR) \
  {                             \
    if(!PTR){                   \
      HE;                   \
      return false;             \
    }                           \
  }                             \ 

int main(){
  char* p = NULL;
  CP_PTR_RETURN_BOOL(p);
  return 0;
}
