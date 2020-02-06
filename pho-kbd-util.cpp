#include "gcin.h"

gboolean not_41_pho_kbd(char *kbd) {
	static char non41[]="et26 hsu pinyin pinyin-no-tone";
	return strstr(non41, kbd) != NULL;
}	  
