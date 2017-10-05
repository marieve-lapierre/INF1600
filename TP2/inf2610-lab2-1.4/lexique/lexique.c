/*
 * lexique.c
 *
 *  Created on: Aug 26, 2013
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "token.h"
#include "frequency.h"

#define PROGNAME "lexique"
#define VAL_VERSION "1.0"
static const char *const progname = PROGNAME;
volatile int started = 0;

struct opts {
    int verbose;
};

/*
 * Variables globales
 * children: contient les PID des enfants
 * fd_word_len: tube pour la transmission de la taille des mots
 * fd_word_str: tube pour la transmission des mots
 */
int children[2];
int fd_word_len[2];
int fd_word_str[2];

/*
 * Indexes pour acces aux descripteurs d'un tube dans un tableau int[2]
 * READ  0: descripteur en lecture
 * WRITE 1: descripteur en ecriture
 *
 * Descripteurs standards dans unistd.h:
 * STDIN_FILENO  0
 * STDOUT_FILENO 1
 * STDERR_FILENO 2
 */
#define READ 0
#define WRITE 1

__attribute__((noreturn))
static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "Analyse lexicale d'un texte\n");
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "--verbose        affiche plus de message\n");
    fprintf(stderr, "--help           ce message d'aide\n");
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct opts *opts) {
    int opt;

    struct option options[] = {
            { "help",       0, 0, 'h' },
            { "verbose",    0, 0, 'v' },
            { 0, 0, 0, 0}
    };
    int idx;

    opts->verbose = 0;
    while ((opt = getopt_long(argc, argv, "hv", options, &idx)) != -1) {
        switch (opt) {
        case 'v':
            opts->verbose = 1;
            break;
        case 'h':
            usage();
            break;
        default:
            usage();
            break;
        }
    }

}

/*
 * Gestionnaire de signal SIGINT
 * Prototype: void handler (int signum)
 */
void handle_quit(int signum) {
    printf("SIGINT\n");
    if (started) {
        // TODO: Envoyer un long == -1 sur fd_word_len pour que task_frequency()
        //       imprime les resultats
    }
    exit(0);
}

int main(int argc, char **argv) {
    int ret;
    struct opts *opts = calloc(1, sizeof(struct opts));
    parse_opts(argc, argv, opts);

    // TODO: Creation des tubes fd_word_len et fd_word_str avec pipe()

    /*
     * Demarrage de task_tokenize()
     */
    children[0] = fork();
    switch(children[0]) {
    case -1:
        fprintf(stderr, "fork failed\n");
        exit(0);
        break;
    case 0:
        // TODO: Fermer les descripteurs de lecture
        // TODO: Appel a task_tokenize(int stdin,
        //                             int output_word_len,
        //                             int output_word_str);
        // TODO: Fermer les descripteurs d'ecriture

        fprintf(stderr, "task_tokenize done\n");
        exit(0);
        break;
    default:
        // TODO: Fermer les descripteurs qui ne sont plus requis dans le parent
        break;
    }

    /*
     * Demarrage de task_frequency()
     */
    children[1] = fork();
    switch(children[1]) {
    case -1:
        fprintf(stderr, "fork failed\n");
        break;
    case 0:
        // Desactivation de SIGINT
        signal(SIGINT, SIG_IGN);
        // TODO: Fermer les descripteurs d'ecriture
        // TODO: Appel a task_frequency(int input_word_len,
        //                             int input_word_str,
        //                             int stdout)
        // TODO: Fermer les descripteurs de lecture

        fprintf(stderr, "task_frequency done\n");
        exit(0);
        break;
    default:
        // TODO: Fermer les descripteurs qui ne sont plus requis dans le parent
        break;
    }

    started = 1;

    // Interception du signal SIGINT
    signal(SIGINT, handle_quit);

    // Attendre la fin du traitement de task_token()
    waitpid(children[0], NULL, 0);

    // Attendre la fin du traitement de task_frequency()
    waitpid(children[1], NULL, 0);

    // Fermeture du descripteur fd_word_len[WRITE]
    close(fd_word_len[WRITE]);

    printf("done %d %d\n", children[0], children[1]);

out:
    // Terminaison
    free(opts);
    return ret;
}
