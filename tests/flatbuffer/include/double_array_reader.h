#ifndef DOUBLE_ARRAY_READER_H
#define DOUBLE_ARRAY_READER_H

/* Generated by flatcc 0.6.1 FlatBuffers schema compiler for C by dvide.com */

#ifndef FLATBUFFERS_COMMON_READER_H
#include "flatbuffers_common_reader.h"
#endif
#include "flatcc/flatcc_flatbuffers.h"
#ifndef __alignas_is_defined
#include <stdalign.h>
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif


typedef const struct DArray_table *DArray_table_t;
typedef struct DArray_table *DArray_mutable_table_t;
typedef const flatbuffers_uoffset_t *DArray_vec_t;
typedef flatbuffers_uoffset_t *DArray_mutable_vec_t;
#ifndef DArray_file_identifier
#define DArray_file_identifier 0
#endif
/* deprecated, use DArray_file_identifier */
#ifndef DArray_identifier
#define DArray_identifier 0
#endif
#define DArray_type_hash ((flatbuffers_thash_t)0xfc4ea228)
#define DArray_type_identifier "\x28\xa2\x4e\xfc"
#ifndef DArray_file_extension
#define DArray_file_extension "bin"
#endif



struct DArray_table { uint8_t unused__; };

static inline size_t DArray_vec_len(DArray_vec_t vec)
__flatbuffers_vec_len(vec)
static inline DArray_table_t DArray_vec_at(DArray_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(DArray_table_t, vec, i, 0)
__flatbuffers_table_as_root(DArray)

__flatbuffers_define_vector_field(0, DArray, values, flatbuffers_double_vec_t, 0)


#include "flatcc/flatcc_epilogue.h"
#endif /* DOUBLE_ARRAY_READER_H */
