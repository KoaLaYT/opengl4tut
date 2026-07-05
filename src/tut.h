#ifndef INCLUDE_TUT_H
#define INCLUDE_TUT_H

#include <glad/gl.h>

const char* tut_read_entire_file(const char* path);

void tut_restart_log();
void tut_log_debug(const char* msg, ...);
void tut_log_info(const char* msg, ...);
void tut_log_err(const char* msg, ...);
void tut_log_glparams();

GLuint tut_compile_program(const char* vs_path, const char* fs_path);

#endif // INCLUDE_TUT_H
