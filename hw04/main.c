/**
 * @file    main.c
 * \mainpage HW04 Documentation
 *
 *@autor Alisia Lyzhina
 * On the 'Files' page, there is a list of documented files with brief descriptions.
 *
*/

#include "graph.h"
#include "heap.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


#define EXIT_OK 0
#define EXIT_FAILURE 1

int create_nodes(FILE *nodes, Graph *graph);

int create_edges(FILE *edges, Graph *graph);

Node * dijkstras_algorithm(unsigned int from_node, unsigned int in_node, Graph *graph);

void print_graph(Node *node);

int print_graph_to_file(Node *node, FILE *file);

int main(int arg,char *argv[])
{
    if(arg < 5 || arg > 6) {
        fprintf(stderr, "Incorrect number of arguments in %s\n", argv[0]);
        return EXIT_FAILURE;
    } 
    FILE *nodes = fopen(argv[1], "r");
    if (nodes == NULL) {
        fprintf(stderr, "Incorrect file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    Graph *graph = graph_new();
    if (create_nodes(nodes, graph) != 0) {
        fprintf(stderr, "Incorrect file with information about nodes in %s\n", argv[0]);
        graph_free(graph);
        fclose(nodes);
        return EXIT_FAILURE;
    }
    FILE *edges = fopen(argv[2], "r");
    if (edges == NULL) {
        fprintf(stderr, "Incorrect file %s\n", argv[2]);
        graph_free(graph);
        fclose(nodes);
        return EXIT_FAILURE;
    }
    if (create_edges(edges, graph) != 0) {
        fprintf(stderr, "Incorrect file with information about edges in %s\n", argv[0]);
        graph_free(graph);
        fclose(edges);
        fclose(nodes);
        return EXIT_FAILURE;
    }
    unsigned int from_node = strtol(argv[3], NULL, 10);
    unsigned int in_node = strtol(argv[4], NULL, 10);
    if (graph_get_node(graph, from_node) == NULL || graph_get_node(graph, in_node) == NULL) {
        fprintf(stderr, "Node does not exist\n");
        graph_free(graph);
        fclose(edges);
        fclose(nodes);
        return EXIT_FAILURE;
    }
    Node *end_node = dijkstras_algorithm(from_node,in_node, graph);
    if (end_node == NULL) {
        fprintf(stderr, "It is impossible find path from %d to %d\n", from_node, in_node);
        fclose(edges);
        fclose(nodes);
        graph_free(graph);
        return EXIT_FAILURE;
    }
    if (arg == 5) {
        print_graph(end_node);
    } else {
        FILE *file = fopen(argv[5], "w");
        if (file == NULL) {
            fprintf(stderr, "Incorrect output file\n");
            fclose(nodes);
            fclose(edges);
            graph_free(graph);
            return EXIT_FAILURE;
        }
        if (print_graph_to_file(end_node, file) != 0) {
            fprintf(stderr, "It is impossible print to file\n");
            fclose(edges);
            fclose(nodes);
            fclose(file);
            graph_free(graph);
            return EXIT_FAILURE;
        }
        fclose(file);
    }
    fclose(edges);
    fclose(nodes);
    graph_free(graph);
    return EXIT_OK;
}

int create_nodes(FILE *nodes, Graph *graph)
{
    assert(nodes != NULL);
    assert(graph != NULL);
    unsigned int node_id = 0;
    int result = 0;
    while (true) {
        if ((result = fscanf(nodes, "%u", &node_id)) == 0) {
            return EXIT_FAILURE;
        }
        if (result == -1) {
            break;
        }
        if (!graph_insert_node(graph, node_id)) {
            return EXIT_FAILURE;
        }
        char tmp;
        do {
            tmp = fgetc(nodes);
        } while (strchr(&tmp, '\n') == NULL);
    }
    return EXIT_OK;
}

int create_edges(FILE *edges, Graph *graph)
{
    assert(edges != NULL);
    assert(graph != NULL);
    unsigned int from_node_id = 0;
    unsigned int in_node_id = 0;
    unsigned int mindelay = 0;
    int result;
    while (true) {
        if ((result = fscanf(edges, "%u", &from_node_id)) == 0) {
            return EXIT_FAILURE;
        }
        if (result == -1) {
            break;
        }
        char tmp = fgetc(edges);
        if ((result = fscanf(edges, "%u", &in_node_id)) == 0) {
            return EXIT_FAILURE;
        }
        tmp = fgetc(edges);
        do {
            tmp = fgetc(edges);
        } while (strchr(&tmp, ',') == NULL);
        if ((result = fscanf(edges, "%u", &mindelay))!= 1) {
            return EXIT_FAILURE;
        }
        if (!graph_insert_edge(graph, from_node_id, in_node_id, mindelay)){
            return EXIT_FAILURE;
        }
        do {
            tmp = fgetc(edges);
        } while (strchr(&tmp, '\n') == NULL);
    }
    return EXIT_OK;
}

Node * dijkstras_algorithm(unsigned int from_node, unsigned int in_node, Graph *graph)
{
    Node *node = graph_get_node(graph, from_node);
    Node *end_node = graph_get_node(graph, in_node);
    struct edge *edges = node_get_edges(node);
    Heap *heap = heap_new_from_graph(graph);
    heap_decrease_distance(heap, node, 0, NULL);
    heap_extract_min(heap);
    unsigned int dist = 0;
    while (dist != UINT_MAX && node_get_id(node) != in_node && !heap_is_empty(heap)) {
        for (size_t i = 0; i < node_get_n_outgoing(node); i++) {
            if (dist != UINT_MAX && edges[i].mindelay + dist < node_get_distance(edges[i].destination)) {
                heap_decrease_distance(heap, edges[i].destination, dist + edges[i].mindelay, node);
            }
        }
        node = heap_extract_min(heap);
        dist = node_get_distance(node);
        edges = node_get_edges(node);
    }
    if (node_get_id(node) != in_node || node_get_distance(node) == UINT_MAX) {
        heap_free(heap);
        return NULL;
    }
    heap_free(heap);
    return end_node;
}


void print_graph(Node *node)
{
    assert(node != NULL);
    printf("digraph {\n");
    while (node_get_distance(node) != 0) {
        Node *prev = node_get_previous(node);
        unsigned int prev_id = node_get_id(prev);
        unsigned int node_id = node_get_id(node);
        size_t dist =node_get_distance(node) - node_get_distance(prev);
        printf("\t%d -> %d [label=%zu];\n", prev_id, node_id, dist);
        node = prev;
    }
    printf("}\n");
}

int print_graph_to_file(Node *node, FILE *file)
{
    assert(node != NULL);
    assert(file != NULL);
    fprintf(file, "digraph {\n");
    while (node_get_distance(node) != 0) {
        Node *prev = node_get_previous(node);
        unsigned int prev_id = node_get_id(prev);
        unsigned int node_id = node_get_id(node);
        size_t dist =node_get_distance(node) - node_get_distance(prev);
        if (fprintf(file, "\t%d -> %d [label=%zu];\n", prev_id, node_id, dist) < 0) {
            return EXIT_FAILURE;
        }
        node = prev;

    }
    fprintf(file, "}\n");
    return EXIT_OK;
}
