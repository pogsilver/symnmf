import math

EPSILON = 0.0001

def euclidean_distance(x1, x2):
    """
    calculates the euclidean distance between two vectors
    assumes len(x1) == len(x2)
    """
    sq_sum = 0
    for i in range(len(x1)):
        sq_sum += math.pow((x1[i] - x2[i]), 2)

    return math.sqrt(sq_sum)

def initialize_clusters(k, vectors):
    """
    takes an integer k and a list of vectors,
    returns a list of the first k elements
    representing the clusters
    """
    clusters = []
    for i in range(k):
        clusters.append(vectors[i])

    return clusters  

def assign_vector(vector, clusters):
    """
    takes a vecotr represented by a tuple of folats
    and a list of vectors.
    returns the number of the cluster the vector 
    is assigned to
    """
    return min(range(len(clusters)), key = lambda x: euclidean_distance(vector, clusters[x]))

def sum_vectors(v1, v2):
    """
    takes a list of floats (v1) and adds to each cordinate
    the value of coresponding cordinate in v2 (tuple)
    returns the updated v1
    """
    d = len(v1)
    for i in range(d):
        v1[i] += v2[i]
    return v1

def update_cluster(cluster_sum, num_of_elemnets):
    """
    takes a list of sums and a counter and returns a tuple
    of the sums divided by the counter
    """
    new_cluster = []
    d = len(cluster_sum)
    for i in range(d):
        new_cluster.append(cluster_sum[i] / num_of_elemnets)
    return tuple(new_cluster)


def find_centroids(k, d, vectors, max_iter):
    # initiating the algorithm
    old_clusters = initialize_clusters(k, vectors)
    i = 0
    while i < max_iter:
        # counter of the number of vectors assigned to each cluster:
        num_of_vectors_in_cluster= [0 for j in range(k)]
        # sum of the values of the vectors assigned to each cluster:
        vectors_sum = [[0 for j in range(d)] for l in range(k)]
        new_clusters = []
        # assigning vectors to clusters:
        for v in vectors:
            curr_assignment = assign_vector(v, old_clusters)
            num_of_vectors_in_cluster[curr_assignment] += 1
            vectors_sum[curr_assignment] = sum_vectors(vectors_sum[curr_assignment], v)
        # updating clusters:
        for j in range(k):
            if num_of_vectors_in_cluster[j] == 0:
                new_clusters.append(old_clusters[j])
            else:
                new_clusters.append(update_cluster(vectors_sum[j], num_of_vectors_in_cluster[j]))

        if all(euclidean_distance(old_clusters[j], new_clusters[j]) < EPSILON for j in range(k)):
            break
        old_clusters = new_clusters   
    return new_clusters

def assign_vectors_to_clusters(k, d, vectors, max_iter):
    clusters = find_centroids(k, d, vectors, max_iter)
    return [assign_vector(v,clusters) for v in vectors]




