#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abr.h"
#include "utils.h"

/* DESCRIZIONE
   Ricerca del nodo contenente la chiave k.

   PARAMETRI DI INPUT
   Abr_t root: 	   			Radice dell'albero binario di ricerca o di un qualsiasi sottoalbero.
   const void *k:  			chiave.
   struct operations *op: 	struttura delle operationi.

   VALORI DI RITORNO
   abr_search restituisce il nodo contenente la chiave k se tale nodo esiste,
   NULL altrimenti.								*/
Abr_t abr_search(Abr_t root, const void *k, struct operations *op)
{
	if(root == NULL || op->Cmp_function(root->key, k) == 0)
		return root;
	else if(op->Cmp_function(k, root->key) < 0)
		return abr_search(root->left, k, op);
	else
		return abr_search(root->right, k, op);
}


/* DESCRIZIONE
   Inserimento di un nodo nell'albero binario di ricerca dato.

   PARAMETRI DI INPUT
   Abr_t root: 				Radice dell'albero.
   const void *key: 		chiave del nodo da inserire nell'albero.
   struct operations *op: 	Struttura delle operazioni.

   VALORI DI RITORNO
   Restituisce la radice del nuovo albero binario di ricerca.	*/
Abr_t abr_insert(Abr_t root, const void *key, struct operations *op)
{
	if(root)
	{
		if(op->Cmp_function(key, root->key) < 0)
			root->left = abr_insert(root->left, key, op);
		else if(op->Cmp_function(key, root->key) > 0)
			root->right = abr_insert(root->right, key, op);

		return root;
	}
	else
		return create_node(key, op);
}

/* 	DESCRIZIONE
	Genera un albero binario di ricerca contenente n nodi casualmente generati.

	PARAMETRI DI INPUT
	int n: 					Numero di nodi da generare.
	struct operations *op: 	Struttura delle operazioni.
	
	VALORE DI RITORNO
	Restituisce un abr con n nodi. 							*/
Abr_t gen_abr(int n, struct operations *op)
{
	return gen_abr_f(NULL, n, op);
}

Abr_t gen_abr_f(Abr_t root, int n, struct operations *op)
{
	Abr_t node = NULL, ret;
	void *d = NULL;

	if(n != 0)
	{
		if((d = op->Rand_function()) == NULL)
			ret = NULL;
		
		if(root == NULL)
		{
			root = abr_insert(root, d, op);
			ret = gen_abr_f(root, n-1, op);
		}
		else
		{
			if(abr_search(root, d, op) != NULL)
				ret = gen_abr_f(root, n, op);
			else
			{
				root = abr_insert(root, d, op);
				ret = gen_abr_f(root, n-1, op);
			}
		}
				
		free(d);	
		return ret;
	}
	else
		return root;

}

/*	DESCRIZIONE
	Confronta due abr dati in input.

	VALORE DI RITORNO
	Restituisce 1 se i due abr hanno le stesse chiavi e la stessa forma, 0 altrimenti.*/
int abr_equal(Abr_t abr1, Abr_t abr2, struct operations *op)
{
	if(abr1 && abr2)
		return !op->Cmp_function(abr1->key, abr2->key) && abr_equal(abr1->left, abr2->left, op) &&
														  abr_equal(abr1->right, abr2->right, op);
	else if(abr1 == NULL && abr2 == NULL)
		return 1;
	else
		return 0;
}

/* DESCRIZIONE
   Crea un nuovo nodo che ha come chiave la stringa str data in input. La funzione inizializza
   opportunamente i puntatori left e right del nodo appena creato.
   
   PARAMETRI DI INPUT
   const void *str: 		Chiave del nodo da creare.
   struct operations *op: 	Struttura delle operazioni.

   VALORI DI RITORNO
   Nuovo nodo contenente come chiave la stringa str se quest'ultima è diversa da NULL, altrimenti,
   la funzione restituisce NULL.			*/
Abr_t create_node(const void *key, struct operations *op)
{
	Abr_t newnode = NULL;

	if(key == NULL)
		return NULL;

	newnode = (Abr_t) malloc(sizeof(struct abr_node));
	if(newnode == NULL)
		NULL;
	newnode->key = op->Dup_dato(key);
	newnode->left = NULL;
	newnode->right = NULL;

	return newnode;
}

/* 	DESCRIZIONE
	Duplica un abr dato in ingresso.

	PARAMETRI DI INPUT
	Abr_t a: 				Abr da duplicare;
	struct operations *op: 	Struttura delle operazioni.
	
	VALORE DI RITORNO
	Restituisce una copia esatta dell'albero binario a. 	*/
Abr_t dup_abr(Abr_t a, struct operations *op)
{
	Abr_t b = NULL;

	if(a)
	{
		if(b = create_node(a->key, op))
		{
			b->left = dup_abr(a->left, op);
			b->right = dup_abr(a->right, op);
		}
	}

	return b;
}

/* DESCRIZIONE
   Cancella il nodo contenente la chiave key dall'albero binario di ricerca.

   PARAMETRI DI INPUT
   Abr_t root:  			Radice dell'albero
   const void *key: 		Chiave contenuta nel nodo da eliminare.
   struct operations *op: 	Struttura delle operazioni.

   VALORI DI RITORNO
   Restituisce la radice del nuovo albero di ricerca. 					*/
Abr_t abr_del_node(Abr_t root, const void *key, struct operations *op)
{
	Abr_t node;

	if(root)
	{
		if(op->Cmp_function(key, root->key) < 0)
			root->left = abr_del_node(root->left, key, op);
		else if(op->Cmp_function(key, root->key) > 0)
			root->right = abr_del_node(root->right, key, op);
		else
		{
			node = root;
			if(node->right == NULL)
				root = node->left;
			else if(node->left == NULL)
				root = node->right;
			else
			{
				node = stacca_min(root->right, root);
				free(root->key);
				root->key = op->Dup_dato(node->key);
			}

			destroy_node(node, op);
		}
	}

	return root;
}

/* 	DESCRIZIONE
	Dato un albero binario di ricerca, elimina tutti i nodi compresi tra a e b (estremi inclusi) e che
	siano pari o dispari in base al parametro p.

	PARAMETRI DI INPUT
	Abr_t root: 			Radice abr.
	const char *a: 			Estremo inferiore.
	const char *b 			Estremi superiore.
	int p: 					Parità. Se p >= 1, si vogliono eliminare le stringhe pari, altrimenti quelle dispari.
	struct operations *op: 	Struttura delle operazioni.
	
	VALORE DI RITORNO
	Restituisce un albero binario di ricerca. 									*/
Abr_t bet(Abr_t root, const void *a, const void *b, const int p, struct operations *op)
{
	if(root)
	{
		if(op->Cmp_function(root->key, b) > 0)
			root->left = bet(root->left, a, b, p, op);
		else if(op->Cmp_function(root->key, a) < 0)
			root->right = bet(root->right, a, b, p, op);
		else
		{
			root->left = bet(root->left, a, b, p, op);
			root->right = bet(root->right, a, b, p, op);

			if(p > 0 && op->Is_even(root->key)) 				/* Pari */
				root = abr_del_node(root, root->key, op);
			else if(p == 0 && !op->Is_even(root->key)) 			/* Dispari */
				root = abr_del_node(root, root->key, op);
		}
	}

	return root;
}

/*	DESCRIZIONE
	Costruisce un albero binario di ricerca perfettamente bilanciato, partendo dall'array dato
	in ingresso.

	PARAMETRI DI INPUT
	void *v[]:				Array contenente le chiavi.
	int start: 				Indice del primo elemento dell'array (o sottoarray)
	int end:   				Indice dell'ultimo elemento dell'array (o sottoarray)
	struct operations *op: 	Struttura delle operazioni.

	VALORE DI RITORNO
	Restituisce un albero binario di ricerca perfettamente bilanciato contenente le chiavi corrispondenti
	all'array dato in ingresso.			*/
Abr_t build_APB_f(void *v[], int start, int end, struct operations *op)
{
	Abr_t nodo = NULL;
	
	
	if(start <= end)
	{
		int mid = start + (end - start) / 2;
		nodo = create_node(v[mid], op);

		nodo->left = build_APB_f(v, start, mid-1, op);
		nodo->right = build_APB_f(v, mid+1, end, op);
	}

	return nodo;
}

Abr_t build_APB(void *v[], int n, struct operations *op)
{
	return build_APB_f(v, 0, n-1, op);
}

/*	DESCRIZIONE
	Riempie un array con le chiavi dell'abr dato in input, in ordine crescente.

	PARAMETRI DI INPUT
	Abr_t root:  			Radice dell'alber da trattare
	char *a[]: 				Array dove verranno memorizzate le chiavi.
	int start: 				Indice del primo elemento dell'array (o sottoarray)
	int end:   				Indice dell'ultimo elemento dell'array (o sottoarray)	
	struct operations *op: 	Struttura delle operazioni. 			*/
int get_array_f(Abr_t root, void *v[], int start, int end, struct operations *op)
{
	if(root)
	{
		int i = get_array_f(root->left, v, start, end, op);
		op->Store_at(v, root->key, i);
		return get_array_f(root->right, v, i+1, end, op);
	}

	return start;
}

int get_array(Abr_t root, void *a[], int n, struct operations *op)
{
	return get_array_f(root, a, 0, n-1, op);
}

/*	DESCRIZIONE
	Conta il numero di nodi presenti in un albero binario di ricerca.

	PARAMETRI DI INPUT
	Abr_t root: Abr_t di cui si vuole conoscere il numero di nodi.

	VALORE DI RITORNO
	Restituisce il valore intero corrispondente al numero di nodi dell'albero dato
	in ingresso.				*/
int get_n_node(Abr_t root)
{
	if(root == NULL)
		return 0;
	else
		return 1 + get_n_node(root->left) + get_n_node(root->right);
}

/* DESCRIZIONE 
   Stacca il nodo minimo dall'albero binario di ricerca, modificando opportunamente
   il puntatore del nodo padre.

   PARAMETRI DI INPUT
   Abr_t root: Radice dell'albero o di un qualsiasi sottoalbero.
   Abr_t P:	 Nodo che punta al padre del nodo root.

   VALORI DI RITORNO
   Tale funzione restituisce il nodo minimo che è stato staccato dall'albero.	*/
Abr_t stacca_min(Abr_t root, Abr_t P)
{
	if(root)
	{
		if(root->left)
			return stacca_min(root->left, root);
		else
			if(root == P->left)
				P->left = root->right;
			else
				P->right = root->right;
	}

	return root;
}

/*	DESCRIZIONE
	Elimina tutti i nodi appartenenti all'albero dato in input.

	PARAMETRI DI INPUT
	Abr_t root: 			Radice dell'albero da eliminare.
	struct operations *op: 	Strutture delle operazioni.

	VALORE DI RITORNO
	Restituisce NULL in ogni caso. 					*/
Abr_t abr_destroy(Abr_t root, struct operations *op)
{
	if(root)
	{
		root->left = abr_destroy(root->left, op);
		root->right = abr_destroy(root->right, op);
		destroy_node(root, op);
	}

	return NULL;
}

/* 	DESCRIZIONE
	Dealloca il nodo dato in input.

 	PARAMETRI
	Abr_t node: 			Nodo da deallocare.
	struct operations *op: 	Struttura delle operazioni. */
void destroy_node(Abr_t node, struct operations *op)
{
	op->Destroy_key(node->key);
	free(node);
}

/* DESCRIZIONE
   Ricerca del minimo in un abr dato.

   PARAMETRI DI INPUT
   Abr_t root: Radice dell'albero dal quale iniziare la ricerca.

   VALORI DI RITORNO
   La funzione restituisce il nodo minimo se root è diverso da NULL. In caso contrario,
   restituisce NULL.		*/
Abr_t abr_min(Abr_t root)
{
	if(root && root->left)
		return abr_min(root->left);
	
	return root;
}

/* DESCRIZIONE
   Ricerca del massimo in un abr dato.

   PARAMETRI DI INPUT
   Abr_t root: Radice dell'albero dal quale iniziare la ricerca.

   VALORI DI RITORNO
   La funzione restituisce il nodo massimo se root è diverso da NULL. In caso contrario,
   restituisce NULL.		*/
Abr_t abr_max(Abr_t root)
{
	if(root && root->right)
		return abr_max(root->right);
	
	return root;
}

/* DESCRIZIONE
   La funzione stampa i nodi dell'albero dato in input, utilizzando la visita pre-order.

   PARAMETRI DI INPUT
   Abr_t root: 				Radice dell'arbero o di un qualsiasi sottoalbero.
   struct operations *op: 	Struttura delle operazioni 				*/
void preorder(Abr_t root, struct operations *op)
{
	if(root)
	{
		op->Print_key(root->key);
		preorder(root->left, op);
		preorder(root->right, op);
	}
}

/* DESCRIZIONE
   La funzione stampa i nodi dell'albero dato in input, utilizzando la visita post-order.

   PARAMETRI DI INPUT
   Abr_t root: radice dell'arbero o di un qualsiasi sottoalbero.
   struct operations *op: 	Struttura delle operazioni 					*/
void postorder(Abr_t root, struct operations *op)
{
	if(root)
	{
		postorder(root->left, op);
		postorder(root->right, op);
		op->Print_key(root->key);
	}
}

/* DESCRIZIONE
   La funzione stampa i nodi dell'albero dato in input, utilizzando la visita in-order.

   PARAMETRI DI INPUT
   Abr_t root: radice dell'arbero o di un qualsiasi sottoalbero.
   struct operations *op: 	Struttura delle operazioni 					 */
void inorder(Abr_t root, struct operations *op)
{
	if(root)
	{
		inorder(root->left, op);
		op->Print_key(root->key);
		inorder(root->right, op);
	}
}
