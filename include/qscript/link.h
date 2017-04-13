/* link.h
 * ------
 * convenient linker functions. */

#ifndef __QS_LINK_H
#define __QS_LINK_H

#include "ids.h"

#define QS_LINK_FRONT_GLOBAL(obj, type) \
   do { \
      qs_id_assign (&qs_##type##_ids, obj, &(obj->id)); \
      obj->prev = NULL; \
      obj->next = qs_##type##_list_front; \
      qs_##type##_list_front = obj; \
      if (obj->next) \
         obj->next->prev = obj; \
      else \
         qs_##type##_list_back = obj; \
      qs_##type##_count++; \
   } while (0)

#define QS_LINK_BACK_GLOBAL(obj, type) \
   do { \
      qs_id_assign (&qs_##type##_ids, obj, &(obj->id)); \
      obj->prev = qs_##type##_list_back; \
      obj->next = NULL; \
      qs_##type##_list_back = obj; \
      if (obj->prev) \
         obj->prev->next = obj; \
      else \
         qs_##type##_list_front = obj; \
      qs_##type##_count++; \
   } while (0)

#define QS_UNLINK_GLOBAL(obj, type) \
   do { \
      qs_id_remove (&qs_##type##_ids, obj, &(obj->id)); \
      if (obj->prev) obj->prev->next = obj->next; \
      else qs_##type##_list_front    = obj->next; \
      if (obj->next) obj->next->prev = obj->prev; \
      else qs_##type##_list_back     = obj->prev; \
      qs_##type##_count--; \
   } while (0)

#define QS_LINK_FRONT(list, obj, type) \
   do { \
      qs_id_assign (&((list)->type##_ids), obj, &(obj->id)); \
      obj->prev = NULL; \
      obj->next = (list)->type##_list_front; \
      (list)->type##_list_front = obj; \
      if (obj->next) \
         obj->next->prev = obj; \
      else \
         (list)->type##_list_back = obj; \
      (list)->type##_count++; \
   } while (0)

#define QS_LINK_BACK(list, obj, type) \
   do { \
      qs_id_assign (&((list)->type##_ids), obj, &(obj->id)); \
      obj->prev = (list)->type##_list_back; \
      obj->next = NULL; \
      (list)->type##_list_back = obj; \
      if (obj->prev) \
         obj->prev->next = obj; \
      else \
         (list)->type##_list_front = obj; \
      (list)->type##_count++; \
   } while (0)

#define QS_UNLINK(list, obj, type) \
   do { \
      qs_id_remove (&((list)->type##_ids), obj, &(obj->id)); \
      if (obj->prev) obj->prev->next = obj->next; \
      else (list)->type##_list_front = obj->next; \
      if (obj->next) obj->next->prev = obj->prev; \
      else (list)->type##_list_back  = obj->prev; \
      (list)->type##_count--; \
   } while (0)

#define QS_LINK_FRONT_EXTRA(list, obj, type, extra_type) \
   do { \
      obj->extra_type##_prev = NULL; \
      obj->extra_type##_next = (list)->type##_list_front; \
      (list)->type##_list_front = obj; \
      if (obj->extra_type##_next) \
         obj->extra_type##_next->extra_type##_prev = obj; \
      else \
         (list)->type##_list_back = obj; \
      (list)->type##_count++; \
   } while (0)

#define QS_LINK_BACK_EXTRA(list, obj, type, extra_type) \
   do { \
      obj->extra_type##_prev = (list)->type##_list_back; \
      obj->extra_type##_next = NULL; \
      (list)->type##_list_back = obj; \
      if (obj->extra_type##_prev) \
         obj->extra_type##_prev->extra_type##_next = obj; \
      else \
         (list)->type##_list_front = obj; \
      (list)->type##_count++; \
   } while (0)

#define QS_UNLINK_EXTRA(list, obj, type, extra_type) \
   do { \
      if (obj->extra_type##_prev) \
         obj->extra_type##_prev->extra_type##_next = obj->extra_type##_next; \
      else \
         (list)->type##_list_front = obj->extra_type##_next; \
      if (obj->extra_type##_next) \
         obj->extra_type##_next->extra_type##_prev = obj->extra_type##_prev; \
      else (list)->type##_list_back \
         = obj->extra_type##_prev; \
      (list)->type##_count--; \
   } while (0)

#endif
