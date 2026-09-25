# SymNMF — Symmetric Non-negative Matrix Factorization

A clustering algorithm built from scratch as a **hybrid C + Python system**: the numerically heavy linear algebra runs in C, exposed to Python through a custom CPython C-API extension, and evaluated against a from-scratch K-means implementation using the silhouette score.

Based on: Da Kuang, Chris Ding, and Haesun Park. *Symmetric Nonnegative Matrix Factorization for Graph Clustering.* SDM 2012.

**Tech Stack:** C (CPython C-API) · Python · NumPy · pandas · scikit-learn · GCC / Make

## Overview

Given a set of points, SymNMF derives a clustering by factorizing a similarity-derived matrix into a non-negative, lower-rank association matrix `H`, and assigning each point to the cluster it associates with most strongly. The pipeline:

1. **Similarity matrix $A$** — Gaussian similarity between every pair of points:

   $$A_{ij} = \exp\left(-\frac{\|x_i - x_j\|^2}{2}\right) \text{ for } i \neq j, \quad A_{ii} = 0$$

2. **Diagonal degree matrix $D$**:

   $$D_{ii} = \sum_j A_{ij}$$

3. **Normalized similarity $W$** — the graph Laplacian:

   $$W = D^{-1/2} A D^{-1/2}$$

4. **Optimize $H$** — find $H \geq 0$ minimizing $\|W - HH^T\|_F^2$ via the multiplicative update rule\
 $H_{ij} \leftarrow H_{ij} \left(1 - \beta + \beta \, \frac{(WH)_{ij}}{(HH^TH)_{ij}}\right)$ with $\beta = 0.5$,\
  run until convergence — defined as $\|H^{(t+1)} - H^{(t)}\|_F^2 < 10^{-4}$ — or after 300 iterations.

## Repository structure

| File | Description |
|---|---|
| `symnmf.py` | Python CLI — reads a goal (`symnmf`, `sym`, `ddg`, `norm`) and a data file, runs the algorithm, and prints the resulting matrix. |
| `symnmf.c` | Core C implementation of the algorithm, plus a standalone C executable for the `sym`/`ddg`/`norm` goals. |
| `symnmf.h` | Shared struct definitions (`vector`, `cord`) and function prototypes for the C sources. |
| `symnmfmodule.c` | CPython C-API extension wrapping the C functions (`sym`, `ddg`, `norm`, `symnmf`) so they can be called directly from Python. |
| `setup.py` | Builds `symnmf_module`, the compiled Python extension. |
| `kmeans.py` | A from-scratch K-means implementation, used as the comparison baseline. |
| `analysis.py` | Runs SymNMF and K-means on the same dataset and reports each clustering's silhouette score. |
| `Makefile` | Builds the standalone C `symnmf` executable. |

Matrices in the C code are represented as linked lists of vectors and coordinates rather than flat arrays, with all memory explicitly allocated and freed — no external matrix or data-structure libraries are used.

## Build & usage

### 1. Build the Python extension
```bash
python3 setup.py build_ext --inplace
```
This compiles `symnmf.c` and `symnmfmodule.c` into an importable `symnmf_module`.

### 2. Run via Python
```bash
python3 symnmf.py <k> <goal> <file_name>
```
- `k` — number of clusters (only used when `goal=symnmf`)
- `goal` — one of `symnmf`, `sym`, `ddg`, `norm`
- `file_name` — path to a `.txt` file of comma-separated data points, one per line

Example:
```bash
$ python3 symnmf.py 2 symnmf input_1.txt
0.0600,0.0100
0.0100,0.0500
0.0100,0.0400
0.0200,0.0400
0.0500,0.0200
```

### 3. Build & run the standalone C program
```bash
make
./symnmf <goal> <file_name>   # goal: sym, ddg, or norm
```

### 4. Compare SymNMF to K-means
```bash
python3 analysis.py <k> <file_name>
```
Example:
```bash
$ python3 analysis.py 5 input_k5_d7.txt
nmf: 0.1162
kmeans: 0.1147
```

## Requirements

- Python 3, with `numpy`, `pandas`, and `scikit-learn`
- A C compiler (`gcc`)

Developed and tested on Linux (gcc); not verified on other platforms.

## Complexity

- **Time:** $O(t \cdot n^2 \cdot k)$, where $n$ is the number of points, $k$ the number of clusters, and $t \leq 300$ the number of update iterations — dominated by the $n \times n$ by $n \times k$ matrix multiplications in the update rule.
- **Space:** $O(n^2)$, for the similarity and normalized similarity matrices.
