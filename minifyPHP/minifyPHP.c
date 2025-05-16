#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>


/**
*
*  - Macros
*
*/


//  Affiche le message d'erreur et quitte
#define ERR(msg) do { perror(msg); return 1; } while(0)

// Ajouter un caractère au buffer minifié
#define ADD(c)  minified[i_min++] = (c)



/**
*
*  STATES
*
*/


// États possibles pour la machine à états
typedef enum {
    NORMAL,    // état par défaut, lecture de code normal
    IN_DQUOTE, // à l'intérieur d'une chaîne double-quote
    IN_SQUOTE, // à l'intérieur d'une chaîne single-quote
    IN_LINE_C, // à l'intérieur d'un commentaire //
    IN_BLOCK_C,// à l'intérieur d'un commentaire /* */
    IN_HTML_C  // (non utilisé ici) réservé si besoin
} State;



/**
*
*  MINIFY
*  - Parcourt le buffer `in` de taille `size`.
*  - Supprime les commentaires et réduit les espaces multiples.
*  - Conserve les chaînes et le code valide.
*  - Retourne un buffer alloué (taille max `size`) et écrit la taille réelle dans `out_size`.
*/


char *minify(const char *in, int size, int *out_size) {
    // Alloue le buffer de sortie
    char *minified = malloc(size);
    if (!minified) return NULL;  // échec d'allocation

    int i_min = 0;      // index courant dans minified
    State state = NORMAL; // état initial

    // Construction d'une table de lookup pour les tokens (caractères spéciaux)
    bool is_token[256] = {0};
    const char tokens[] = "{}();*+/&^+-=%<>!?.,:| \n\r\t";
    for (size_t t = 0; t < sizeof(tokens) - 1; ++t) {
        is_token[(unsigned char)tokens[t]] = true;
    }

    // Parcours du buffer d'entrée
    for (int i = 0; i < size; ++i) {
        char c = in[i];
        switch (state) {
            case NORMAL:
                // Détection de début de chaîne double-quote
                if (c == '"') {
                    state = IN_DQUOTE;
                    ADD(c);
                }
                // Détection de début de chaîne single-quote
                else if (c == '\'') {
                    state = IN_SQUOTE;
                    ADD(c);
                }
                // Détection de commentaire //
                else if (c == '/' && i + 1 < size && in[i+1] == '/') {
                    state = IN_LINE_C;
                    ++i; // saute le second '/'
                }
                // Détection de commentaire /* */
                else if (c == '/' && i + 1 < size && in[i+1] == '*') {
                    state = IN_BLOCK_C;
                    i += 1; // saute le '*'
                }
                // Gestion des espaces et retours à la ligne
                else if (isspace((unsigned char)c)) {
                    // On écrit un seul espace consécutif
                    if (i_min > 0 && minified[i_min-1] != ' ') {
                        ADD(' ');
                    }
                }
                // Caractère normal à conserver
                else {
                    ADD(c);
                }
                break;

            case IN_DQUOTE:
                // À l'intérieur d'une chaîne, on conserve tout
                ADD(c);
                // Fin de chaîne si " non échappé
                if (c == '"' && in[i-1] != '\\') {
                    state = NORMAL;
                }
                break;

            case IN_SQUOTE:
                // À l'intérieur d'une chaîne, on conserve tout
                ADD(c);
                // Fin de chaîne si ' non échappé
                if (c == '\'' && in[i-1] != '\\') {
                    state = NORMAL;
                }
                break;

            case IN_LINE_C:
                // Ignorer jusqu'à la fin de ligne
                if (c == '\n') {
                    state = NORMAL;
                    ADD(' '); // remplace le commentaire par un espace
                }
                break;

            case IN_BLOCK_C:
                // Ignorer jusqu'à la séquence */
                if (c == '*' && i + 1 < size && in[i+1] == '/') {
                    state = NORMAL;
                    i += 1; // saute le '/'
                }
                break;

            default:
                // autres états (IN_HTML_C non implémenté ici)
                break;
        }
    }

    // On retourne la taille réelle et le buffer
    *out_size = i_min;
    return minified;
}
