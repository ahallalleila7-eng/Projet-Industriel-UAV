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
 
// paire la plus proche trouvee jusqu'ici
Drone *meilleur1 = NULL;
Drone *meilleur2 = NULL;
 
// carre de la distance (evite sqrt inutile)
float distCarre(Drone *a, Drone *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx*dx + dy*dy + dz*dz;
}
 
// pour trier par X
int trierX(const void *a, const void *b) {
    Drone *d1 = (Drone *)a;
    Drone *d2 = (Drone *)b;
    if (d1->x < d2->x) return -1;
    if (d1->x > d2->x) return 1;
    return 0;
}
 
// pour trier les pointeurs par Y (bande centrale)
int trierY(const void *a, const void *b) {
    Drone *d1 = *(Drone **)a;
    Drone *d2 = *(Drone **)b;
    if (d1->y < d2->y) return -1;
    if (d1->y > d2->y) return 1;
    return 0;
}
 
// algorithme diviser pour regner
// retourne le carre de la distance minimale dans le sous-tableau
float plusProche(Drone *tab, int n) {
 
    // cas de base : on compare directement
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
 
    int mid = n / 2;
    float xMid = (tab + mid)->x;
 
    // recursion gauche
    float dGauche = plusProche(tab, mid);
    Drone *sauv1 = meilleur1;
    Drone *sauv2 = meilleur2;
 
    // recursion droite
    float dDroite = plusProche(tab + mid, n - mid);
 
    // garder le meilleur des deux cotes
    float dmin;
    if (dGauche <= dDroite) {
        dmin = dGauche;
        meilleur1 = sauv1;
        meilleur2 = sauv2;
    } else {
        dmin = dDroite;
    }
 
    // verifier la bande centrale (drones proches de la frontiere)
    float delta = sqrtf(dmin);
 
    Drone **bande = (Drone **)malloc(n * sizeof(Drone *));
    if (!bande) { fprintf(stderr, "malloc bande echoue\n"); exit(1); }
 
    int nb = 0;
    for (int i = 0; i < n; i++) {
        if (fabsf((tab + i)->x - xMid) < delta) {
            *(bande + nb) = tab + i;
            nb++;
        }
    }
 
    qsort(bande, nb, sizeof(Drone *), trierY);
 
    // dans la bande, 7 comparaisons suffisent (preuve geometrique)
    for (int i = 0; i < nb - 1; i++) {
        for (int j = i + 1; j < nb && j <= i + 7; j++) {
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
 
    Drone *essaim = (Drone *)malloc(n * sizeof(Drone));
    if (!essaim) {
        printf("Erreur allocation memoire\n");
        return 1;
    }
 
    // initialisation aleatoire sans crochets
    srand((unsigned int)time(NULL));
    for (int i = 0; i < n; i++) {
        (essaim + i)->id = i + 1;
        (essaim + i)->x  = (float)(rand() % 10000);
        (essaim + i)->y  = (float)(rand() % 10000);
        (essaim + i)->z  = (float)(rand() % 10000);
    }
 
    // tri par X : etape obligatoire avant le diviser pour regner
    qsort(essaim, n, sizeof(Drone), trierX);
 
    // lancement de l'algorithme
    float dCarreMin = plusProche(essaim, n);
    float distMin = sqrtf(dCarreMin);
 
    clock_t fin = clock();
    double temps = (double)(fin - debut) / CLOCKS_PER_SEC;
 
    printf("===== SYSTEME DE COLLISION UAV =====\n\n");
    printf("Drone 1 : ID=%d | x=%.2f | y=%.2f | z=%.2f\n",
           meilleur1->id, meilleur1->x, meilleur1->y, meilleur1->z);
    printf("Drone 2 : ID=%d | x=%.2f | y=%.2f | z=%.2f\n",
           meilleur2->id, meilleur2->x, meilleur2->y, meilleur2->z);
    printf("\nDistance minimale : %.4f\n", distMin);
    printf("Temps d'execution : %.6f secondes\n", temps);
 
    free(essaim);
    return 0;
}
