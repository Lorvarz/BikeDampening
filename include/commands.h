#ifndef __COMMANDS_H__
#define __COMMANDS_H__

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

void command_shell(void);
int input_IMU(const char * data, const char * path, int nl);
void test_cat(const char * path);

#endif /* __COMMANDS_H_ */
