

#include <stddef.h>

extern int stdc_3D85D6F8(const char *str1, const char *str2);							// strcmp()
int strcmp(const char *str1, const char *str2) {if(!str1 || !str2) return -1; return stdc_3D85D6F8(str1, str2);}


extern size_t stdc_2F45D39C(const char *str);											// strlen()
#define __strlen	stdc_2F45D39C
size_t strlen(const char *str) {if(!str) return 0; return __strlen(str);}

extern void allocator_77A602DD(void *ptr);												// free()
void free(void *ptr) {if(!ptr) return; allocator_77A602DD(ptr);}

extern void *allocator_759E0635(size_t size);											// malloc()
void* malloc (size_t size) {if(!size) return NULL; return allocator_759E0635(size);}