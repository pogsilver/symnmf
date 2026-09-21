#ifndef SYMNMF_H_
#define SYMNMF_H_
#endif
struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};
typedef struct cord cord;
typedef struct vector vector;
vector *AddVector(vector *vec);
cord *AddCord(cord *cord);
vector *Sym(vector *vectors);
vector *Ddg(vector *vectors);
vector *Norm(vector *vectors);
vector *Symnmf(vector *H, vector *W);
void freeMatMemory(vector *mat);
