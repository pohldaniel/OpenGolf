#ifndef _GOLF_JSON_H 
#define _GOLF_JSON_H 

#include <parson.h>
#include "level.h"
#include "maths.h"

void golf_json_init(void);

void golf_json_object_get_data(JSON_Object *obj, const char *name, unsigned char **data, int *data_len);
void golf_json_object_set_data(JSON_Object *obj, const char *name, unsigned char *data, int data_len);

void golf_json_array_get_data(JSON_Array *arr, int idx, unsigned char **data, int *data_len);
void golf_json_array_append_data(JSON_Array *arr, unsigned char *data, int data_len);

vec2 golf_json_object_get_vec2(JSON_Object *obj, const char *name);
void golf_json_object_set_vec2(JSON_Object *obj, const char *name, vec2 v);

vec3 golf_json_object_get_vec3(JSON_Object *obj, const char *name);
void golf_json_object_set_vec3(JSON_Object *obj, const char *name, vec3 v);

vec4 golf_json_object_get_vec4(JSON_Object *obj, const char *name);
void golf_json_object_set_vec4(JSON_Object *obj, const char *name, vec4 v);

quat golf_json_object_get_quat(JSON_Object *obj, const char *name);
void golf_json_object_set_quat(JSON_Object *obj, const char *name, quat q);

void golf_json_object_get_float_array(JSON_Object *obj, const char *name, float **v, int *n);
void golf_json_object_set_float_array(JSON_Object *obj, const char *name, float *arr, int n, float min, float max);

#endif 
