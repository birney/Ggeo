
#ifndef GGEO_RENDER_HEADER
#define GGEO_RENDER_HEADER

#include "ggeo_feature.h"

#define GGEO_RENDER_PARAMETER 16


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
  int pixel_tile_width; /* deliberately int to ensure snap-ability etc */
  double rgbcolor[3];
  double alpha;
  Ggeo_Render_Parameter para[GGEO_MAX_PARAMETER];
  int len;
} Ggeo_Render_User_Parameter_Set;


typedef struct ggeo_track_renderer {
  void * data;
  void (*free_data)(struct ggeo_track_render *);
  Ggeo_Render_Object * (*render_track)(Ggeo_Feature_LinkedList *,long int start,long int end);
} Ggeo_Track_Renderer;


typedef struct ggeo_track_renderer_factory {
  void * data;
  void (*free_data)(struct ggeo_track_render *);
  Ggeo_Render_User_Parameter_Set * (*get_default_parameters)(char * response_type);
  Ggeo_Track_Renderer * (*make_renderer)(char * response_type,Ggeo_Render_User_Parameter_Set * ps);
} Ggeo_Track_Renderer_Factory;


#endif

