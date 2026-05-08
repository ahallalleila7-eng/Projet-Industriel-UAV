#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    int id;
    float x;
    float y;
    float z;
} Drone;

float distanceDrone(Drone *a, Drone *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

int comparerX(const void *a, const void *b) {
    Drone *d1 = (Drone *)a;
    Drone *d2 = (Drone *)b;

    if (d1->x < d2->x) return -1;
    if (d1->x > d2->x) return 1;
    return 0;
}

int main() {
    clock_t debut = clock();

    int n = 10000;

    Drone *essaim = (Drone *)malloc(n * sizeof(Drone));

    if (essaim == NULL) {
        printf("Erreur : allocation memoire impossible.\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        (*(essaim + i)).id = i + 1;
        (*(essaim + i)).x = (float)(rand() % 10000);
        (*(essaim + i)).y = (float)(rand() % 10000);
        (*(essaim + i)).z = (float)(rand() % 10000);
    }

    qsort(essaim, n, sizeof(Drone), comparerX);

    Drone *drone1 = essaim;
    Drone *drone2 = essaim + 1;

    float distanceMin = distanceDrone(drone1, drone2);

    for (int i = 0; i < n - 1; i++) {
        float d = distanceDrone(essaim + i, essaim + i + 1);

        if (d < distanceMin) {
            distanceMin = d;
            drone1 = essaim + i;
            drone2 = essaim + i + 1;
        }
    }

    clock_t fin = clock();

    double tempsExecution = ((double)(fin - debut)) / CLOCKS_PER_SEC;

    printf("===== SYSTEME DE COLLISION UAV =====\n\n");

    printf("Drone 1 : ID = %d | x = %.2f | y = %.2f | z = %.2f\n",
           drone1->id, drone1->x, drone1->y, drone1->z);

    printf("Drone 2 : ID = %d | x = %.2f | y = %.2f | z = %.2f\n",
           drone2->id, drone2->x, drone2->y, drone2->z);

    printf("\nDistance minimale trouvee : %.2f\n", distanceMin);

    printf("Temps d'execution : %.6f secondes\n", tempsExecution);

    free(essaim);

    return 0;
}