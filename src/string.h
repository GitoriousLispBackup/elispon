#ifndef STRING_H__
#define STRING_H__

typedef struct String String;

String *String_new (char *str);
void String_delete (String *self);

char *String_buf (String *self);

#endif /* STRING_H__ */
