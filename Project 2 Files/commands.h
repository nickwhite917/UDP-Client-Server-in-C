#ifndef _COMMANDS_H_
#define _COMMANDS_H_

void mount(char *filename);
void unmount();
void structure();
void traverse(int option);
void showsector(int sectNum);
void showfat();
int checkMount();

#endif
