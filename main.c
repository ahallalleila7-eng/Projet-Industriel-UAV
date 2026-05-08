#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct {
    int id;
    float x;
    float y;
    float z;
} Drone;

// paire la plus proche trouvee
Drone *meilleur1 = NULL;
Drone *meilleur2 = NULL;

// distance au carre
float distCarre(Drone *a, Drone *b) {

    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;

    return dx * dx + dy * dy + dz * dz;
}

// tri par X
int trierX(const void *a, const void *b) {

    Drone *d1 = (Drone *)a;
    Drone *d2 = (Drone *)b;

    if (d1->x < d2->x)
        return -1;

    if (d1->x > d2->x)
        return 1;

    return 0;
}

// tri des pointeurs par Y
int trierY(const void *a, const void *b) {

    Drone *d1 = *(Drone **)a;
    Drone *d2 = *(Drone **)b;

    if (d1->y < d2->y)
        return -1;

    if (d1->y > d2->y)
        return 1;

    return 0;
}

// algorithme divide and conquer
float plusProche(Drone *tab, int n) {

    // cas de base
    if (n <= 3) {

        float dmin = FLT_MAX;

        for (int i = 0; i < n - 1; i++) {

            for (int j = i + 1; j < n; j++) {

                float d = distCarre(tab + i, tab + j);

                if (d < dmin) {

                    dmin = d;

                    meilleur1 = tab + i;
                    meilleur2 = tab + j;
                }
            }
        }

        return dmin;
    }

    // division
    int mid = n / 2;

    float xMilieu = (tab + mid)->x;

    // partie gauche
    float dGauche = plusProche(tab, mid);

    Drone *save1 = meilleur1;
    Drone *save2 = meilleur2;

    // partie droite
    float dDroite = plusProche(tab + mid, n - mid);

    float dmin;

    // garder la meilleure paire
    if (dGauche <= dDroite) {

        dmin = dGauche;

        meilleur1 = save1;
        meilleur2 = save2;
    }
    else {

        dmin = dDroite;
    }

    // largeur de la bande centrale
    float delta = sqrtf(dmin);

    // allocation bande
    Drone **bande = (Drone **)malloc(n * sizeof(Drone *));

    if (!bande) {

        printf("Erreur allocation bande\n");
        exit(1);
    }

    int nb = 0;

    // construction bande
    for (int i = 0; i < n; i++) {

        if (fabsf((tab + i)->x - xMilieu) < delta) {

            *(bande + nb) = tab + i;
            nb++;
        }
    }

    // tri par Y
    qsort(bande, nb, sizeof(Drone *), trierY);

    // verification bande centrale
    for (int i = 0; i < nb - 1; i++) {

        for (int j = i + 1; j < nb; j++) {

            // optimisation
            if ((*(bande + j))->y - (*(bande + i))->y >= delta)
                break;

            float d = distCarre(*(bande + i), *(bande + j));

            if (d < dmin) {

                dmin = d;

                meilleur1 = *(bande + i);
                meilleur2 = *(bande + j);
            }
        }
    }

    free(bande);

    return dmin;
}

int main() {

    clock_t debut = clock();

    int n = 10000;

    if (n < 2) {

        printf("Pas assez de drones\n");
        return 1;
    }

    // allocation dynamique
    Drone *essaim = (Drone *)malloc(n * sizeof(Drone));

    if (!essaim) {

        printf("Erreur allocation memoire\n");
        return 1;
    }

    // generation aleatoire
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

    // tri obligatoire par X
    qsort(essaim, n, sizeof(Drone), trierX);

    // recherche paire minimale
    float dCarreMin = plusProche(essaim, n);

    float distMin = sqrtf(dCarreMin);

    clock_t fin = clock();

    double temps =
        (double)(fin - debut) / CLOCKS_PER_SEC;

    // affichage
    printf("===== SYSTEME DE COLLISION UAV =====\n\n");

    printf(
        "Drone 1 : "
        "ID=%d | x=%.2f | y=%.2f | z=%.2f\n",

        meilleur1->id,
        meilleur1->x,
        meilleur1->y,
        meilleur1->z
    );

    printf(
        "Drone 2 : "
        "ID=%d | x=%.2f | y=%.2f | z=%.2f\n",

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
        "Temps d'execution : %.6f secondes\n",
        temps
    );

    free(essaim);

    return 0;
}
