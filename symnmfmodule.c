#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"



/*
takes a linked list of vectors and returns it in python lists form
*/
PyObject* VecToPyList(vector *v)
{
    vector *currVec;
    cord *currCord;
    double val;
    PyObject *mat, *PyCurrVec, *PyCurrCord;
    mat = PyList_New(0);
    currVec = v;
    while (!(currVec==NULL)){
        PyCurrVec = PyList_New(0);
        currCord=currVec->cords;
        while (!(currCord->next==NULL))
        {
            val = currCord->value;
            PyCurrCord = Py_BuildValue("d", val);
            PyList_Append(PyCurrVec, PyCurrCord);
            currCord=currCord->next;
        }

        val = currCord->value;
        PyCurrCord = Py_BuildValue("d", val);
        PyList_Append(PyCurrVec, PyCurrCord);
        PyList_Append(mat, PyCurrVec);
        currVec=currVec->next;
    }
    return mat;
}

/*
takes a list of python lists and returns a linked list of vectors
*/
vector *PyListToVec(PyObject *lst){
    int n, k, i, j;
    double val;
    PyObject *cordsList, *PyVal;

    vector *headPoint, *currVec;
    cord *headCord, *currCord;
    headPoint = NULL;
    headCord = NULL;
    currCord = NULL;
    currVec = NULL;
    PyVal = NULL;
    cordsList = NULL;
    val = -1;
    n = PyLong_AsLong(PyLong_FromSsize_t(PyList_Size(lst)));
    k = PyLong_AsLong(PyLong_FromSsize_t(PyList_Size(PyList_GetItem(lst, 0))));
    i = n-1;
    j = k-1;
    for(; i > -1; i--){
        currVec = malloc(sizeof(vector));
        currVec->next = headPoint;
        headPoint = currVec;
        headCord = NULL;
        cordsList = PyList_GetItem(lst, i);
        for(j = k-1; j > -1; j--){
            currCord = malloc(sizeof(cord));
            currCord->next = headCord;
            headCord = currCord;
            PyVal = PyList_GetItem(cordsList, j);
            val = PyFloat_AsDouble(PyVal);
            currCord->value = val;
        }
        currVec->cords = headCord;
    }
    return headPoint;
}

/*a function to handle the cases where the goal is not symnmf to avoid code duplication*/
static PyObject* GoalIdentifier(PyObject *args, char id){
    PyObject *lst;
    vector *headPoint, *result;
    if(!PyArg_ParseTuple(args, "O",&lst)){
        return NULL;
    }
    headPoint = PyListToVec(lst);
    result = NULL;
    switch (id){
        case 's':
            result = Sym(headPoint);
            break;
        case 'd':
            result = Ddg(headPoint);
            break;
        case 'n':
            result = Norm(headPoint);
            break;
    }
    lst = VecToPyList(result);
    freeMatMemory(result);
    freeMatMemory(headPoint);
    return lst;

}
/*A wrapping function for the python file to call the sym function*/
static PyObject* Pysym(PyObject *self, PyObject *args){
    return GoalIdentifier(args, 's');
}
/*A wrapping function for the python file to call the ddg function*/
static PyObject* Pyddg(PyObject *self, PyObject *args){
    return GoalIdentifier(args, 'd');
}
/*A wrapping function for the python file to call the norm function*/
static PyObject* Pynorm(PyObject *self, PyObject *args){
    return GoalIdentifier(args, 'n');
}

/*A wrapping function for the python file to call the symnmf function*/
static PyObject *Pysymnmf(PyObject *self, PyObject *args, char id){
    PyObject *PyH, *PyW, *FinalH;
    vector *H, *W, *result;
    if(!PyArg_ParseTuple(args, "OO",&PyH, &PyW)){
        return NULL;
    }
    H = PyListToVec(PyH);
    W = PyListToVec(PyW);
    result = Symnmf(H, W);
    freeMatMemory(W);
    FinalH = VecToPyList(result);
    freeMatMemory(result);

    return FinalH;
}



static PyMethodDef funcs[] = {
    {
        "sym",(PyCFunction) Pysym, METH_VARARGS, PyDoc_STR ("A function to calculate the sym matrix of the set of points")
    },
    {
        "ddg",(PyCFunction) Pyddg, METH_VARARGS, PyDoc_STR ("A function to calculate the ddg matrix of the set of points")
    },
    {
        "norm",(PyCFunction) Pynorm, METH_VARARGS, PyDoc_STR ("A function to calculate the normalized sym matrix of the set of points")
    },
    {
        "symnmf",(PyCFunction) Pysymnmf, METH_VARARGS, PyDoc_STR ("A function to calculate the H matrix of the set of points as listed in the algorithm")
    },
    {
        NULL, NULL, 0, NULL
    }
};

static struct PyModuleDef symnmf_module = {
    PyModuleDef_HEAD_INIT,
    "symnmf_module",
    "Python wrapper for the symnmf c functions",
    -1,
    funcs
};

PyMODINIT_FUNC PyInit_symnmf_module(void){
    PyObject *m;
    m = PyModule_Create(&symnmf_module);
    if (!m) {
        return NULL;
    }
    return m;
}