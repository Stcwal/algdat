#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 100
#define uendelig 1000000000

typedef struct kant_st {
    struct kant_st *neste;
    struct node_st *til;
} Kant;

typedef struct node_st {
    Kant *kant1;
    void *d;
} Node;

typedef struct {
    int N,K;
    Node *node;
} Graf;

Graf *ny_ugraf(FILE *f) {
    Graf *G = (Graf *)malloc(sizeof(Graf));
    fscanf(f, "%i %i\n", &G->N, &G->K);
    G->node = (Node *)calloc(G->N, sizeof(Node));
    for (int i=0; i < G->K; ++i) {
        int fra, til;
        Kant *k = (Kant *)malloc(sizeof(Kant));
        fscanf(f, "%i %i\n", &fra, &til);
        k->til = &G->node[til];
        k->neste = G->node[fra].kant1;
        G->node[fra].kant1 = k;
    }
    return G;
}

typedef struct {
    int dist;
    Node *forgj;
} Forgj;

Forgj *ny_forgj() {
    Forgj *f = calloc(1, sizeof(Forgj));
    f->dist = uendelig;
    return f;
}

void initforgj(Graf *G, Node *s) {
    for (int i = G->N; i--;) {
        G->node[i].d = ny_forgj();
    }
    ((Forgj *)s->d)->dist = 0;
}

typedef struct {
    void **tab;
    int start, slutt, antall, max;
} Koe;

Koe *nyKoe(int str) {
    Koe *k = (Koe *)malloc(sizeof(Koe));
    k->tab = (void *)(malloc(str*sizeof(void*)));
    k->max = str;
    k->start = k->slutt = k->antall = 0; 
    return k;
}

bool tomKoe(Koe *k) {
    return !k->antall;
}

bool fullKoe(Koe *k) {
    return k->antall == k->max;
}

void leggIKoe(Koe *k, void *e) {
    if (fullKoe(k)) return;
    k->tab[k->slutt] = e;
    k->slutt = (k->slutt+1)%k->max;
    ++k->antall;
}

void *nesteIKoe(Koe *k) {
    void *e;
    if (tomKoe(k)) return NULL;
    e = k->tab[k->start];
    k->start = (k->start+1)%k->max;
    --k->antall;
    return e;
}

void *sjekkKoe(Koe*k) {
    if (tomKoe(k)) return NULL;
    return k->tab[k->start];
}


Koe *bfs(Graf *G, Node *s) {
    initforgj(G,s);
    Koe *koe = nyKoe(G->N-1);
    leggIKoe(koe,s);
    while (!tomKoe(koe)) {
        Node *n = nesteIKoe(koe);
        for (Kant *k = n->kant1;
        k; k = k -> neste) {
            Forgj *f = k->til->d;
            if (f->dist == uendelig) {
                f->dist=((Forgj*)n->d)->dist+1;
                f->forgj = n;
                leggIKoe(koe, k->til);
            }
        }
    }
    return koe;
}

typedef struct {
    int funnet;
    Node *neste;
} Topo_lst;

Node *df_topo(Node *n, Node *l) {
    Topo_lst *nd = n->d;
    if (nd->funnet) return l;
    nd->funnet = 1;
    for (Kant *k = n->kant1; k; k = k->neste) {
        l = df_topo(k->til, l);
    }
    nd->neste = l;
    return n;
}

Node *topologisort(Graf *G) {
    Node *l = 0;
    for (int i = G->N; i--;) {
        G->node[i].d = calloc(sizeof(Topo_lst),1);
    }
    for (int i = G->N; i--;) {
        l = df_topo(&G->node[i],l);
    }
    return l;
}

int oppg1(FILE *files[]) {
    for (int i = 0; i < 4; i++) {
        printf("Graf %d:\n", i+1);
        Graf *graf = ny_ugraf(files[i]);
        Koe *koe = bfs(graf, &graf->node[i]);

        printf("Node   Forgj   Dist\n");
        for (int j = 0; j < graf->N; j++) {
            Forgj *f = (Forgj *)graf->node[j].d;
            printf("%-7d", j);
            if (f->forgj) {
                printf("%-8ld", f->forgj - graf->node); 
            } else {
                printf("%-8c", ' '); 
            }
            printf("%d\n", f->dist); 
        }
    }

    return 0;
}

int oppg2(FILE *files[]) {
    for (int i = 0; i < 2; i++) {
        printf("\nGraf %d:\n", i+1);
        Graf *graf = ny_ugraf(files[i]);
        Node *n = topologisort(graf);
        while(n) {
            printf("%ld ", (n-graf->node));
            Topo_lst *t = (Topo_lst *)n->d;
            n = t->neste;
        }
        printf("\n");
    }
    return 0;
}

int main() {
    FILE *fptr1;
    FILE *fptr2;
    FILE *fptr3;
    FILE *fptr5;
    FILE *fptr7;

    fptr1 = fopen("ø5g1.txt", "r");
    fptr2 = fopen("ø5g2.txt", "r");
    fptr3 = fopen("ø5g3.txt", "r");
    fptr5 = fopen("ø5g5.txt", "r");
    fptr7 = fopen("ø5g7.txt", "r");

    FILE *files1 [] = {fptr1, fptr2, fptr5, fptr7};
    FILE *files2 [] = {fptr5, fptr7};


    //oppg1(files1);
    oppg2(files2);

    for (int i = 0; i < 4; i++) {
        fclose(files1[i]);
    }
    return 0;
}