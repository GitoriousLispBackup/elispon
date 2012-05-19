#ifndef STRING_H__
#define STRING_H__

typedef struct String String;

String *String_new (const char *str);
void String_delete (String *self);

char *String_buf (String *self);
int String_length (String *self);

char String_getChar (String *self, int i);

#endif /* STRING_H__ */
