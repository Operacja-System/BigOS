=======================
Memory regions
=======================

This module provides iterators to enumerate memory regions from the device tree:

- **Memory regions**: Physical memory regions available for use (from /memory nodes)
- **Reserved regions**: Memory ranges reserved by the system and must not be allocated
  (from /reserved-memory node and memory reservations)

Usage Pattern
=============

Both iterators follow the same init-then-iterate pattern:

.. code-block:: c

    // Initialize iterator
    hal_memory_iterator_t iter;
    if (hal_get_memory_regions_iterator(&iter) != ERR_NONE)
        return; // Handle error

    // Iterate through regions
    physical_memory_region_t region;
    while (hal_get_next_memory_region(&iter, &region) == ERR_NONE) {
        // Process region
    }
    // Loop exits when hal_get_next_memory_region returns ERR_NOT_FOUND

Return Value Semantics
======================

- ``ERR_NONE``: Successfully retrieved next region; output parameter is valid
- ``ERR_NOT_FOUND``: End of iteration reached (normal termination, not an error)
- Other error codes: Actual error occurred; output parameters remain unchanged

API Reference
=============

.. doxygengroup:: hal_riscv_mem
