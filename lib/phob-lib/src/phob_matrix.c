#include "phob_matrix.h"

// This function calculates the inverse of a 3x3 matrix.
void matrix_inverse(const float in[3][3], float out[3][3])
{
	float det = in[0][0] * (in[1][1] * in[2][2] - in[2][1] * in[1][2]) -
							in[0][1] * (in[1][0] * in[2][2] - in[1][2] * in[2][0]) +
							in[0][2] * (in[1][0] * in[2][1] - in[1][1] * in[2][0]);

	float invdet = 1.0f / det;

  // Then, calculate each element of the output matrix, which is the adjugate of the
  // input matrix divided by the determinant. The adjugate is the transpose of the
  // cofactor matrix, which is why the row and column indices are swapped compared to
  // the input matrix.
	out[0][0] = (in[1][1] * in[2][2] - in[2][1] * in[1][2]) * invdet;
	out[0][1] = (in[0][2] * in[2][1] - in[0][1] * in[2][2]) * invdet;
	out[0][2] = (in[0][1] * in[1][2] - in[0][2] * in[1][1]) * invdet;
	out[1][0] = (in[1][2] * in[2][0] - in[1][0] * in[2][2]) * invdet;
	out[1][1] = (in[0][0] * in[2][2] - in[0][2] * in[2][0]) * invdet;
	out[1][2] = (in[1][0] * in[0][2] - in[0][0] * in[1][2]) * invdet;
	out[2][0] = (in[1][0] * in[2][1] - in[2][0] * in[1][1]) * invdet;
	out[2][1] = (in[2][0] * in[0][1] - in[0][0] * in[2][1]) * invdet;
	out[2][2] = (in[0][0] * in[1][1] - in[1][0] * in[0][1]) * invdet;
}

// Self-explanatory.
void matrix_multiply(const float left[3][3], const float right[3][3], float output[3][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			output[i][j] = 0;
			for (int k = 0; k < 3; k++)
			{
				output[i][j] += left[i][k] * right[k][j];
			}
		}
	}
}

void matrix_print(const float matrix[3][3])
{
	int i, j, nrow, ncol;
	nrow = 3;
	ncol = 3;
	debug_println();
	for (i = 0; i < nrow; i++)
	{
		for (j = 0; j < ncol; j++)
		{
			debug_print(matrix[i][j], 6); // print 6 decimal places
			debug_print(", ");
		}
		debug_println();
	}
	debug_println();
};
