#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

/* Structure d'un drone */
typedef struct {

    int id;
    float x;
    float y;
    float z;

} Drone;

/* Deux drones les plus proches */
Drone *meilleur1 = NULL;
Drone *meilleur2 = NULL;

/* -----------------------------------------
   Distance au carre entre deux drones
   ----------------------------------------- */
float distCarre(Drone *a, Drone *b) {

    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;

    return dx * dx + dy * dy + dz * dz;
}

/* -----------------------------------------
   Tri des drones selon X
   ----------------------------------------- */
int trierX(const void *a, const void *b) {

    Drone *d1 = (Drone *)a;
    Drone *d2 = (Drone *)b;

    if (d1->x < d2->x)
        return -1;

    if (d1->x > d2->x)
        return 1;

    return 0;
}

/* -----------------------------------------
   Tri des pointeurs selon Y
   ----------------------------------------- */
int trierY(const void *a, const void *b) {

    Drone *d1 = *(Drone **)a;
    Drone *d2 = *(Drone **)b;

    if (d1->y < d2->y)
        return -1;

    if (d1->y > d2->y)
        return 1;

    return 0;
}

/* -----------------------------------------
   Recherche de la paire la plus proche
   ----------------------------------------- */
float plusProche(Drone *tab, int n) {

    /* Cas simple */
    if (n <= 3) {

        float dmin = FLT_MAX;

        for (int i = 0; i < n - 1; i++) {

            for (int j = i + 1; j < n; j++) {

                float d =
                    distCarre(tab + i, tab + j);

                if (d < dmin) {

                    dmin = d;

                    meilleur1 = tab + i;
                    meilleur2 = tab + j;
                }
            }
        }

        return dmin;
    }

    /* Separation */
    int milieu = n / 2;

    float xMilieu =
        (tab + milieu)->x;

    /* Partie gauche */
    float dGauche =
        plusProche(tab, milieu);

    Drone *save1 = meilleur1;
    Drone *save2 = meilleur2;

    /* Partie droite */
    float dDroite =
        plusProche(tab + milieu,
                   n - milieu);

    /* Distance minimale */
    float dmin;

    if (dGauche <= dDroite) {

        dmin = dGauche;

        meilleur1 = save1;
        meilleur2 = save2;
    }
    else {

        dmin = dDroite;
    }

    /* Distance actuelle */
    float delta = sqrtf(dmin);

    /* Zone centrale */
    Drone **zone =
        (Drone **)malloc(
            n * sizeof(Drone *)
        );

    if (!zone) {

        printf("Erreur memoire\n");
        exit(1);
    }

    int nb = 0;

    /* Drones proches du milieu */
    for (int i = 0; i < n; i++) {

        if (fabsf(
            (tab + i)->x - xMilieu
            ) < delta) {

            *(zone + nb) = tab + i;

            nb++;
        }
    }

    /* Tri selon Y */
    qsort(zone,
          nb,
          sizeof(Drone *),
          trierY);

    /* Verification */
    for (int i = 0; i < nb - 1; i++) {

        for (int j = i + 1; j < nb; j++) {

            float dy =
                (*(zone + j))->y -
                (*(zone + i))->y;

            /* inutile d'aller plus loin */
            if (dy >= delta)
                break;

            float d =
                distCarre(
                    *(zone + i),
                    *(zone + j)
                );

            if (d < dmin) {

                dmin = d;

                meilleur1 =
                    *(zone + i);

                meilleur2 =
                    *(zone + j);

                delta = sqrtf(dmin);
            }
        }
    }

    free(zone);

    return dmin;
}

/* -----------------------------------------
   Programme principal
   ----------------------------------------- */
int main() {

    clock_t debut = clock();

    int n = 10000;

    if (n < 2) {

        printf("Pas assez de drones\n");

        return 1;
    }

    /* Allocation dynamique */
    Drone *essaim =
        (Drone *)malloc(
            n * sizeof(Drone)
        );

    if (!essaim) {

        printf("Erreur memoire\n");

        return 1;
    }

    /* Valeurs aleatoires */
    srand((unsigned int)time(NULL));

    for (int i = 0; i < n; i++) {

        (essaim + i)->id = i + 1;

        (essaim + i)->x =
            (float)(rand() % 10000);

        (essaim + i)->y =
            (float)(rand() % 10000);

        (essaim + i)->z =
            (float)(rand() % 10000);
    }

    /* Tri selon X */
    qsort(essaim,
          n,
          sizeof(Drone),
          trierX);

    /* Recherche */
    float dCarreMin =
        plusProche(essaim, n);

    float distMin =
        sqrtf(dCarreMin);

    clock_t fin = clock();

    double temps =
        (double)(fin - debut)
        / CLOCKS_PER_SEC;

    /* Resultat */
    printf(
        "===== SYSTEME UAV =====\n\n"
    );

    printf(
        "Drone 1 : "
        "ID=%d | x=%.2f | "
        "y=%.2f | z=%.2f\n",

        meilleur1->id,
        meilleur1->x,
        meilleur1->y,
        meilleur1->z
    );

    printf(
        "Drone 2 : "
        "ID=%d | x=%.2f | "
        "y=%.2f | z=%.2f\n",

        meilleur2->id,
        meilleur2->x,
        meilleur2->y,
        meilleur2->z
    );

    printf(
        "\nDistance minimale : %.4f\n",
        distMin
    );

    printf(
        "Temps execution : %.6f secondes\n",
        temps
    );

    free(essaim);

    return 0;
}
