#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
	FILE *fp = fopen("bunnyN.ply","r");

	char* line = NULL;
	size_t len =  0;
	char* word = NULL;
	int vertex = 0;
	int face = 0;	
	do {
		getline(&line, &len, fp);
		word = strtok(line, " ");
		if (!strcmp(word, "element")) {
			word = strtok(NULL, " ");
			if (!strcmp(word, "vertex")) {
				word = strtok(NULL, " ");
				vertex = atoi(word);
			} else if (!strcmp(word, "face")) {
				word = strtok(NULL, " ");
				face = atoi(word);
			}
		}
	} while (strncmp(line, "end_header", 10) != 0);

	int i;
	float vertex_array[vertex*6];
	for (i = 0; i < vertex; i++) {
		getline(&line, &len, fp);

		word = strtok(line, " ");
		vertex_array[i*6 + 0] = atof(word);
		word = strtok(NULL, " ");
		vertex_array[i*6 + 1] = atof(word);
		word = strtok(NULL, " ");
		vertex_array[i*6 + 2] = atof(word);
		word = strtok(NULL, " ");
		vertex_array[i*6 + 3] = atof(word);
		word = strtok(NULL, " ");
		vertex_array[i*6 + 4] = atof(word);
		word = strtok(NULL, " ");
		vertex_array[i*6 + 5] = atof(word);
	}

	int face_array[face*3];
	for (i = 0; i < face; i++) {
		getline(&line, &len, fp);
		
		word = strtok(line, " ");
		word = strtok(NULL, " ");
		face_array[i*3 + 0] = atoi(word);
		word = strtok(NULL, " ");
		face_array[i*3 + 1] = atoi(word);
		word = strtok(NULL, " ");
		face_array[i*3 + 2] = atoi(word);
	}
}