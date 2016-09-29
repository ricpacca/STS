/*****************************************************************************
 R A N K   A L G O R I T H M   R O U T I N E S
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "externs.h"
#include "matrix.h"
#include "debug.h"


#define	MATRIX_FORWARD_ELIMINATION	0
#define	MATRIX_BACKWARD_ELIMINATION	1

int
computeRank(int M, int Q, BitSequence ** matrix)
{
	int i;
	int rank;
	int m = MIN(M, Q);

	/*
	 * FORWARD APPLICATION OF ELEMENTARY ROW OPERATIONS
	 */
	for (i = 0; i < m - 1; i++) {
		if (matrix[i][i] == 1) {
			perform_elementary_row_operations(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix);
		} else {		/* matrix[i][i] = 0 */
			if (find_unit_element_and_swap(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix) == 1) {
				perform_elementary_row_operations(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix);
			}
		}
	}

	/*
	 * BACKWARD APPLICATION OF ELEMENTARY ROW OPERATIONS
	 */
	for (i = m - 1; i > 0; i--) {
		if (matrix[i][i] == 1) {
			perform_elementary_row_operations(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix);
		} else {		/* matrix[i][i] = 0 */
			if (find_unit_element_and_swap(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix) == 1) {
				perform_elementary_row_operations(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix);
			}
		}
	}

	rank = determine_rank(m, M, Q, matrix);

	return rank;
}

void
perform_elementary_row_operations(int flag, int i, int M, int Q, BitSequence ** A)
{
	int j;
	int k;

	if (flag == MATRIX_FORWARD_ELIMINATION) {
		for (j = i + 1; j < M; j++) {
			if (A[j][i] == 1) {
				for (k = i; k < Q; k++) {
					A[j][k] = (A[j][k] + A[i][k]) % 2;
				}
			}
		}
	} else {
		for (j = i - 1; j >= 0; j--) {
			if (A[j][i] == 1) {
				for (k = 0; k < Q; k++) {
					A[j][k] = (A[j][k] + A[i][k]) % 2;
				}
			}
		}
	}
}

int
find_unit_element_and_swap(int flag, int i, int M, int Q, BitSequence ** A)
{
	int index;
	int row_op = 0;

	if (flag == MATRIX_FORWARD_ELIMINATION) {
		index = i + 1;
		while ((index < M) && (A[index][i] == 0)) {
			index++;
		}
		if (index < M) {
			row_op = swap_rows(i, index, Q, A);
		}
	} else {
		index = i - 1;
		while ((index >= 0) && (A[index][i] == 0)) {
			index--;
		}
		if (index >= 0) {
			row_op = swap_rows(i, index, Q, A);
		}
	}

	return row_op;
}

int
swap_rows(int i, int index, int Q, BitSequence ** A)
{
	int p;
	BitSequence temp;

	for (p = 0; p < Q; p++) {
		temp = A[i][p];
		A[i][p] = A[index][p];
		A[index][p] = temp;
	}

	return 1;
}

int
determine_rank(int m, int M, int Q, BitSequence ** A)
{
	int i;
	int j;
	int rank;
	int allZeroes;

	/*
	 * DETERMINE RANK, THAT IS, COUNT THE NUMBER OF NONZERO ROWS
	 */

	rank = m;
	for (i = 0; i < M; i++) {
		allZeroes = 1;
		for (j = 0; j < Q; j++) {
			if (A[i][j] == 1) {
				allZeroes = 0;
				break;
			}
		}
		if (allZeroes == 1) {
			rank--;
		}
	}

	return rank;
}


/*
 * create_matrix - allocate a 2D matrix of BitSequence values
 *
 * given:
 *      M       // number of rows in the matrix
 *      Q       // number of columns in each matrix row
 *
 * returns:
 *      An allocated 2D matrix of BitSequence values.
 *
 * NOTE: This function does NOT return on error.
 *
 * NOTE: Unlike older versions of this function, create_matrix()
 *       does not zeroize the matrix.
 */
BitSequence **
create_matrix(int M, int Q)
{
	BitSequence **matrix;	// matrix top return
	int i;

	/*
	 * firewall
	 */
	if (M < 0) {
		err(10, __FUNCTION__, "number of rows: %d must be > 0", M);
	}
	if (Q < 0) {
		err(10, __FUNCTION__, "number of coolumns per rows: %d must be > 0", Q);
	}

	/*
	 * allocate array of matrix rows
	 */
	matrix = malloc(M * sizeof(matrix[0]));
	if (matrix == NULL) {
		errp(10, __FUNCTION__, "cannot malloc of %ld elements of %ld bytes each for matrix rows",
		     (long int) M, sizeof(BitSequence *));
	}

	/*
	 * allocate the columns for each matrix row
	 */
	for (i = 0; i < M; i++) {
		matrix[i] = malloc(Q * sizeof(matrix[0][0]));
		if (matrix[i] == NULL) {
			errp(10, __FUNCTION__, "cannot malloc of %ld elements of %ld bytes each for matrix[%d] column",
			     (long int) Q, sizeof(BitSequence), i);
		}
	}
	return matrix;
}


void
def_matrix(struct state *state, int M, int Q, BitSequence ** m, int k)
{
	int i;
	int j;

	/*
	 * firewall
	 */
	if (state == NULL) {
		err(1, __FUNCTION__, "state arg is NULL");
	}
	if (state->epsilon == NULL) {
		err(10, __FUNCTION__, "state->epsilon is NULL");
	}

	for (i = 0; i < M; i++) {
		for (j = 0; j < Q; j++) {
			m[i][j] = state->epsilon[k * (M * Q) + j + i * M];
		}
	}
}

void
delete_matrix(int M, BitSequence ** matrix)
{
	int i;

	for (i = 0; i < M; i++) {
		free(matrix[i]);
	}
	free(matrix);
}
