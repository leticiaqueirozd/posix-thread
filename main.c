#Exercicio Thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 15
#define NUM_CHASSIS 5
#define NUM_ENGINES 5
#define NUM_CARS 100

pthread_mutex_t mutex_chassis, mutex_engines, mutex_cars;
pthread_cond_t cond_chassis, cond_engines, cond_cars;

int chassis_count = 0;
int engine_count = 0;
int car_count = 0;

void *build_chassis(void *arg) {
  while (car_count < NUM_CARS) {
    pthread_mutex_lock(&mutex_chassis);
    while (chassis_count >= NUM_CHASSIS) {
      pthread_cond_wait(&cond_chassis, &mutex_chassis);
    }
    chassis_count++;
    printf("Chassis montado!\n");
    pthread_mutex_unlock(&mutex_chassis);
    pthread_cond_signal(&cond_engines);
    usleep(rand() % 3000000 + 1000000);
  }
  return NULL;
}

void *install_engine(void *arg) {
  while (car_count < NUM_CARS) {
    pthread_mutex_lock(&mutex_engines);
    while (engine_count >= NUM_ENGINES || chassis_count == 0) {
      pthread_cond_wait(&cond_engines, &mutex_engines);
    }
    engine_count++;
    chassis_count--;
    printf("Motor instalado!\n");
    pthread_mutex_unlock(&mutex_engines);
    pthread_cond_signal(&cond_cars);
    usleep(rand() % 3000000 + 1000000);
  }
  return NULL;
}

void *assemble_car(void *arg) {
  while (car_count < NUM_CARS) {
    pthread_mutex_lock(&mutex_cars);
    while (car_count >= NUM_CARS || engine_count == 0) {
      pthread_cond_wait(&cond_cars, &mutex_cars);
    }
    car_count++;
    engine_count--;
    printf("Carro completo!\n");
    pthread_mutex_unlock(&mutex_cars);
    usleep(rand() % 3000000 + 1000000);
  }
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_THREADS];
  int i;
  srand(time(NULL));
  pthread_mutex_init(&mutex_chassis, NULL);
  pthread_mutex_init(&mutex_engines, NULL);
  pthread_mutex_init(&mutex_cars, NULL);
  pthread_cond_init(&cond_chassis, NULL);
  pthread_cond_init(&cond_engines, NULL);
  pthread_cond_init(&cond_cars, NULL);
  for (i = 0; i < NUM_THREADS / 3; i++) {
    pthread_create(&threads[i], NULL, build_chassis, NULL);
  }
  for (i = NUM_THREADS / 3; i < 2 * NUM_THREADS / 3; i++) {
    pthread_create(&threads[i], NULL, install_engine, NULL);
  }
  for (i = 2 * NUM_THREADS / 3; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, assemble_car, NULL);
  }
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&mutex_chassis);
  pthread_mutex_destroy(&mutex_engines);
  pthread_mutex_destroy(&mutex_cars);
  pthread_cond_destroy(&cond_chassis);
  pthread_cond_destroy(&cond_engines);
  pthread_cond_destroy(&cond_cars);
  printf("Produção finalizada!\n");
  return 0;
}