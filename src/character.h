#ifndef CHARACTER_H__
#define CHARACTER_H__

typedef struct Character Character;

Character *Character_new (char c);
void Character_delete (Character *self);

char Character_c (Character *self);

#endif /* CHARACTER_H__ */
