#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

const int ITER = 300;
const double EPS = 0.0001;
const double BETA = 0.5;
const double  REMAINDER = 0.000001;


/*
checks the validity of goal
*/
int ValidateGoal(char *goal){
    if (strcmp(goal, "sym") == 0){
        return 0;
    }
    if (strcmp(goal, "ddg") == 0){
        return 0;
    }
    if (strcmp(goal, "norm") == 0){
        return 0;
    }
    return 1;

}

/*
frees the meomory taken by given cord struct
*/
void freeCordMemory(cord *cords){
    cord *curr,*tmp;
    curr = cords;
    while(!(curr == NULL)){
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    return;
}

/*
frees the meomory taken by given vector struct
*/
void freeVectorMemory(vector *v){
    vector *curr,*tmp;
    curr = v;
    while(!(curr == NULL)){
        freeCordMemory(curr->cords);
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    return;
}
/*
frees the meomory taken by given vector struct representing a matrix
*/
void freeMatMemory(vector *mat){
    freeVectorMemory(mat);
    return;
}

/*
calculates the squared euclidean distance between two vectors
*/
double euclideanDistance(vector *v1, vector *v2){
    double sum, curr_dif;

    cord *v1Cords; 
    cord *v2Cords;
    v1Cords = v1->cords;
    v2Cords = v2->cords;

    sum = 0;
    while (v1Cords != NULL){
        curr_dif = v1Cords->value - v2Cords->value;
        sum = sum + curr_dif*curr_dif;
        v1Cords = v1Cords->next;
        v2Cords = v2Cords->next;
    }
    return sum;
}

/*
takes a vector, and adds a new vector to its next attribute,
and advances it 
*/
vector *AddVector(vector *vec){
    vec->next = malloc(sizeof(vector));
    vec->next->next = NULL;
    return vec->next;
}
/*
takes a cord, and adds a new cord to its next attribute
*/
cord *AddCord(cord *curr_cord){
    
    curr_cord->next = malloc(sizeof(cord));
    curr_cord->next->next = NULL;
    return curr_cord->next;
}

/*
takes a vector representing a matrix and prints it
*/
void printMat(vector *mat)
{
    vector *currRow;
    cord *currCord;
    double val;
    currRow = mat;
    while (!(currRow == NULL)){
        currCord=currRow->cords;
        while (!(currCord->next==NULL))
        {
            val=currCord->value;
            printf("%.4f,",val);
            currCord=currCord->next;
        }
        val=(currCord->value);
        printf("%.4f\n",val);
        currRow=currRow->next;
    }
}
/* Handles a row in the sym matrix, given a matrix of vectors
*/
void HandeSymRow(int i, vector *i_curr_vec, vector *j_curr_vec, cord *curr_cord){
    int j;
    j = 0;

    curr_cord->value = (i == j)? 0: exp((-1) * euclideanDistance(i_curr_vec, j_curr_vec)/2);;
    j_curr_vec = j_curr_vec->next;
    j++;
    while(j_curr_vec != NULL){
           curr_cord = AddCord(curr_cord);
           curr_cord->value = (i == j)? 0: exp((-1) * euclideanDistance(i_curr_vec, j_curr_vec)/2);

            j++;
            j_curr_vec = j_curr_vec->next;

        }
}

/*takes a vector representing a matrix, calculates the sym matrix 
according to the algorithm and and returns a vector representing the sym matrix*/
vector *Sym(vector *vectors){
    int i;
    vector *out_head_vec, *i_curr_vec, *j_curr_vec, *out_curr_vec;
    cord *head_cord, *curr_cord;

    head_cord = malloc(sizeof(cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    out_head_vec = malloc(sizeof(vector));
    out_curr_vec = out_head_vec;
    out_curr_vec->next = NULL;

    i_curr_vec = vectors;
    j_curr_vec = vectors;
    /*Handaling first row*/
    i = 0;
    HandeSymRow(i, i_curr_vec, j_curr_vec, curr_cord);   
    i++;
    i_curr_vec = i_curr_vec->next;
    out_curr_vec->cords = head_cord;
    /*Handaling rest of the matrix*/
    while (i_curr_vec != NULL){
        out_curr_vec = AddVector(out_curr_vec);
        head_cord = malloc(sizeof(cord));
        curr_cord = head_cord;
        HandeSymRow(i, i_curr_vec, j_curr_vec, curr_cord);   
        i++;
        i_curr_vec = i_curr_vec->next;
        out_curr_vec->cords = head_cord;
    }
    return out_head_vec;
}

/*takes a vector representing a matrix, calculates the diagonal of the ddg matrix 
according to the algorithm and and returns a vector representing the diagonal of matrix
used to prevent redundent calls*/
vector *DdgHelper(vector *symMat){

    double sum;
    vector *vec, *currVec;
    cord *headCord, *symCurrCord, *currCord;
    vec = malloc(sizeof(vector));
    vec->next = NULL;
    currVec = symMat;

    headCord = malloc(sizeof(cord));
    headCord->next = NULL;
    currCord = headCord;

    while(currVec != NULL){
        symCurrCord = currVec->cords;
        sum = 0;
        while(symCurrCord != NULL){
            sum += symCurrCord->value;
            symCurrCord = symCurrCord->next;
        }
        currCord->value = sum;
        if(currVec->next != NULL){
           currCord = AddCord(currCord);
        }
        currVec = currVec->next;
        
    }

    vec->cords = headCord;
    return vec;
}
/*takes a vector structs representing a matrix, counts and returns the amount of columns in
the matrix*/
int countColumns(vector *vec){
    cord *curr;
    int i;
    curr = vec->cords;
    i = 0;
    while (curr != NULL){
        curr = curr->next;
        i++;
    } 
    return i;
}

/*Pads the vector of ddg that represents the matrix diagonal
*/
vector *PadDdg(vector *vec){
    vector *out_vec, *curr_vec;
    cord *curr_cord, *out_curr_cord;
    int i, j, n;

    out_vec = malloc(sizeof(vector));
    out_vec->next = NULL;
    out_vec->cords = malloc(sizeof(cord));
    out_vec->cords->next = NULL;
    curr_vec = out_vec;
    out_curr_cord = out_vec->cords;
    curr_cord = vec->cords;
    i = 0;
    j = 0;
    n = countColumns(vec);
    for(; i < n; i++){
        for(j = 0; j < n; j++){
            out_curr_cord->value = (i == j) ? curr_cord->value : 0;

            if(j < n-1){
            out_curr_cord = AddCord(out_curr_cord);
            }
        }
        curr_cord = curr_cord->next;
        if(i < n-1){
        curr_vec = AddVector(curr_vec);
        curr_vec->cords = malloc(sizeof(cord));
        curr_vec->cords->next = NULL;
        out_curr_cord = curr_vec->cords;
        }
    }

    return out_vec;
}

/*takes a vector representing a matrix, calculates the ddg matrix 
according to the algorithm and and returns a vector representing the ddg matrix*/
vector *Ddg(vector *vectors){
    vector *A, *diag, *out;
    A = Sym(vectors);
    diag = DdgHelper(A);
    out = PadDdg(diag);
    freeMatMemory(A);
    freeVectorMemory(diag);
    return out;
}

/*takes a vector representing a matrix, calculates the normalized sym matrix 
according to the algorithm and and returns a vector representing the normalized sym matrix*/
vector *Norm(vector *vectors){

    vector *A, *D, *i_A_curr;
    cord *j_A_curr, *i_D_cord, *j_D_cord;
    double di, dj;

    A = Sym(vectors);
    D = DdgHelper(A);
    i_A_curr = A;
    i_D_cord = D->cords;
    j_D_cord = D->cords;

    while(i_A_curr != NULL){
        j_A_curr = i_A_curr->cords;
        di = sqrt(i_D_cord->value);
        while(j_A_curr != NULL){
            dj = sqrt(j_D_cord->value);
            j_A_curr->value = (di * dj != 0) ? j_A_curr->value / (di * dj): j_A_curr->value / REMAINDER;
            j_A_curr = j_A_curr->next;
            j_D_cord = j_D_cord->next;
        }
        i_A_curr = i_A_curr->next;

        i_D_cord = i_D_cord->next;
        j_D_cord = D->cords;
    }
    freeMatMemory(D);
    return A;
}

/*takes a vector A representing a matrix, and a vector BT representing a transposed matrix B 
calculates and returns a vector representing AB matrix*/
vector *MatMultipication(vector *A, vector *BT){
    vector *head, *i_vec, *j_vec, *curr_vec;
    cord *i_curr, *j_curr, *head_cord, *curr_cord;
    double val;
    head = malloc(sizeof(vector));
    head->next = NULL;
    head_cord = malloc(sizeof(cord));
    curr_cord = head_cord;
    curr_vec = head;
    i_vec = A;
    i_curr = i_vec->cords;
    while(i_vec != NULL){
        j_vec = BT;
        while(j_vec != NULL){
            val = 0;
            i_curr = i_vec->cords;
            j_curr = j_vec->cords;
            /*calculating the ij entry:*/
            while(i_curr != NULL){
                val = val + i_curr->value * j_curr->value;
                i_curr = i_curr->next;
                j_curr = j_curr->next;
            }
            curr_cord->value = val;
            if(j_vec->next != NULL){curr_cord = AddCord(curr_cord);}
            j_vec = j_vec->next;
        }
        i_vec = i_vec->next;
        curr_vec->cords = head_cord;
        if(i_vec != NULL){
            curr_vec = AddVector(curr_vec);
            head_cord = malloc(sizeof(cord));
            head_cord->next = NULL;
            curr_cord = head_cord;
        }
    }
    return head;
}
/*
takes a vector representing a matrix and returns its number of rows
*/
int countRows(vector *mat){

    int rows;
    vector *counter;
    counter = mat;
    rows = 0;
    while(counter != NULL){
        counter = counter->next;
        rows++;
    }
    return rows;
}

/*takes a vector A representing a matrix, and returns a vector representing A transposed*/
vector *MatTranspose(vector *A){
    int rows, i, j;
    vector *curr_vec, *head_vec, *A_curr_vec;
    cord **next_cords, *curr_cord;

    rows = countRows(A);
    next_cords = malloc(rows * sizeof(cord));

    head_vec = malloc(sizeof(vector));
    head_vec->cords = malloc(sizeof(cord));
    head_vec->cords->next = NULL;
    head_vec->next = NULL;
    curr_cord = head_vec->cords;
    A_curr_vec = A;
    curr_vec = head_vec;
    j = 0;
    for(i = 0;i < rows; i++){
        next_cords[i] = A_curr_vec->cords;
        A_curr_vec = A_curr_vec->next;
    }
    /*next_cords is an array, s.t. its ith entry has a pointer to the next cord of the ith vector*/
    while(next_cords[0] != NULL){
        for(i = 0; i < rows; i++){
            curr_cord->value = next_cords[i]->value;
            next_cords[i] = next_cords[i]->next;
            if(i < rows - 1){
               curr_cord = AddCord(curr_cord);
            }
        }
        if(next_cords[0] != NULL){
            curr_vec = AddVector(curr_vec);
            curr_vec->cords = malloc(sizeof(cord));
            curr_cord = curr_vec->cords;
            curr_cord->next = NULL;
        }
        j++;
    }
    return head_vec;   
}

/*takes vectors A and B representing matrices 
calculates and returns their Frobenius norm squared*/
double DistanceByFNorm(vector *A, vector *B){
    double sum;
    vector *i_currA, *i_currB;
    cord *j_currA, *j_currB;
    
    i_currA = A;
    i_currB = B;

    sum = 0;
    while(i_currA != NULL){
        j_currA = i_currA->cords;
        j_currB = i_currB->cords;
        while(j_currA != NULL){
            sum = sum + pow((j_currA->value - j_currB->value), 2);
            j_currA = j_currA->next;
            j_currB = j_currB->next;
        }
        i_currA = i_currA ->next;
        i_currB = i_currB ->next;
    }

    return sum;   
}

/*
calculates the ij place of the H matrix according to the algorithm
*/
double updadeH_i_j(cord *j_currH, cord *j_currA, cord *j_currB){
    if(j_currB->value == 0.0){
        j_currB->value = REMAINDER;
    }
    return (j_currH->value) * (1 - BETA + BETA * (j_currA->value) / (j_currB->value));
}

/*Goes over all the entries in the H matrix and updates them into new_H according to the algorithm
*/
void updateHLoop(vector *A, vector *B, vector *H, vector *new_H){
    vector *i_currA, *i_currB, *i_curr_newH, *i_currH;
    cord *j_currA, *j_currB, *j_curr_newH, *j_currH;
    /* A = W*H, B = H*H_t*H */
    i_currA = A;
    i_currB = B;
    i_curr_newH = new_H;
    i_currH = H;
    
    while(i_currH != NULL){
        j_currA = i_currA->cords;
        j_currB = i_currB->cords;
        j_currH = i_currH->cords;
        j_curr_newH = i_curr_newH->cords;
        while(j_currH != NULL){  
            j_curr_newH->value = updadeH_i_j(j_currH, j_currA, j_currB);
            j_currA = j_currA->next;
            j_currB = j_currB->next;
            j_currH = j_currH->next;
            if(j_currH != NULL){
                j_curr_newH = AddCord(j_curr_newH);
            }
        }
        i_currA = i_currA->next;
        i_currB = i_currB->next;
        i_currH = i_currH->next;
        if(i_currH != NULL){
            i_curr_newH = AddVector(i_curr_newH);
            i_curr_newH->cords = malloc(sizeof(cord));
            i_curr_newH->cords->next = NULL;
            
        }
    }
}
/*updates H according to the algorithm and returns the updated H
*/
vector *UpdateH(vector *H, vector *W){
    /* A = W*H, B = H*H_t*H */
    vector *A, *H_t, *HH_t, *B, *new_H;
    H_t = MatTranspose(H);
    A = MatMultipication(W, H_t);
    HH_t = MatMultipication(H,H);
    B = MatMultipication(HH_t, H_t);
    freeMatMemory(H_t);
    freeMatMemory(HH_t);
    new_H = malloc(sizeof(vector));
    new_H->next = NULL;
    new_H->cords = malloc(sizeof(cord));
    new_H->cords->next = NULL;
    
    updateHLoop(A, B, H, new_H);
    freeMatMemory(A);
    freeMatMemory(B);

    return new_H;
} 

/*calculates H according to the algorithm and returns the Final H,
given the initial H and the normalized sym matrix W
*/
vector *Symnmf(vector *H, vector *W){

    vector *newH, *oldH;
    double d;
    int i;
    i = 0;
    oldH = H;
    newH = UpdateH(oldH, W);
    d = DistanceByFNorm(oldH, newH);
    while((i < ITER)){
        freeMatMemory(oldH);
        oldH = newH;
        newH = UpdateH(oldH, W);
        d = DistanceByFNorm(oldH, newH);
        if ( d < EPS){
            break;
        }
        i++;
    }
    freeMatMemory(oldH);
    return newH;
}


/*
a function to handle the given goal from the user
*/

vector *HandleCommand(char goal, vector *head_vec){
    vector *result;
    result = NULL;
     switch (goal){
        case 's':
            result = Sym(head_vec);
            break;
        case 'd':
            result = Ddg(head_vec);
            break;
        case 'n':
            result = Norm(head_vec);
            break;
    }
    return result;

}
/*Deletes the last redudent vector struct in the list*/
void GetPointsFromFileHelper(vector *head_vec){
    vector *curr_vec;
    curr_vec = head_vec;
    while(curr_vec->next->next!=NULL)
    {
    curr_vec=curr_vec->next;
    }
    free(curr_vec->next);
    curr_vec->next=NULL;
}

/* takes the name of the file and returns a vectors linked list of the vectors
provided by the file
*/
vector *GetPointsFromFile(FILE *fl){
    char c;
    double val;
    vector *head_vec, *curr_vec;
    cord *head_cord, *curr_cord;
    
    head_vec = malloc(sizeof(vector));
    curr_vec = head_vec;

    head_cord = malloc(sizeof(cord));
    curr_cord = head_cord;
    /* transfering the file's data to linked list*/
    while(fscanf(fl, "%lf%c", &val, &c) == 2){
        curr_cord->value = val;
        if(c == ','){
           curr_cord = AddCord(curr_cord);
        }
        else{ 
            curr_vec->cords=head_cord;

            curr_vec = AddVector(curr_vec);
            head_cord=malloc(sizeof(cord));
            curr_cord=head_cord;
        }
    }
    /*Deletes the last redudent vector and cord structs in the list*/
    GetPointsFromFileHelper(head_vec);
    free(head_cord); 
    return head_vec;
}

int main(int argc, char **argv){
    char *fileName, *goal;
    FILE *fl;
    vector *head_vec, *result;
    /*validating the input:*/
    if((argc != 3)){
        printf("An Error Has Occured\n");
        exit(1);
    }
    goal = argv[1];
    fileName = argv[2];
    fl = fopen(fileName, "r");
    if((fl == NULL) || ValidateGoal(goal)){
        printf("An Error Has Occured\n");
        exit(1);
    }
    /*executing the algorithm:*/
    head_vec = GetPointsFromFile(fl);
    fclose(fl);
    result = HandleCommand(goal[0], head_vec);
    printMat(result);
    freeMatMemory(result);
    freeVectorMemory(head_vec);

    exit(0);
}