#ifndef PRINTER_H__
#define PRINTER_H__

#include "port.h"
#include "expression.h"

typedef struct Printer Printer;

Printer *Printer_new (Port *output);
void Printer_delete (Printer *self);

void Printer_print (Printer *self, Expression *expr);

#endif /* PRINTER_H__ */
