
#ifndef GGEO_RENDER_HEADER
#define GGEO_RENDER_HEADER

#include "ggeo_feature.h"

#define GGEO_MAX_PARAMETER 24


typedef struct ggeo_render_object {
  void * data;
  void (*free_data)(struct ggeo_render_object *);
  long int start;
  long int end;
} Ggeo_Render_Object;


typedef struct ggeo_render_user_parameter {
  char * parameter_name;
  int parameter_type;
  int int_val;
  double double_val;
} Ggeo_Render_User_Parameter;



typedef struct ggeo_user_parameter_set {
  Ggeo_Render_User_Parameter para[GGEO_MAX_PARAMETER];
  int len;
  double red;
  double green;
  double blue;
  double alpha_t;
  int tile_pixel_width;
} Ggeo_Render_User_Parameter_Set;


typedef struct ggeo_track_renderer {
  void * data;
  void (*free_data)(struct ggeo_track_renderer *);
  Ggeo_Render_Object * (*render_track)(void * data,Ggeo_Feature_LinkedList *,long int start,long int end);
} Ggeo_Track_Renderer;


typedef struct ggeo_track_renderer_factory {
  void * data;
  void (*free_data)(struct ggeo_track_renderer_factory *);
  Ggeo_Render_User_Parameter_Set * (*get_default_parameters)(void * data,char * response_type);
  Ggeo_Track_Renderer * (*make_renderer)(void * data,Ggeo_Render_User_Parameter_Set * gp,char * response_type);
} Ggeo_Track_Renderer_Factory;

#endif

