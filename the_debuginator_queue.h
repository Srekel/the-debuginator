
/*
the_debuginator_queue.h - v0.01 - public domain - Anders Elfgren @srekel, 2018

# THE DEBUGINATOR QUEUE

A utility library to enable simple multi-threading support for The Debuginator.

See github for latest version: https://github.com/Srekel/the-debuginator

## Usage

In *ONE* source file, put:

```C
#define DEBUGINATOR_QUEUE_IMPLEMENTATION

// Define any of these if you wish to override them.
// (There are more. Find them in the beginning of the code.)
#define DEBUGINATOR_assert
#define DEBUGINATOR_memcpy
#define DEBUGINATOR_fabs

#include "the_debuginator.h"
#include "the_debuginator_queue.h"
```

Depending on if its in a binary that has actual access to the debuginator,
also #define DEBUGINATOR_QUEUE_CAN_PROCEES

Other source files should just include the_debuginator_queue.h

## Notes

See the documentation on the github page.

## License

Basically Public Domain / MIT.
See end of file for license information.

*/

#ifndef INCLUDE_THE_DEBUGINATOR_QUEUE_H
#define INCLUDE_THE_DEBUGINATOR_QUEUE_H

#include "the_debuginator.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUGINATOR_ENABLE_WARNINGS
#ifdef _MSC_VER
#pragma warning( push, 0 )
// #pragma warning( disable: 4820 4201)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-value"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif
#endif

#define DEBUGINATOR_QUEUE_MAX_PATH_LENGTH 128
#define DEBUGINATOR_QUEUE_MAX_DESCRIPTION_LENGTH 512

typedef enum DebuginatorQueueItemTypes {
  DEBUGINATOR_QUEUE_CreateItem,
  DEBUGINATOR_QUEUE_NumItemTypes
} DebuginatorQueueItemTypes;

typedef struct {
  char                             path[DEBUGINATOR_QUEUE_MAX_PATH_LENGTH];
  char                             description[DEBUGINATOR_QUEUE_MAX_DESCRIPTION_LENGTH];
  void*                            userdata;
  DebuginatorOnItemChangedCallback callback;
} DebuginatorQueue_CreateItemData;

typedef struct DebuginatorQueueItem {
  DebuginatorQueueItemTypes type;
  union {
    DebuginatorQueue_CreateItemData create_item;
  } data;
} DebuginatorQueueItem;

typedef struct TheDebuginatorQueue TheDebuginatorQueue;

typedef void* ( *DebuginatorQueueAllocateFunc )( void* userdata, int bytes );
typedef void ( *DebuginatorQueueDellocateFunc )( void* userdata, void* ptr );
typedef void ( *DebuginatorQueueItemCleanupFunc )( TheDebuginatorQueue*  queue,
                                                   DebuginatorQueueItem* item );

typedef struct TheDebuginatorQueue {
  void*                           userdata;
  DebuginatorQueueItem*           items;
  int                             num_items;
  int                             capacity;
  DebuginatorQueueAllocateFunc    allocate;
  DebuginatorQueueDellocateFunc   deallocate;
  DebuginatorQueueItemCleanupFunc cleanup_funcs[DEBUGINATOR_QUEUE_NumItemTypes];
} TheDebuginatorQueue;

TheDebuginatorQueue* debuginator_queue_create( int                           initial_size,
                                               DebuginatorQueueAllocateFunc  allocate_func,
                                               DebuginatorQueueDellocateFunc deallocate_func,
                                               void*                         userdata );

unsigned char* debuginator_queue_data( TheDebuginatorQueue* queue, int* out_size );
void           debuginator_queue_clear( TheDebuginatorQueue* queue );
void debuginator_queue_process( const unsigned char* data, int size, TheDebuginator* debuginator );
void debuginator_queue_create_bool_item( TheDebuginatorQueue* queue,
                                         const char*          path,
                                         const char*          description,
                                         void*                userdata );
void debuginator_queue_create_bool_item_with_callback( TheDebuginatorQueue*             queue,
                                                       const char*                      path,
                                                       const char*                      description,
                                                       void*                            userdata,
                                                       DebuginatorOnItemChangedCallback callback );

#ifdef __cplusplus
}
#endif

#ifdef DEBUGINATOR_QUEUE_IMPLEMENTATION

#ifndef DEBUGINATOR_QUEUE_assert
#include <assert.h>
#define DEBUGINATOR_QUEUE_assert assert;
#endif

#ifndef DEBUGINATOR_QUEUE_memcpy
#include <string.h>
#define DEBUGINATOR_QUEUE_memcpy memcpy
#endif

#ifndef DEBUGINATOR_QUEUE_strcpy_s
#include <string.h>
#define DEBUGINATOR_QUEUE_strcpy_s strcpy_s
#endif

static void
debuginator_queue__ensure_capacity( TheDebuginatorQueue* queue ) {
  if ( queue->capacity + 1 == queue->num_items ) {
    int   bytes = sizeof( DebuginatorQueueItem ) * queue->num_items * 2;
    void* items = queue->allocate( queue->userdata, bytes );
    DEBUGINATOR_QUEUE_memcpy( items, queue->items, bytes );
    queue->deallocate( queue->userdata, queue->items );
    queue->items    = (DebuginatorQueueItem*)items;
    queue->capacity = queue->num_items * 2;
  }
}

// static const char*
// debuginator_queue__copy_string( TheDebuginatorQueue* queue, const char* str ) {
//   int   bytes  = strlen( str );
//   void* buffer = queue->allocate( queue->userdata, bytes );
//   DEBUGINATOR_QUEUE_memcpy( buffer, str, bytes );
//   return (const char*)buffer;
// }

void
debuginator_queue_clear( TheDebuginatorQueue* queue ) {
  // for ( int i_item = 0; i_item < queue->num_items; ++i_item ) {
  //   DebuginatorQueueItem* item = &queue->items[i_item];
  //   queue->cleanup_funcs[item->type]( queue, item );
  // }
  queue->num_items = 0;
}

unsigned char*
debuginator_queue_data( TheDebuginatorQueue* queue, int* out_size ) {
  *out_size = queue->num_items * sizeof( DebuginatorQueueItem );
  return (unsigned char*)queue->items;
}

void
debuginator_queue_create_bool_item( TheDebuginatorQueue* queue,
                                    const char*          path,
                                    const char*          description,
                                    void*                userdata ) {

  debuginator_queue__ensure_capacity( queue );

  DebuginatorQueue_CreateItemData data;
  DEBUGINATOR_QUEUE_strcpy_s( data.path, DEBUGINATOR_QUEUE_MAX_PATH_LENGTH, path );
  DEBUGINATOR_QUEUE_strcpy_s(
    data.description, DEBUGINATOR_QUEUE_MAX_DESCRIPTION_LENGTH, description );
  data.userdata = userdata;

  DebuginatorQueueItem* item = &queue->items[queue->num_items++];
  item->type                 = DEBUGINATOR_QUEUE_CreateItem;
  item->data.create_item     = data;
}

void
debuginator_queue_create_bool_item_with_callback( TheDebuginatorQueue*             queue,
                                                  const char*                      path,
                                                  const char*                      description,
                                                  void*                            userdata,
                                                  DebuginatorOnItemChangedCallback callback ) {

  debuginator_queue__ensure_capacity( queue );

  DebuginatorQueue_CreateItemData data;
  DEBUGINATOR_QUEUE_strcpy_s( data.path, DEBUGINATOR_QUEUE_MAX_PATH_LENGTH, path );
  DEBUGINATOR_QUEUE_strcpy_s(
    data.description, DEBUGINATOR_QUEUE_MAX_DESCRIPTION_LENGTH, description );
  data.userdata = userdata;
  data.callback = callback;

  DebuginatorQueueItem* item = &queue->items[queue->num_items++];
  item->type                 = DEBUGINATOR_QUEUE_CreateItem;
  item->data.create_item     = data;
}

// void
// debuginator_queue__create_bool_item_cleanup( TheDebuginatorQueue*  queue,
//                                              DebuginatorQueueItem* item ) {

//   DebuginatorQueue_CreateItemData* data = (DebuginatorQueue_CreateItemData*)item->data;
//   queue->deallocate( queue->userdata, (void*)data->path );
//   queue->deallocate( queue->userdata, (void*)data->description );
//   queue->deallocate( queue->userdata, (void*)item->data );
// }

void
debuginator_queue_process( const unsigned char* data, int size, TheDebuginator* debuginator ) {
  (void)( data, size, debuginator );
#ifdef DEBUGINATOR_QUEUE_CAN_PROCEES
  const void*                 data_end = data + size;
  const DebuginatorQueueItem* item     = (DebuginatorQueueItem*)(unsigned long long)data;
  while ( item < data_end ) {
    switch ( item->type ) {
    case DEBUGINATOR_QUEUE_CreateItem: {
      //   debuginator_create_array_item(debuginator, NULL, item->data.create_item.path,
      // description, item->data.create_item.callback,  item->data.create_item.userdata,
      // debuginator->bool_titles, debuginator->bool_values, 2,
      // sizeof(debuginator->bool_values[0]));
      if ( item->data.create_item.callback == NULL ) {
        debuginator_create_bool_item( debuginator,
                                      item->data.create_item.path,
                                      item->data.create_item.description,
                                      item->data.create_item.userdata );
      }
      else {
        debuginator_create_bool_item_with_callback( debuginator,
                                                    item->data.create_item.path,
                                                    item->data.create_item.description,
                                                    item->data.create_item.userdata,
                                                    item->data.create_item.callback );
      }
    } break;
    default:
      break;
    }

    item++;
  }

  DEBUGINATOR_QUEUE_assert( item == data_end );
#endif
}

TheDebuginatorQueue*
debuginator_queue_create( int                           initial_size,
                          DebuginatorQueueAllocateFunc  allocate_func,
                          DebuginatorQueueDellocateFunc deallocate_func,
                          void*                         userdata ) {

  int                  buffer_bytes = sizeof( TheDebuginatorQueue );
  int                  item_bytes   = sizeof( DebuginatorQueueItem ) * initial_size;
  void*                queue_buffer = allocate_func( userdata, buffer_bytes );
  void*                item_buffer  = allocate_func( userdata, item_bytes );
  TheDebuginatorQueue* queue        = (TheDebuginatorQueue*)queue_buffer;
  queue->num_items                  = 0;
  queue->capacity                   = initial_size;
  queue->userdata                   = userdata;
  queue->allocate                   = allocate_func;
  queue->deallocate                 = deallocate_func;
  queue->items                      = (DebuginatorQueueItem*)item_buffer;

  // queue->cleanup_funcs[DEBUGINATOR_QUEUE_CreateItem] =
  // debuginator_queue__create_bool_item_cleanup;

  return queue;
}

#endif // DEBUGINATOR_QUEUE_IMPLEMENTATION

#ifndef DEBUGINATOR_ENABLE_WARNINGS
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

#endif // INCLUDE_THE_DEBUGINATOR_H

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Anders Elfgren
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
