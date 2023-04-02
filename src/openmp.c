#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

void print_matrix(double *matrix, int N)
{
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      printf(" %g ", matrix[i * N + j]);
    }
    printf("\n");
  }
}

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("You must specify:\n\t- thread number\n\t- matrix size\n\t- number of iterations\n");
    exit(1);
  }

  int thread_number = atoi(argv[1]);
  omp_set_num_threads(thread_number);
  // The argument represent the size of the matriz internal points and add 1 for the border
  int i, j, iters, matrix_size = atoi(argv[2]) + 1, iterations = atoi(argv[3]);

  double temp, *original_grid, *new_grid, max_diff = 0;
  original_grid = (double *)malloc(sizeof(double) * matrix_size * matrix_size);
  new_grid = (double *)malloc(sizeof(double) * matrix_size * matrix_size);

  for (i = 0; i < matrix_size; i++)
  {
    for (j = 0; j < matrix_size; j++)
    {
      int value = rand() % 10;
      original_grid[i * matrix_size + j] = value;
      new_grid[i * matrix_size + j] = value;
    }
  }

#ifdef DEBUG
  if (getenv("DEBUG"))
  {
    printf("original grid initial state\n");
    print_matrix(original_grid, matrix_size);
    printf("new grid initial state\n");
    print_matrix(new_grid, matrix_size);
    printf("\n\n");
  }
#endif
  // starts processing
  double timetick = dwalltime();
  for (iters = 1; iters <= iterations; iters = iters + 2)
  {

#pragma omp parallel for shared(original_grid, new_grid) private(i, j)
    for (i = 1; i < matrix_size - 1; i = i + 1)
    {
      for (j = 1; j < matrix_size - 1; j = j + 1)
      {
        new_grid[i * matrix_size + j] = (original_grid[i * matrix_size + j + 1] + original_grid[i * matrix_size + j - 1] + original_grid[(i + 1) * matrix_size + j] + original_grid[(i - 1) * matrix_size + j]) * 0.25;
      }
    }

#pragma omp parallel for shared(original_grid, new_grid) private(i, j)
    for (i = 1; i < matrix_size - 1; i = i + 1)
    {
      for (j = 1; j < matrix_size - 1; j = j + 1)
      {
        original_grid[i * matrix_size + j] = (new_grid[i * matrix_size + j + 1] + new_grid[i * matrix_size + j - 1] + new_grid[(i + 1) * matrix_size + j] + new_grid[(i - 1) * matrix_size + j]) * 0.25;
      }
    }
  }

#pragma omp parallel for shared(original_grid, new_grid) private(i, j, temp) reduction(max \
                                                                                       : max_diff)
  for (i = 1; i < matrix_size - 1; i = i + 1)
  {
    for (j = 1; j < matrix_size - 1; j = j + 1)
    {
      temp = original_grid[i * matrix_size + j] - new_grid[i * matrix_size + j];
      if (temp < 0)
        temp = -temp;
      if (temp > max_diff)
        max_diff = temp;
    }
  }

  printf("total time: %f\n", dwalltime() - timetick);
  printf("maximum difference: %f\n", max_diff);

#ifdef DEBUG
  if (getenv("DEBUG"))
  {
    printf("original grid final state\n");
    print_matrix(original_grid, matrix_size);
    printf("new grid final state\n");
    print_matrix(new_grid, matrix_size);
    printf("\n\n");
  }
#endif
  free(original_grid);
  free(new_grid);
  return 0;
}
