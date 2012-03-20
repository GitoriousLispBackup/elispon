#ifndef PRINTER_H__
#define PRINTER_H__

typedef struct Printer Printer;

Printer *Printer_new (const char *output);
void Printer_delete (Printer *self);

void Printer_printExpression (Printer *self, Expression *expr);

#endif /* PRINTER_H__ */
