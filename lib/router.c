#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* path;
    char* file;
} Route;

#define MAX_ROUTES 1000
Route routes[MAX_ROUTES];
int route_count = 0;

void init_routes() {
    routes[route_count++] = (Route) {"/", "src/html/index.html"};
    routes[route_count++] = (Route) {"/secondPage", "src/html/secondPage.html"};
    routes[route_count++] = (Route) {"/styles/index.css", "src/styles/index.css"};
}

char* route_request(const char* path) {
    for (int i = 0; i < route_count; ++i) {
        if (strcmp(path, routes[i].path) == 0) {
            return routes[i].file;
        }
    }
    return "src/html/unknownPath.html";  // return route to page not found page if route is not defined
}