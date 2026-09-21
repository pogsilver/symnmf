import sys
import pandas as pd
import numpy as np
import math
import symnmf_module

EPSILON = math.pow(10, -4)
MAX_ITER = 300
GOALS = {"sym", "ddg", "norm", "symnmf"}

np.random.seed(1234)
def initiate_arguments():
    """
    initiates the agruments for the file
    Returns:
        tuple:
        K value, goal, input vectors list, flag to determine if an error has occured
    """
    args = sys.argv
    flag = False
    try:
        k = args[1]
        goal = args[2]
        file_name = args[3]
        if goal not in GOALS:
            flag = True
    except:
        flag = True
    try:
        # checking numbeer of inputs is valid
        len_of_input = args[4]
        flag = True
    except IndexError:
        pass
    try:
        file = pd.read_csv(file_name, index_col=False, header=None)
        df = pd.DataFrame(file)
        vectors = df.to_numpy()
    except:
        flag = True
    try:
        flag = k_validity(k, len(vectors)) or flag
        k = int(float(k))
    except:
        flag = True
    if flag:
        return 0, "", None ,flag
    
    return k, goal, vectors, flag

def k_validity(k, n):
    """
    checks if the given k represents an integer which is between 1 and given n
    returns True if the input is invalid
    """
    try:
        float_k = float(k)
        k = int(float_k)
        if float_k != k:
            return True
    except ValueError:
        return True
        
    if k <= 1 or n <= k:
        return True
    return False

def format_ls_to_str(ls):
    """
    takes a list of floats in the format [a, b, c] and returns it as str
    in the format a,b,c
    """
    s = ""
    for i in range(len(ls)):
        s += str("{0:.4f}".format(ls[i])) + ","
                 
    return s[:-1]

def print_matrix(mat):
    """
    takes a list of lists of floats representing clusters
    and prints them in the wanted format
    """
    for row in mat:
        print(format_ls_to_str(row))


def initialize_H(m, n, k):
    """
    initializes H matrix according to the algorithm
    Takes:
    m: a float representing the mean of all entries of W
    n: an int representing the number of rows in H (number of given vectors)
    k: an int representing the number of columns in H (number of desired clusters)
    Returns:
        numpy array representing H
    """
    return np.random.uniform(size=(n,k), low=0, high = 2 * math.sqrt(m/k))

def symnmf_func(k, vectors):
    """
    calculates the last updated H which converges according to the algorithm
    Takes:
    k: an int representing the number of columns in H (number of desired clusters)
    vectors: a list of lists representing the vectors
    Returns:
        list of lists representing the converged H
    """
    w = symnmf_module.norm(vectors)
    m = np.mean(w)
    m = float(m)
    h = initialize_H(m, len(vectors),k)
    h_as_ls = h.tolist()

    h_t = symnmf_module.symnmf(h_as_ls, w)
    return h_t


def sym(vectors):
    """
    calculates the sym matrix according to the algorithm and returns it
    Takes:
    vectors: a list of lists representing the vectors
    """
    return symnmf_module.sym(vectors)


def ddg(vectors):
    """
    calculates the ddg matrix according to the algorithm and returns it
    Takes:
    vectors: a list of lists representing the vectors

    """
    return symnmf_module.ddg(vectors)

def norm(vectors):
    """
    calculates the normalized sym matrix according to the algorithm and returns it
    Takes:
    vectors: a list of lists representing the vectors
    """
    return symnmf_module.norm(vectors)

def main():
    # initite and validate input:
    k, goal, vectors, flag = initiate_arguments()
    if flag:
        print("An Error Has Occured")
        sys.exit(1)

    vectors_as_ls = vectors.tolist()
    # excecuting the given command:
    if goal == "symnmf":
        result_mat = symnmf_func(k, vectors_as_ls)
    else:
        result_mat = eval(goal+"(vectors_as_ls)")
    print_matrix(result_mat)

    sys.exit(0)
if __name__ == "__main__":
    main()