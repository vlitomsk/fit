#ifndef BMOORE_H_INCLUDED
#define BMOORE_H_INCLUDED

/* 
  Возвращает индекс, с которого начинается совпадение, 
   если needle найден в haystack
  Иначе возвращает -1
*/
extern int bmoore_search(const char *haystack, const char *needle);
extern void pi_function(const char *s, int *pi);

#endif