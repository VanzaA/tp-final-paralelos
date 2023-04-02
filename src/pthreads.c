#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <pthread.h>

int matrix_size, iterations, thread_number;
double temp, *local_max_diff, *original_grid, *new_grid, max_diff = 0;
pthread_barrier_t barrier;

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

void *get_max_diff(void *arg);

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("You must specify:\n\t- thread number\n\t- matrix size\n\t- number of iterations\n");
    exit(1);
  }

  thread_number = atoi(argv[1]);
  pthread_t threads[thread_number];
  pthread_barrier_init(&barrier, NULL, thread_number);
  int ids[thread_number];
  local_max_diff = (double *)malloc(sizeof(double) * thread_number);
  // The argument represent the size of the matriz internal points and add 1 for the border
  matrix_size = atoi(argv[2]) + 1, iterations = atoi(argv[3]);

  original_grid = (double *)malloc(sizeof(double) * matrix_size * matrix_size);
  new_grid = (double *)malloc(sizeof(double) * matrix_size * matrix_size);

  for (int i = 0; i < matrix_size; i++)
  {
    for (int j = 0; j < matrix_size; j++)
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
  for (int i = 0; i < thread_number; i++)
  {
    ids[i] = i;
    pthread_create(&threads[i], NULL, get_max_diff, &ids[i]);
  }

  // Wait for all threads
  for (int i = 0; i < thread_number; i++)
    pthread_join(threads[i], NULL);

  for (int i = 0; i < thread_number; i++)
  {
    if (local_max_diff[i] > max_diff)
      max_diff = local_max_diff[i];
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

void *get_max_diff(void *arg)
{
  int tid = *(int *)arg,
      start = matrix_size / thread_number * tid,
      limit = (matrix_size / thread_number) * (tid + 1),
      i, j, iters;

  if (start == 0)
  {
    start = 1;
  }

  if (limit == matrix_size)
  {
    limit = limit - 1;
  }

  for (iters = 1; iters <= iterations; iters = iters + 2)
  {
    for (i = start; i < limit; i = i + 1)
    {
      for (j = start; j < limit; j = j + 1)
      {
        new_grid[i * matrix_size + j] = (original_grid[i * matrix_size + j + 1] + original_grid[i * matrix_size + j - 1] + original_grid[(i + 1) * matrix_size + j] + original_grid[(i - 1) * matrix_size + j]) * 0.25;
      }
    }

    pthread_barrier_wait(&barrier);

    for (i = start; i < limit; i = i + 1)
    {
      for (j = start; j < limit; j = j + 1)
      {
        original_grid[i * matrix_size + j] = (new_grid[i * matrix_size + j + 1] + new_grid[i * matrix_size + j - 1] + new_grid[(i + 1) * matrix_size + j] + new_grid[(i - 1) * matrix_size + j]) * 0.25;
      }
    }
    pthread_barrier_wait(&barrier);
  }

  double temp;
  for (i = start; i < limit; i = i + 1)
  {
    for (j = start; j < limit; j = j + 1)
    {
      temp = original_grid[i * matrix_size + j] - new_grid[i * matrix_size + j];
      if (temp < 0)
        temp = -temp;
      if (temp > local_max_diff[tid])
        local_max_diff[tid] = temp;
    }
  }
  return 0;
}