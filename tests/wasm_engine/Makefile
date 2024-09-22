MODULE_big = wasm_engine

EXTENSION = wasm_engine
DATA = wasm_engine--1.0.sql

# PG_CPPFLAGS = -I$(libpq_srcdir)
SHLIB_LINK_INTERNAL = $(libpq)
SHLIB_LINK += -lvmlib
SHLIB_LINK += -l:libflatccrt.so
SHLIB_LINK += -ldocker
SHLIB_LINK += -lcurl
SHLIB_LINK += -lstdc++
PG_LDFLAGS += -L.

# PG_LDFLAGS += -L/home/xiaqian/wasm-udf/performance/flatbuffer/lib
PG_LDFLAGS += -L/home/xiaqian/wasm-udf/pgsql/lib
PG_LDFLAGS += -L/home/xiaqian/wasm-udf/pgsql/libdocker/
PG_LDFLAGS += -L/usr/lib
PG_LDFLAGS += -L/home/xiaqian/wasm-udf/pgsql
# wamr
WAMR_ROOT_DIR = /home/xiaqian/wasm-udf/wasm-micro-runtime
SHARED_DIR = $(WAMR_ROOT_DIR)/core/shared
UNCOMMON_SHARED_DIR = $(SHARED_DIR)/utils/uncommon
UNCOMMON_SHARED_SOURCE = $(wildcard $(UNCOMMON_SHARED_DIR)/*.c)
# UNCOMMON_SHARED_SOURCE_OBJ = $(UNCOMMON_SHARED_SOURCE:.c=.o)
$(info $(UNCOMMON_SHARED_SOURCE:.c=.o))

OBJS = wasm_engine.o  $(UNCOMMON_SHARED_SOURCE:.c=.o)


PG_CPPFLAGS += -I$(SHARED_DIR)/platform/linux
PG_CPPFLAGS += -I$(SHARED_DIR)/utils
PG_CPPFLAGS += -I$(UNCOMMON_SHARED_DIR)
PG_CPPFLAGS += -I$(WAMR_ROOT_DIR)/core/iwasm/include
PG_CPPFLAGS += -I$(WAMR_ROOT_DIR)/core/iwasm/include
PG_CPPFLAGS += -I$(WAMR_ROOT_DIR)/core
# PG_CPPFLAGS += -I/usr/src/linux-headers-5.15.0-92-generic/include/generated
# PG_CPPFLAGS += -I/usr/src/linux-headers-5.15.0-100-generic/include/generated
PG_CPPFLAGS += -I/home/xiaqian/wasm-udf/performance/flatbuffer/include
PG_CPPFLAGS += -I/home/xiaqian/wasm-udf/pgsql

#PG_CPPFLAGS += -I/home/xiaqian/gpdb-archive/src/include

# PG_CPPFLAGS += -lflatcc -lflatcc_d.a -lflatccrt_d.a -lflatccrt.a


# $(info $(KERNEL_VERSION_NUMBER))
# $(info $(SHARED_DIR))
# $(info $(UNCOMMON_SHARED_DIR))
# $(info $(PG_CPPFLAGS))


# USE_PGXS=1

# ifdef USE_PGXS
# PG_CONFIG = pg_config
# PGXS := $(shell $(PG_CONFIG) --pgxs)
# include $(PGXS)
# else
# $(info NO_USE_PGXS)
# This should be built on the top of source tree
#subdir = contrib/wasm
#top_builddir = ../..
#include /usr/lib/postgresql/14/lib/pgxs/src/Makefile.global
#include $(top_srcdir)/contrib/contrib-global.mk
#override CPPFLAGS := $(filter-out -fPIE, $(CPPFLAGS)) -fPIC
#endif
ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/wasm
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

