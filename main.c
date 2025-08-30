#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 256
#define SIZE 49999
#define SIZE_INGREDIENTI 131113

typedef struct ingredienti{
    char* nome_ingrediente;
    int quantita;
    struct ingredienti * next;
} ingredienti;

typedef struct ricetta{
    char nome[256];
    ingredienti * lista_ingredienti;
    int peso;
    struct ricetta * next;
    struct ricetta * prev;
} ricetta;

typedef struct hash_ricetta{
    ricetta * lista_ricette;
} hash_ricetta;

typedef struct nodo_ingrediente{
    char nome[256];
    struct nodo_ingrediente * next;
} nodo_ingrediente;

typedef struct hash_ingredienti{
    nodo_ingrediente * lista_ingredienti;
}hash_ingredienti;

typedef struct partita{
    int quantity;
    int scadenza;
    struct partita * next;
}partita;

typedef struct lotto {
    char* nome;
    partita* partite;
    int tot_quantita;
    struct lotto * next;
    struct lotto * prev;
} lotto;

typedef struct hash_lotti{
    lotto* lista_lotti;
} hash_lotti;

typedef struct lista_ordini{
    ricetta * nome_ricetta;
    short int q_ordine;
    int arrivo;
    int peso;
    struct lista_ordini * next;
    struct lista_ordini * prev;
} lista_ordini;

typedef struct struttura_ordini{
    lista_ordini * testa_ordini;
    lista_ordini * ultimo;
}struttura_ordini;

int tempo = 0;

int funzione_hash(char*, short int );
char* aggiungi_ingrediente(hash_ingredienti**, char*);
char* cerca_ingrediente (hash_ingredienti**, char *);
void aggiungi_ricetta (hash_ricetta ** , hash_ingredienti** , FILE *);
ricetta * cerca_ricetta(hash_ricetta**, char*);
void rimuovi_ricetta(hash_ricetta**, struttura_ordini *, struttura_ordini *, hash_lotti **, FILE*);
void rifornimento(char*, int, int, hash_lotti **, hash_ingredienti**);
lotto * cerca_lotto (hash_lotti*, char*);
void rimuovi_lotto(hash_lotti **, lotto*);
void ordine(struttura_ordini ** ,struttura_ordini**, hash_ricetta **, hash_lotti **, FILE *);
void inserisci_ordine(struttura_ordini ** , struttura_ordini **, ricetta* , int , hash_ricetta ** , hash_lotti **);
void inserisci_dalla_testa(struttura_ordini ** , lista_ordini *);
void inserisci_dalla_coda(struttura_ordini ** , lista_ordini *);
int search_ordine(struttura_ordini *, struttura_ordini*, char*);
void controlla_ordine (struttura_ordini ** , struttura_ordini ** , hash_lotti ** );
int is_ready(hash_lotti **, lista_ordini *);
void rimuovi_ordine (struttura_ordini ** , lista_ordini *);
void stampa_camioncino(struttura_ordini**, int);
void aggiungi_lista_ordini(struttura_ordini ** , lista_ordini *);

//Funzione di hash
/**Tested: ok*/
int funzione_hash(char* str, short int m) {
    unsigned int hash = 5381;
    int c;
    int size =0;
    if(m==0)
        size = SIZE_INGREDIENTI;
    else
        size=SIZE;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % size;
}

/**Tested: ok*/
void aggiungi_ricetta(hash_ricetta ** tabella_ricette, hash_ingredienti** tabella_ingredienti, FILE * file) {
    char nome_ricetta[256];

    if(fscanf(file,"%255s", nome_ricetta)!=1)
        return;

    //se la ricetta è già nel catalogo
    if (cerca_ricetta(tabella_ricette, nome_ricetta) != NULL) {
        char ch;
        while ((ch = fgetc(file)) != EOF && ch != '\n') {
            //legge tutta la riga
        }
        printf("ignorato\n");
        return;
    }

    //altrimenti la inserisco e inserisco i suoi ingredienti
    ricetta * new_ricetta = (ricetta*) malloc(sizeof(ricetta));
    strcpy(new_ricetta->nome, nome_ricetta);
    new_ricetta->next = NULL;
    new_ricetta->prev= NULL;

    new_ricetta->lista_ingredienti = (ingredienti*) malloc(sizeof(ingredienti));
    new_ricetta->lista_ingredienti = NULL;

    char nome_ingrediente[256];
    int q = 0;
    int peso = 0;
    while(fgetc(file) != '\n') {
        if(fscanf(file, "%255s", nome_ingrediente) != 1)
            return;

        if(fscanf(file, "%d", &q) != 1)
            return;

        ingredienti *new = (ingredienti *) malloc(sizeof(ingredienti));
        new->next = NULL;

        if (cerca_ingrediente(tabella_ingredienti, nome_ingrediente) != NULL)
            new->nome_ingrediente = cerca_ingrediente(tabella_ingredienti, nome_ingrediente);
        else {
            new->nome_ingrediente= aggiungi_ingrediente(tabella_ingredienti, nome_ingrediente);
        }
        new->quantita = q;
        peso= peso+q;
        //inserisce l'ingrediente dentro la lista degli ingredienti della ricetta
        new->next = new_ricetta->lista_ingredienti;
        new_ricetta->lista_ingredienti = new;
        }

        //inserisco ora la ricetta nella tabella di hash
        short int is_ricetta=1;
        int indice= funzione_hash(nome_ricetta, is_ricetta);

        if((*tabella_ricette)[indice].lista_ricette == NULL){
            (*tabella_ricette)[indice].lista_ricette = new_ricetta;
        }

        else{
            new_ricetta->next =  (*tabella_ricette)[indice].lista_ricette;
            (*tabella_ricette)[indice].lista_ricette = new_ricetta;

        }
        new_ricetta->peso = peso;
        printf("aggiunta\n");
    }

/**Tested: ok*/
//aggiunge l'ingrediente in una tabella di hash
char* aggiungi_ingrediente (hash_ingredienti ** tabella_ingredienti, char* nome_ingrediente){
    short int is_ricetta=0;
    unsigned int indice = funzione_hash(nome_ingrediente, is_ricetta);

    nodo_ingrediente * new_ingrediente = (nodo_ingrediente *) malloc(sizeof(nodo_ingrediente));
    strcpy(new_ingrediente->nome, nome_ingrediente);
    new_ingrediente->next = NULL;

    if((*tabella_ingredienti)[indice].lista_ingredienti == NULL) {
        (*tabella_ingredienti)[indice].lista_ingredienti = new_ingrediente;
    }

    else {
        new_ingrediente -> next = (*tabella_ingredienti)[indice].lista_ingredienti;
        (*tabella_ingredienti)[indice].lista_ingredienti = new_ingrediente;
    }

    return new_ingrediente->nome;
}

/**Tested: ok*/
char* cerca_ingrediente(hash_ingredienti ** tabella_ingredienti, char* nome_ingrediente){
    short int is_ricetta =0;
    unsigned int indice = funzione_hash(nome_ingrediente, is_ricetta);

    if((*tabella_ingredienti)[indice].lista_ingredienti != NULL){
        nodo_ingrediente * temp = (*tabella_ingredienti)[indice].lista_ingredienti;

        while(temp != NULL){
            if(strcmp(temp->nome, nome_ingrediente) == 0)
                return temp -> nome;

            temp = temp->next;
        }
    }
    return NULL;
}

/**Tested: ok*/
//cerca la ricetta in una tabella di hash
ricetta * cerca_ricetta(hash_ricetta **tabella_ricette, char* nome_ricetta){
    unsigned int indice;
    short int is_ricetta =1;
    indice = funzione_hash(nome_ricetta, is_ricetta);

    if((*tabella_ricette)[indice].lista_ricette != NULL){
        ricetta * temp = (*tabella_ricette)[indice].lista_ricette;

        while(temp != NULL){

            if(strcmp(temp->nome, nome_ricetta)== 0)
                return temp;

            temp = temp -> next;
        }
    }
    return NULL;
}

/**Tested: ok*/
//rimuove una ricetta dalla tabella di hash
void rimuovi_ricetta(hash_ricetta **tabella_ricette, struttura_ordini *ordini_attesa, struttura_ordini *ordini_pronti, hash_lotti **magazzino, FILE *file) {

    char old_ricetta[MAX_LENGTH];

    if (fscanf(file, "%s", old_ricetta) != 1) {
        return;
    }

    unsigned int indice;
    short int is_ricetta = 1;
    indice = funzione_hash(old_ricetta, is_ricetta);

    ricetta *temp = (*tabella_ricette)[indice].lista_ricette;
    ricetta *prev = NULL;

    //trova la ricetta da rimuovere nella lista
    while (temp != NULL) {
        if (strcmp(temp->nome, old_ricetta) == 0) {

            //verifica gli ordini in sospeso
            if (search_ordine(ordini_attesa, ordini_pronti, old_ricetta) != -1) {
                printf("ordini in sospeso\n");
                return;
            }

            //la rimuove
            if (prev == NULL) {
                (*tabella_ricette)[indice].lista_ricette = temp->next;
            } else {
                prev->next = temp->next;
            }

            printf("rimossa\n");
            return;
        }

        prev = temp;
        temp = temp->next;
    }

    //se arriva qui non l'ha trovata
    printf("non presente\n");
}

/**Tested: ok*/
//inserisce un lotto in una tabella di hash
void rifornimento(char* nome_ingrediente, int quantity, int scadenza, hash_lotti **magazzino, hash_ingredienti **tabella_ingredienti) {
    if (scadenza <= tempo)
        return;

    short int is_ricetta = 0;
    unsigned int indice = funzione_hash(nome_ingrediente, is_ricetta);

    lotto *temp = cerca_lotto(&(*magazzino)[indice], nome_ingrediente);

    if (temp == NULL) {
        lotto *new_lotto = (lotto *) malloc(sizeof(lotto));
        new_lotto->partite = NULL;

        if (cerca_ingrediente(tabella_ingredienti, nome_ingrediente) != NULL)
            new_lotto->nome = cerca_ingrediente(tabella_ingredienti, nome_ingrediente);
        else {
            new_lotto->nome = aggiungi_ingrediente(tabella_ingredienti, nome_ingrediente);
        }

        //creo la partita e la inserisco
        partita *new_partita = (partita *) malloc(sizeof(partita));
        new_partita->quantity = quantity;
        new_partita->scadenza = scadenza;
        new_partita->next = NULL;
        new_lotto->partite = new_partita;
        new_lotto->tot_quantita = quantity;


        new_lotto->next = (*magazzino)[indice].lista_lotti;
        new_lotto->prev = NULL;

        if ((*magazzino)[indice].lista_lotti != NULL) {
            (*magazzino)[indice].lista_lotti->prev = new_lotto;
        }

        (*magazzino)[indice].lista_lotti = new_lotto;

    } else {
        //il lotto esiste già
        partita *new_partita = (partita *) malloc(sizeof(partita));
        new_partita->quantity = quantity;
        new_partita->scadenza = scadenza;
        new_partita->next = NULL;
        temp->tot_quantita += quantity;

        //inserisco in ordine di scadenza
        partita *current = temp->partite;
        partita *prev = NULL;
        while (current != NULL && current->scadenza < scadenza) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            new_partita->next = temp->partite;
            temp->partite = new_partita;
        } else {
            prev->next = new_partita;
            new_partita->next = current;
        }
    }
}

/**Tested: ok*/
void aggiorna_scadenze(lotto* lotto){
    partita *temp = lotto->partite;

    while(temp!= NULL && temp->scadenza<=tempo){
        lotto->tot_quantita= lotto->tot_quantita - temp->quantity;
        temp= temp->next;
    }
    lotto->partite = temp;
}

//controlla che nel magazzino ci sia una quantità sufficiente, aggiornando anche le scadenze nel lotto interessato
/**Tested: ok*/
int controllo_quantity(ingredienti ingredient, hash_lotti **magazzino) {
    short int is_ricetta = 0;
    unsigned int indice = funzione_hash(ingredient.nome_ingrediente, is_ricetta);

    lotto *temp = cerca_lotto(&(*magazzino)[indice], ingredient.nome_ingrediente);

    if (temp == NULL) {
        return 0;
    }

    //aggiorna le scadenze e ritorna la quantità totale
    aggiorna_scadenze(temp);
    return temp->tot_quantita;
}

/**Tested: ok*/
//rimuove il lotto dalla tabella hash
void rimuovi_lotto(hash_lotti **magazzino, lotto *prodotto) {
    if (magazzino == NULL || prodotto == NULL ||prodotto->nome == NULL)
        return;

    short int is_ricetta =0;
    unsigned int indice = funzione_hash(prodotto->nome, is_ricetta);

    if (prodotto->prev != NULL) {
        prodotto->prev->next = prodotto->next;
    } else {
        //primo della lista
        (*magazzino)[indice].lista_lotti = prodotto->next;
    }

    if (prodotto->next != NULL) {
        prodotto->next->prev = prodotto->prev;
    }
}

/**Tested: ok*/
lotto* cerca_lotto(hash_lotti *magazzino, char *nome_lotto) {
    if (magazzino == NULL || nome_lotto == NULL) {
        return NULL;
    }

    lotto *curr = magazzino->lista_lotti;

    while (curr != NULL) {
        if (strcmp(curr->nome, nome_lotto) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;  //se non trova il lotto
}

/**Tested: ok*/
//controlla se è possibile eseguire un ordine
void ordine(struttura_ordini ** ordini_attesa, struttura_ordini ** ordini_pronti, hash_ricetta ** tabella_ricette, hash_lotti ** magazzino, FILE * file){
    char nome_ricetta[256];
    int quantity;

    if(fscanf(file,"%255s", nome_ricetta)!=1)
        return;
    if(fscanf(file,"%d", &quantity)!=1)
        return;

    ricetta* r = cerca_ricetta(tabella_ricette, nome_ricetta);
    if(r != NULL) {

        inserisci_ordine(ordini_attesa, ordini_pronti, r, quantity, tabella_ricette, magazzino);
        printf("accettato\n");
        return;
    }
    else {
        printf("rifiutato\n");
        return;
    }
}

void aggiungi_lista_ordini(struttura_ordini ** lista, lista_ordini *ordine) {
    //se è vuota
    if (*lista == NULL || (*lista)->testa_ordini == NULL) {
        *lista = (struttura_ordini *)malloc(sizeof(struttura_ordini));
        (*lista)->testa_ordini = ordine;
        (*lista)->ultimo = ordine;
    } else {
        (*lista)->ultimo->next = ordine;
        ordine->prev = (*lista)->ultimo;
        (*lista)->ultimo = ordine;
    }
}

void inserisci_ordine(struttura_ordini **ordini_attesa, struttura_ordini **ordini_pronti, ricetta *ricetta, int quantity, hash_ricetta **tabella_ricette, hash_lotti **magazzino) {
    lista_ordini *new_ordine = (lista_ordini *)malloc(sizeof(lista_ordini));
    new_ordine->nome_ricetta = ricetta;
    new_ordine->q_ordine = quantity;
    new_ordine->arrivo = tempo;
    new_ordine->peso = ricetta->peso * quantity;
    new_ordine->next = NULL;
    new_ordine->prev = NULL;

    //aggiunge l'ordine
    if (!is_ready(magazzino, new_ordine)) {
        aggiungi_lista_ordini(ordini_attesa, new_ordine);
    } else {
        aggiungi_lista_ordini(ordini_pronti, new_ordine);
    }
}

//cerca un ordine (usato per rimuovi ricetta)
int search_ordine(struttura_ordini * ordini_attesa, struttura_ordini* ordini_pronti, char* nome_ricetta) {

    if(ordini_attesa != NULL) {
        //cerco nella lista degli ordini in attesa
        lista_ordini *temp = (*ordini_attesa).testa_ordini;
        while (temp != NULL) {
            if (strcmp(temp->nome_ricetta->nome, nome_ricetta) == 0)
                return 1;
            temp = temp->next;
        }
    }

    //cerco negli ordini pronti
    if(ordini_pronti != NULL) {
        lista_ordini * temp = (*ordini_pronti).testa_ordini;
        while (temp != NULL) {
            if (strcmp(temp->nome_ricetta->nome, nome_ricetta) == 0)
                return 1;
            temp = temp->next;
        }
    }
    return -1;
}

void controlla_ordine(struttura_ordini **ordini_attesa, struttura_ordini **ordini_pronti, hash_lotti **magazzino) {

    if (*ordini_attesa == NULL) {
        return;
    }

    lista_ordini *temp = (*ordini_attesa)->testa_ordini;

    while (temp != NULL) {
        lista_ordini *next_temp = temp->next;

        //se è pronto
        if (is_ready(magazzino, temp)) {
            rimuovi_ordine(ordini_attesa, temp);
            if (*ordini_pronti== NULL ||(*ordini_pronti)->testa_ordini == NULL) {
                //crea una lista
                *ordini_pronti = (struttura_ordini *)malloc(sizeof(struttura_ordini));
                (*ordini_pronti)->testa_ordini = temp;
                (*ordini_pronti)->ultimo = temp;
                temp->next = NULL;
                temp->prev = NULL;
            } else {
                if ((*ordini_pronti)->testa_ordini->arrivo > temp->arrivo) {
                    //in testa
                    temp->next = (*ordini_pronti)->testa_ordini;
                    temp->prev = NULL;
                    (*ordini_pronti)->testa_ordini->prev = temp;
                    (*ordini_pronti)->testa_ordini = temp;
                } else if ((*ordini_pronti)->ultimo->arrivo < temp->arrivo) {
                    //alla fine della lista
                    (*ordini_pronti)->ultimo->next = temp;
                    temp->prev = (*ordini_pronti)->ultimo;
                    (*ordini_pronti)->ultimo = temp;
                    temp->next = NULL;
                } else {
                    //in mezzo
                    if (abs((*ordini_pronti)->testa_ordini->arrivo - temp->arrivo) <= abs((*ordini_pronti)->ultimo->arrivo - temp->arrivo)) {
                        inserisci_dalla_testa(ordini_pronti, temp);
                    } else {
                        inserisci_dalla_coda(ordini_pronti, temp);
                    }
                }
            }
            temp = next_temp;
        } else {
            temp = next_temp;
        }
    }
}

void inserisci_dalla_testa(struttura_ordini ** ordini_pronti, lista_ordini *ordine_aggiunto){

    lista_ordini *curr = (*ordini_pronti)->testa_ordini;
    while (curr->next != NULL && curr -> next -> arrivo < ordine_aggiunto->arrivo){
        curr = curr -> next;
    }

    ordine_aggiunto->next = curr->next;
    ordine_aggiunto->prev = curr;

    if(curr->next != NULL)
        curr->next->prev = ordine_aggiunto;

    curr -> next = ordine_aggiunto;

}

void inserisci_dalla_coda(struttura_ordini ** ordini_pronti, lista_ordini * ordine_aggiunto){
    lista_ordini *curr = (*ordini_pronti)->ultimo;
    while (curr->prev != NULL && curr -> prev -> arrivo >= ordine_aggiunto->arrivo){
        curr = curr -> prev;
    }

    ordine_aggiunto->prev = curr->prev;
    ordine_aggiunto->next = curr;

    if(curr->prev != NULL)
        curr->prev->next = ordine_aggiunto;

    curr->prev=ordine_aggiunto;

}

int is_ready(hash_lotti **magazzino, lista_ordini *ordine) {
    ricetta *ricetta_ordine = ordine->nome_ricetta;
    ingredienti *ingredienti_ordine = ricetta_ordine->lista_ingredienti;
    int quantita_magazzino;
    int peso_ingrediente;

    //controllo se la quantità è sufficiente nel magazzino
    while (ingredienti_ordine != NULL) {
        quantita_magazzino = controllo_quantity(*ingredienti_ordine, magazzino);
        peso_ingrediente = ingredienti_ordine->quantita * ordine->q_ordine;

        if (quantita_magazzino < peso_ingrediente) {
            return 0;
        }

        ingredienti_ordine = ingredienti_ordine->next;
    }

    //scalo le quantità
    ingredienti_ordine = ricetta_ordine->lista_ingredienti;
    while (ingredienti_ordine != NULL) {
        unsigned int indice = funzione_hash(ingredienti_ordine->nome_ingrediente, 0);
        lotto *temp = cerca_lotto(&(*magazzino)[indice], ingredienti_ordine->nome_ingrediente);
        lotto *temp_next;

        peso_ingrediente = ingredienti_ordine->quantita * ordine->q_ordine;

        while (temp != NULL && peso_ingrediente > 0) {
            temp_next = temp->next;

            if (strcmp(temp->nome, ingredienti_ordine->nome_ingrediente) == 0) {
                partita *current_partita = temp->partite;
                partita *prev_partita = NULL;

                while (current_partita != NULL && peso_ingrediente > 0) {
                    if (peso_ingrediente <= current_partita->quantity) {
                        current_partita->quantity -= peso_ingrediente;
                        temp->tot_quantita -= peso_ingrediente;

                        if (current_partita->quantity <= 0) {
                            if (prev_partita == NULL) {
                                temp->partite = current_partita->next;
                            } else {
                                prev_partita->next = current_partita->next;
                            }
                        }
                        peso_ingrediente = 0;
                    } else {
                        temp->tot_quantita -= current_partita->quantity;
                        peso_ingrediente -= current_partita->quantity;

                        if (prev_partita == NULL) {
                            temp->partite = current_partita->next;
                        } else {
                            prev_partita->next = current_partita->next;
                        }
                        current_partita = (prev_partita == NULL) ? temp->partite : prev_partita->next;
                        continue;
                    }
                    prev_partita = current_partita;
                    current_partita = current_partita->next;
                }

                if (temp->partite == NULL) {
                    rimuovi_lotto(magazzino, temp);
                }
            }
            temp = temp_next;
        }
        ingredienti_ordine = ingredienti_ordine->next;
    }
    return 1;
}

void rimuovi_ordine(struttura_ordini **ordini, lista_ordini *ordine_rimosso) {
    if (*ordini == NULL || ordine_rimosso == NULL) {
        return;
    }
    //se la lista contiene solo un nodo
    if ((*ordini)->testa_ordini == ordine_rimosso && (*ordini)->ultimo == ordine_rimosso) {
        (*ordini)->testa_ordini = NULL;
        (*ordini)->ultimo = NULL;
    } else if ((*ordini)->testa_ordini == ordine_rimosso) {
        //è il primo nodo della lista
        (*ordini)->testa_ordini = ordine_rimosso->next;
        if ((*ordini)->testa_ordini != NULL) {
            (*ordini)->testa_ordini->prev = NULL;
        }
    } else if (ordine_rimosso->next == NULL) {
        //è l'ultimo nodo della lista
        (*ordini)->ultimo = ordine_rimosso->prev;
        if ((*ordini)->ultimo != NULL) {
            (*ordini)->ultimo->next = NULL;
        }
    } else {
        //è un nodo intermedio
        if (ordine_rimosso->next != NULL) {
            ordine_rimosso->next->prev = ordine_rimosso->prev;
        }
        if (ordine_rimosso->prev != NULL) {
            ordine_rimosso->prev->next = ordine_rimosso->next;
        }
    }
}

//---------------MERGESORT----------------------
lista_ordini* merge(lista_ordini* a, lista_ordini* b) {
    lista_ordini* result = NULL;

    if (a == NULL) return b;
    if (b == NULL) return a;
    if (a->peso > b->peso || (a->peso == b->peso && a->arrivo < b->arrivo)) {
        result = a;
        result->next = merge(a->next, b);
        if (result->next != NULL) {
            result->next->prev = result;
        }
        result->prev = NULL;
    } else {
        result = b;
        result->next = merge(a, b->next);
        if (result->next != NULL) {
            result->next->prev = result;
        }
        result->prev = NULL;
    }
    return result;
}

void split(lista_ordini* source, lista_ordini** frontRef, lista_ordini** backRef) {
    lista_ordini* fast;
    lista_ordini* slow;

    if (source == NULL || source->next == NULL) {
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;

        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }

        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}

void mergesort(lista_ordini** headRef) {
    lista_ordini* head = *headRef;
    lista_ordini* a;
    lista_ordini* b;

    if (head == NULL || head->next == NULL) {
        return;
    }

    split(head, &a, &b);

    mergesort(&a);
    mergesort(&b);

    *headRef = merge(a, b);
}
//---------------MERGESORT-------------------

//funzione per stampare e ordinare gli ordini
void stampa_camioncino(struttura_ordini** ordini_pronti, int capacita_camioncino) {
    //verifica se la lista degli ordini pronti è vuota
    if (*ordini_pronti == NULL || (*ordini_pronti)->testa_ordini == NULL) {
        printf("camioncino vuoto\n");
        return;
    }

    int somma = 0;
    lista_ordini* temp = (*ordini_pronti)->testa_ordini;
    lista_ordini* ordini_da_stampare = NULL; //lista degli ordini da stampare
    lista_ordini* last_added = NULL;

    //scelta degli ordini pronti
    while (temp != NULL) {
        if (somma + temp->peso > capacita_camioncino) {
            break;
        }

        somma += temp->peso;

        lista_ordini* prossimo = temp->next;

        //stacca la lista dagli ordini pronti
        if (ordini_da_stampare == NULL) {
            ordini_da_stampare = temp;
        } else {
            last_added->next = temp;
            temp->prev = last_added;
        }

        last_added = temp;
        last_added->next = NULL;  //termina la lista staccata

        temp = prossimo;
    }

    //aggiorna la lista degli ordini pronti
    (*ordini_pronti)->testa_ordini = temp;
    if (temp != NULL) {
        temp->prev = NULL;
    } else {
        (*ordini_pronti)->ultimo = NULL;  //se non ci sono più ordini, aggiorna ultimo
    }

    if (ordini_da_stampare == NULL) {
        printf("camioncino vuoto\n");
        return;
    }

    //ordina per peso decrescente la lista staccata
    mergesort(&ordini_da_stampare);

    lista_ordini* curr = ordini_da_stampare;
    while (curr != NULL) {
        printf("%d %s %d\n", curr->arrivo, curr->nome_ricetta->nome, curr->q_ordine);
        curr = curr->next;
    }
}

int main() {
    //creo una tabella di hash per il nome degli ingredienti
    hash_ingredienti *tabella_ingredienti = (hash_ingredienti *) malloc(sizeof(hash_ingredienti) * SIZE_INGREDIENTI);
    for (int i = 0; i < SIZE_INGREDIENTI; ++i) {
        tabella_ingredienti[i].lista_ingredienti = NULL;
    }

    //creo hash table delle ricette
    hash_ricetta *tabella_ricette = (hash_ricetta *) malloc(sizeof(hash_ricetta) * SIZE);
    for (int i = 0; i < SIZE; ++i) {
        tabella_ricette[i].lista_ricette = NULL;
    }

    //creo il magazzino per gli ingredienti
    hash_lotti *magazzino = (hash_lotti *) malloc(SIZE_INGREDIENTI * sizeof(hash_lotti));
    for (int i = 0; i < SIZE_INGREDIENTI; ++i) {
        magazzino[i].lista_lotti= NULL;
    }

    //creo lista di ordini in attesa e pronti
    struttura_ordini * ordini_attesa = NULL;
    struttura_ordini * ordini_pronti = NULL;

    //parametri letti in ingresso
    int periodo = 0;

    int capienza;
    char input[30];
    FILE *file = stdin;

    if (file != NULL) {
        if (fscanf(file, "%d ", &periodo) != 1)
            return 0;
        if (fscanf(file, "%d \n", &capienza) != 1)
            return 0;
    }

    int conta_periodo = 0;

    while (fscanf(file, "%s", input) != EOF) {

        if (conta_periodo != 0 && (conta_periodo % periodo == 0)) {
            //CARICA E STAMPA GLI ORDINI DEL FURGONE
            stampa_camioncino(&ordini_pronti, capienza);
        }
        conta_periodo++;

        switch (input[0]) {

            //---------AGGIUNGI_RICETTA---------
            case 'a':
                //prima cerca se è già presente la ricetta nella tabella di hash
                aggiungi_ricetta(&tabella_ricette, &tabella_ingredienti, file);
                tempo++;
                break;

                //---------ORDINE------------------
            case 'o':
                ordine(&ordini_attesa, &ordini_pronti, &tabella_ricette, &magazzino, file);
                tempo ++;
                break;

            case 'r':
                switch (input[2]) {
                    //---------RIMUOVI_RICETTA-----------
                    case 'm':
                        rimuovi_ricetta(&tabella_ricette, ordini_attesa, ordini_pronti, &magazzino, file);
                        tempo++;
                        break;

                    //---------RIFORNIMENTO-------------
                    case 'f': {
                        char nome_ingrediente[MAX_LENGTH];
                        int quantity;
                        int scadenza;

                        while (fgetc(file) != '\n') {

                            if (fscanf(file, "%s ", nome_ingrediente) != 1)
                                return 0;

                            if (fscanf(file, "%d", &quantity) != 1)
                                return 0;

                            if (fscanf(file, "%d", &scadenza) != 1)
                                return 0;

                            rifornimento(nome_ingrediente, quantity, scadenza, &magazzino, &tabella_ingredienti);
                        }

                        //gestisci_scadenza(&magazzino, tempo);
                        printf("rifornito\n");
                        controlla_ordine(&ordini_attesa, &ordini_pronti, &magazzino);
                        tempo++;
                        break;
                    }
                }
                break;

            default:
                break;
        }

    }
    fclose(file);
    return 0;
}
