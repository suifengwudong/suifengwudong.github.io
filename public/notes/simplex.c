#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief 简单的单纯形法实现 (Linear programming solver)
 * 目标: max c^T x, s.t. A x = b, x >= 0
 * 
 * 使用方式:
 * 1. 输入约束数 M 和 变量数 N (含松弛/人工变量)
 * 2. 输入目标函数系数 c (注意如果是大M法，需人为给定一个很大的负值代表大M)
 * 3. 输入系数矩阵 A
 * 4. 输入右端常数项 b
 * 5. 输入初始基索引列表 basis
 */

void simplex(int M_val, int N_val, double **A, double *b, double *c, int *basis) {
    int iter = 0;
    while (iter < 1000) {
        // 1. 计算检验数 reduced_cost
        double *reduced_cost = (double *)malloc(N_val * sizeof(double));
        for (int j = 0; j < N_val; j++) {
            reduced_cost[j] = c[j];
            for (int i = 0; i < M_val; i++) {
                reduced_cost[j] -= c[basis[i]] * A[i][j];
            }
        }

        // 2. 选择入基变量 (针对最大化问题: 选择检验数最大的正值)
        int enter = -1;
        double max_val = 1e-9; 
        for (int j = 0; j < N_val; j++) {
            if (reduced_cost[j] > max_val) {
                max_val = reduced_cost[j];
                enter = j;
            }
        }
        
        free(reduced_cost);
        if (enter == -1) break; // 已达到最优

        // 3. 选择出基变量 (最小比值原则)
        int leave = -1;
        double min_ratio = INFINITY;
        for (int i = 0; i < M_val; i++) {
            if (A[i][enter] > 1e-9) {
                double ratio = b[i] / A[i][enter];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    leave = i;
                }
            }
        }

        if (leave == -1) {
            printf("Problem is unbounded.\n");
            return;
        }

        // 4. 迭代更新 (Pivot Row Operations)
        basis[leave] = enter;
        double pivot = A[leave][enter];
        for (int j = 0; j < N_val; j++) A[leave][j] /= pivot;
        b[leave] /= pivot;

        for (int i = 0; i < M_val; i++) {
            if (i != leave) {
                double factor = A[i][enter];
                for (int j = 0; j < N_val; j++) {
                    A[i][j] -= factor * A[leave][j];
                }
                b[i] -= factor * b[leave];
            }
        }
        iter++;
    }

    // 5. 输出结果
    double obj = 0;
    double *x = (double *)calloc(N_val, sizeof(double));
    for (int i = 0; i < M_val; i++) x[basis[i]] = b[i];
    for (int j = 0; j < N_val; j++) obj += c[j] * x[j];

    printf("\nOptimization Completed in %d iterations.\n", iter);
    printf("Optimal objective value: %.4f\n", obj);
    for (int i = 0; i < N_val; i++) {
        printf("x[%d] = %.4f\n", i, x[i]);
    }
    free(x);
}

int main() {
    int M_val, N_val;
    printf("Enter M and N: ");
    if (scanf("%d %d", &M_val, &N_val) != 2) return 1;

    double *c = (double *)malloc(N_val * sizeof(double));
    for (int i = 0; i < N_val; i++) scanf("%lf", &c[i]);

    double **A = (double **)malloc(M_val * sizeof(double *));
    for (int i = 0; i < M_val; i++) {
        A[i] = (double *)malloc(N_val * sizeof(double));
        for (int j = 0; j < N_val; j++) scanf("%lf", &A[i][j]);
    }

    double *b = (double *)malloc(M_val * sizeof(double));
    for (int i = 0; i < M_val; i++) scanf("%lf", &b[i]);

    int *basis = (int *)malloc(M_val * sizeof(int));
    for (int i = 0; i < M_val; i++) scanf("%d", &basis[i]);

    simplex(M_val, N_val, A, b, c, basis);
    return 0;
}