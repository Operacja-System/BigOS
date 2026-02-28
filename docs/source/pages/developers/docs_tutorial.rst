Documentation Guidelines
========================

:Author: Oskar Meler
:Date: 24-02-2026
:Revision: 1

This page describes the BigOS documentation workflow, style guidelines,
and overall approach to documentation.

--------
Workflow
--------

.. _Sphinx: https://www.sphinx-doc.org/en/master/index.html
.. _Doxygen: https://www.doxygen.nl/index.html
.. _reStructuredText: https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html

BigOS documentation is generated using `Sphinx`_ and `Doxygen`_.
All documentation-related files reside in the ``/docs/`` directory.

Directory structure:

* ``/docs/source/``
  Contains the source files and configuration for `Sphinx`_.

* ``/docs/doxygen/``
  Contains manual entries for `Doxygen`_, such as group definitions.

* ``/docs/source/pages/wiki/``
  Contains general operating systems development knowledge that is not specific to BigOS.

* ``/docs/source/pages/bigos/``
  Contains detailed explanations of BigOS systems and interfaces.

* ``/docs/source/pages/api/``
  Contains auto-generated API documentation produced by `Doxygen`_.

* ``/docs/source/pages/developers/``
  Contains information relevant to BigOS contributors and developers.

* ``/docs/source/pages/about/``
  Contains general information about the BigOS project.

All documentation files are written in `Sphinx`_-flavored `reStructuredText`_.

-----
Style
-----

All documentation must follow a formal, academic writing style.
Text should be precise, technically accurate, and free of colloquial language.

Every documentation page must include an Author and Date field
at the beginning of the document, after the title, formatted as follows:

Single author::

   :Author: John Bigos

Multiple authors::

   :Author: John Bigos, Bohn Jigos

Date::

   :Date: dd-mm-yyyy

All documentation pages containing guidelines or rules must include a Revision field.
The revision number shall start at 1 and must be incremented for every logical change.

    .. note::

        The purpose of the Author field is to indicate who should be contacted regarding the content of the page.
        An author should be added only when modifying the content of a page, not for stylistic, formatting, or consistency changes.

-------------------------
Approach to Documentation
-------------------------

The objective of BigOS documentation is to provide a comprehensive knowledge base for
operating systems that enables readers with a basic understanding of low-level programming to fully comprehend

* How BigOS systems function
* How internal components interact
* Why certain design decisions were made
* How external interfaces should be used

Documentation is considered equally important as source code.

It must remain up to date at all times and should cover all topics
necessary to understand the implementation and design of BigOS.

The overarching objective of this project is that, after carefully
studying it, a reader should be capable of designing
and implementing an operating system from scratch, as well as
understanding existing operating systems with confidence.
All documentation must meaningfully contribute to achieving this objective.
