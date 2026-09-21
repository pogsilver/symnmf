from sklearn.metrics import silhouette_score

import sys
import pandas as pd
import numpy as np

import symnmf
import kmeans

MAX_ITER = 300

def initiate():
    """
    initiates the agruments for the file

    Returns:
        tuple:
        K value, input vectors list, flag to determine if an error has occured
    """
    args = sys.argv
    flag = False
    try:
        k = args[1]
        file_name = args[2]
    except:
        flag = True    
    try:
        # checking numbeer of inputs is valid
        len_of_input = args[3]
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
        flag = symnmf.k_validity(k, len(vectors)) or flag
        k = int(float(k))
    except:
        flag = True
    if flag:
        return 0, None ,flag

    return k, vectors, flag


def clusters_from_symnmf(matrix):
    """
    returns a list of the points clusters assignment
    """
    return [max(range(len(row)),key=lambda i:row[i]) for row in matrix]


def main():

    k, np_vectors, flag = initiate()
    if flag:
        print("An Error Has Occured")
        sys.exit(1)

    vectors = np_vectors.tolist()
    h_t = symnmf.symnmf_func(k, vectors)
    symnmf_clusters = clusters_from_symnmf(h_t)

    kmeans_clusters_assignment = kmeans.assign_vectors_to_clusters(k, len(vectors[0]), vectors, MAX_ITER)

    kmeans_score = silhouette_score(vectors, kmeans_clusters_assignment)
    symnmf_score = silhouette_score(vectors, symnmf_clusters)

    print(f"nmf: {symnmf_score:.4f}\nkmeans: {kmeans_score:.4f}")

    sys.exit(0)
if __name__ == "__main__":
    main()
    


