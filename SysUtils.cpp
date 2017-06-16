
extern "C" {
  #include "user_interface.h"
}


uint32_t freeHeapSize() {
  return system_get_free_heap_size();
}
