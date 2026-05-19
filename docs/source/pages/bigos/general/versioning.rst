Versioning
==========

:Author: Oskar Meler
:Date: 19-05-2026
:Revision: 2


Versioning Scheme
-----------------

All BigOS version numbers follow the ``generation.feature.patch`` format.

Generation
^^^^^^^^^^
Incremented when breaking changes are made to existing interfaces or formats.
A change in Generation indicates **backward-incompatible modifications**.

Feature
^^^^^^^
Incremented when new functionality is added in a backward-compatible manner.
Feature updates do not break compatibility with older versions within the same Generation.

Patch
^^^^^
Incremented for bug fixes, internal improvements, and other non-breaking changes that do not affect interface behavior or compatibility.


Data Format vs Software Version
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BigOS distinguishes between:

Client version
    The version of the running BigOS implementation (the consumer).

Interface version
    The version of the interface contract being consumed.

The compatibility rules define whether a given client version can correctly interact with a given interface version.


Compatibility Rule
~~~~~~~~~~~~~~~~~~

A client version ``C`` is compatible with an interface version ``I`` if and only if the following conditions are met:

- C.generation == I.generation
- C.feature ≥ I.feature

Patch versions do not affect compatibility.


Version Limits
~~~~~~~~~~~~~~

Version components are constrained as follows:

- Generation ≤ 65535
- Feature ≤ 65535
- Patch ≤ 65535


Implementation Notes
~~~~~~~~~~~~~~~~~~~~

stdbigos provides a :doc:`utility library </pages/api/libs/stdbigos/version>` for working with version values.
