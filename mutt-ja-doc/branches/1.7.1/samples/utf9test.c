#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
int main(void) {
  wchar_t w[] = L"×αЯⅡ⊿⌒⑪■○★※〒㊤";
  wchar_t *wp;
  int i;
  setlocale(LC_ALL, "ja_JP.UTF-9");
  for (wp = w; *wp; wp++)
      printf("%06X[%lc]: %d\n", *wp, *wp, wcwidth(*wp));
  return 0;
}

