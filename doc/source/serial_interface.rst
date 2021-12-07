.. _page-serial_interface:

The ChIMES Calculator Serial Interface
========================================

Overview
********

The ChIMES calculator serial interface provides an easier means of evaluating ChIMES interactions for a given system. In constrast to the ChIMES calculator (i.e. ``chimesFF``), which takes information on *individual* atom clusters and returns the cluster energy, stress tensor, via ``compute_xB`` functions, the serial interface (i.e. ``serial_chimes_interface``) takes *overall* system information and returns *overall* provides *overall* system energy, stress tensor, and forces. Though far less flexible than direct use of ``chimesFF``, ``serial_chimes_interface`` allows users to leverage ChIMES with much less coding. For further details on ``chimesFF``, see :ref:`The ChIMES Calculator <page-chimesFF>`. For a complete set of ChIMES references, see :ref:`Citing ChIMES <page-citing>`.


The ChIMES Calculator Serial Interface
****************************************

The ChIMES calculator serial interface source files are located in ``serial_interface/src/``. To use in a C++ code, simply ``#include "chimescalc_serial.h"`` in the target code and instantiate a ``serial_chimes_interface`` object. As described in greater detail below, ``serial_chimes_interface`` objects take information on the overall system and provide the corresponding ChIMES energy, stress tensor, and forces.  Any such code must at least include the following operations, in order:

    .. code-block:: cpp

       int my_rank = 0;
       // Instantiate
       serial_chimes_interface chimes;
       // Specify the parameter files and set  the MPI rank (replace with zero if used in serial code)
       chimes.init_chimesFF("my_parameter_file", my_rank);

.. Warning::

	When instantiated via ``serial_chimes_interface chimes``, the code will automatically replicate systems for
    which the smallest cell length is greater than the ChIMES outer cutoff.

    This will produce incorrect results unless:

    1. Configuration cell lengths are greater than teh ChIMES outer cutoff
    2. The material is perfectly crystalling

    Developer note: To recover behavior of the reserach code, instantiate with: ``serial_chimes_interface chimes(false)``

Note that the ChIMES calculator serial interface ``serial_chimes_interface`` class provides users with the following functions:

=========== =================  =================
Return Type Name               Arguments and Description
=========== =================  =================
void        init_chimesFF      =======================   =====
                               Type                      Description
                               =======================   =====
                               string                    Parameter file
                               int                       MPI rank
                               =======================   =====

                               Instantiates ``serial_chimes_interface`` object, sets rank, reads parameter file.
                               With the exception of error messages, the ChIMES calculator will only print output for rank 0.


void        calculate          =======================   =====
                               Type                      Description
                               =======================   =====
                               vector<double>            Vector of x-coordinates for system atoms
                               vector<double>            Vector of y-coordinates for system atoms
                               vector<double>            Vector of z-coordinates for system atoms
                               vector<double>            System cell a lattice vector
                               vector<double>            System cell b lattice vector
                               vector<double>            System cell c lattice vector
                               vector<string>            Vector of atom types for system atoms
                               double                    Overall system energy (updated by function)
                               vector<vector<double> >   Vector of forces for system atoms (updated by function); ([atom index][fx, fy, fz])
                               vector<double>            System stress tensor (updated by function); ([s_xx, s_xy, s_xz, s_yx, s_yy, s_yz, s_zx, s_zy, s_zz])
                               =======================   =====

                               Takes system coordinates and cell lattice vectors, computes corresponding ChIMES energy, stress tensor, and system forces.
=========== =================  =================



.. _sec-ser-c-api:

The C API
^^^^^^^^^

The C API (``chimescalc_serial_C*``) is located in ``serial_interface/api``. This wrapper provides C style name mangling and creates a  set of C-style wrapper functions. The latter are needed for compatibility with std::vector which is heavily used in ``serial_chimes_interface`` and not supported in most other languages. Any C code attempting to use the ChIMES calculator serial interface should ``#include "chimescalc_C.h"``
and at least include the following operations, in order:

    .. code-block:: cpp

       int my_rank = 0;
       set_chimes_serial();         // Instantiate; as for the C++ API (see warning message), can pass 0/1 for false/true
       init_chimes_serial("my_parameter_file", my_rank); // Set MPI rank (replace with zero if used in serial code)

For additional information on compiling, see :ref:`Implementation Examples <sec-ser-use-examples-api>`.

Note that the ChIMES calculator serial interface ``chimescalc_serial_C`` API provides users with the following functions:

=========== ========================    =================
Return Type Name                        Arguments and Description
=========== ========================    =================
void        set_chimes_serial           Creates a pointer to a ``serial_chimes_interface`` object.

                                        =======================   =====
					Type                      Description
					=======================   =====
					int                       Boolean: Allow replication? (0/1 for false/true); default = true
					=======================   =====


void        init_chimes_serial          =======================   =====
                                        Type                      Description
                                        =======================   =====
                                        string                    Parameter file
                                        int                       MPI rank
                                        =======================   =====

                                        Sets rank and reads the parameter file to the ``serial_chimes_interface`` object.
                                        With the exception of error messages, the ChIMES calculator will only print output for rank 0.

void        calculate_chimes            =======================   =====
                                        Type                      Description
                                        =======================   =====
                                        int                       number of atoms in system
                                        double array              Vector of x-coordinates for system atoms
                                        double array              Vector of y-coordinates for system atoms
                                        double array              Vector of z-coordinates for system atoms
                                        char  array               System cell a lattice vector
                                        double array              System cell b lattice vector
                                        double array              System cell c lattice vector
                                        double array              Vector of atom types for system atoms
                                        double*                   Overall system energy (updated by function)
                                        double array              Vector of forces for system atoms (updated by function); ([atom index][fx, fy, fz])
                                        double array              System stress tensor (updated by function); ([s_xx, s_xy, s_xz, s_yx, s_yy, s_yz, s_zx, s_zy, s_zz])
                                        =======================   =====

                                        Takes system coordinates and cell lattice vectors, computes corresponding ChIMES energy, stress tensor, and system forces.
=========== ========================    =================

.. _sec-ser-fortran-api:

The Fortran API
^^^^^^^^^^^^^^^

The Fortran API (``chimescalc_serial_F*``) is located in ``serial_interface/api``. This wrapper enables access to ``serial_chimes_interface`` functions
through the C API and handles other details like differences in array storage order.


Any Fortran code attempting to use the ChIMES Calculator should ``use chimescalc`` and at least include the following
operations, in order:

    .. code-block:: fortran

       integer(C_int) :: my_rank
       ! Instantiate; as for the C++ API (see warning message), can pass 0/1 for false/true
       call f_set_chimes()
       ! Specify the parameter files and set  the MPI rank (replace with zero if used in serial code)
       call f_init_chimes(string2Cstring("my_parameter_file"), my_rank)


For additional information on compiling, see :ref:`Implementation Examples <sec-ser-use-examples-api>`.

Note that the ChIMES calculator serial interface ``chimescalc_serial_F`` API provides users with the following functions:


=========== ========================    =================
Return Type Name                        Arguments and Description
=========== ========================    =================
none        f_set_chimes		Creates a pointer to a ``serial_chimes_interface`` object.

                                        =======================   =====
					Type                      Description
					=======================   =====
					C_int                     Boolean: Allow replication? (0/1 for false/true); default = true
                                        =======================   =====

none        f_init_chimes               =======================   =====
                                        Type                      Description
                                        =======================   =====
                                        C_char                    Parameter file
                                        C_int                     MPI rank
                                        =======================   =====

                                        Sets rank and reads the parameter file to the ``serial_chimes_interface`` object.
                                        With the exception of error messages, the ChIMES calculator will only print output for rank 0.


void        f_calculate_chimes          =======================   =====
                                        Type                      Description
                                        =======================   =====
                                        C_int                       number of atoms in system
                                        C_double array              Vector of x-coordinates for system atoms
                                        C_double array              Vector of y-coordinates for system atoms
                                        C_double array              Vector of z-coordinates for system atoms
                                        C_char  array               System cell a lattice vector
                                        C_double array              System cell b lattice vector
                                        C_double array              System cell c lattice vector
                                        C_double array              Vector of atom types for system atoms
                                        C_double*                   Overall system energy (updated by function)
                                        C_double array              Vector of forces for system atoms (updated by function); ([atom index][fx, fy, fz])
                                        C_double array              System stress tensor (updated by function); ([s_xx, s_xy, s_xz, s_yx, s_yy, s_yz, s_zx, s_zy, s_zz])
                                        =======================   =====

                                        Takes system coordinates and cell lattice vectors, computes corresponding ChIMES energy, stress tensor, and system forces.

C_string    string2Cstring              ======   ===
                                        Type     Description
                                        ======   ===
                                        string   Any text
                                        ======   ===

                                        Converts a Fortran string to a C_string
=========== ========================    =================



.. _sec-ser-python-api:

The Python API
^^^^^^^^^^^^^^

The Python API (``chimescalc_serial_py*``) is located in ``serial_interface/api``. Like the Fortran API, this wrapper enables access to
``serial_chimes_interface`` functions through the C API, via ctypes.

Any python code attempting to use the ChIMES Calculator should ``import chimescalc_serial_py`` and at least include the following
operations, in order:

    .. code-block:: python

       # Associate the wrapper with a compiled C API library file
       chimescalc_serial_py.chimes_wrapper = chimescalc_serial_py.init_chimes_wrapper("lib-C_wrapper-serial_interface.so")
       # Instantiate; as for the C++ API (see warning message), can pass 0/1 for false/true
       chimescalc_serial_py.set_chimes()
       # Read the parameter file, set MPI rank to 0 (i.e. no MPI used)
       chimescalc_serial_py.init_chimes("my_parameter_file", 0)


For additional information on compiling (i.e. generation of ``lib-C_wrapper-serial_interface.so``), see :ref:`Implementation Examples <sec-ser-use-examples-api>`.

Note that the ChIMES calculator serial interface ``chimescalc_serial_py`` API provides users with the following functions:


=============== ========================    =================
Return Type      Name                        Arguments and Description
=============== ========================    =================
See description init_chimes_wrapper         =======================   =====
                                            Type                      Description
                                            =======================   =====
                                            string                    Library name
                                            =======================   =====

                                            Associate ctypes.CDLL (i.e. the return type) with a the compiled ChIMES calculator serial interface C-library.


void            set_chimes                  Creates a pointer to a ``serial_chimes_interface`` object.

                                            =======================   =====
                                            Type                      Description
                                            =======================   =====
                                            bool                      Allow replication? ; default = true
                                            =======================   =====


void            init_chimes                 =======================   =====
                                            Type                      Description
                                            =======================   =====
                                            string                    Parameter file
                                            int                       MPI rank
                                            =======================   =====

                                            Sets rank and reads the parameter file to the ``serial_chimes_interface`` object.
                                            With the exception of error messages, the ChIMES calculator will only print output for rank 0.

See description calculate_chimes            =======================   =====
                                            Type (input)              Description
                                            =======================   =====
                                            int                       number of atoms in system
                                            float list                Vector of x-coordinates for system atoms
                                            float list                Vector of y-coordinates for system atoms
                                            float list                Vector of z-coordinates for system atoms
                                            str list                  System cell a lattice vector
                                            float list                System cell b lattice vector
                                            float list                System cell c lattice vector
                                            float list                Vector of atom types for system atoms
                                            float                     Overall system energy
                                            float list                Vector of forces for system atoms ([atom index][fx, fy, fz])
                                            float list                System stress tensor ([s_xx, s_xy, s_xz, s_yx, s_yy, s_yz, s_zx, s_zy, s_zz])
                                            =======================   =====

                                            Takes system coordinates and cell lattice vectors, computes corresponding ChIMES energy, stress tensor, and system forces.

                                            =======================   =====
                                            Type (return)             Description
                                            =======================   =====
                                            float list                List of x-force components for system atoms
                                            float list                List of y-force components for system atoms
                                            float list                List of z-force components for system atoms
                                            float list                System stress tensor [s_xx, s_xy, s_xz, s_yx, s_yy, s_yz, s_zx, s_zy, s_zz]
                                            float                     System energy
                                            =======================   =====

=============== ========================    =================






---------------

.. _sec-ser-use-examples-api:

Implementation Examples
^^^^^^^^^^^^^^^^^^^^^^^

The following codes demonstrates how ``serial_chimes_interface.{h,cpp}`` can be used to obtain the overall stress tensor, energy, and per-atom forces for a given system configuration using C, C++ Fortran, and Python. See the ``main.*`` files in each corresponding subdirectory of ``serial_interface/examples`` for further implementation details. Note that sample system configurations (i.e. ``*xyz`` files) and parameter files can be found in ``serial_interface/test/configurations`` and ``serial_interface/test/force_fields``, respectively.
For user generated tests, note that ``*.xyz`` files must provide lattice vectors in the comment line, e.g. lx 0.0 0.0 0.0 ly 0.0 0.0 0.0 lz. Click :ref:`here <page-units>` for an overview of ChIMES units.

.. Note::

    All implementation examples are intended to be run on Unix-based systems (e.g. Linux, OSX).

.. Warning::

     These codes are for demonstrative purposes only and come with no guarantees.

.. Note::

    All example executables can be compiled at once via ``./install.sh`` from the ``chimes_calculator`` base directory, and similarly uninstalled via ``./uninstall.sh``. However, the examples below compile via the user-generated Makefiles located in each ``examples`` subdirectory, for demonstrative purposes.


* **C Example:** The ``main`` function of this example includes the C API, ``chimescalc_serial_C.{h,cpp}``, which creates a global static pointer to a ``serial_chimes_interface`` object.
  The ``serial_chimes_interface`` pointer object is set up, i.e. by ``set_chimes_serial()``, and used for access to ``serial_chimes_interface`` member functions, etc.

   * Navigate to ``serial_interface/examples/c``
   * Compile with: ``make all``
   * Test with: ``./C_wrapper-serial_interface <parameter file> <xyz file>``

* **C++ Example:** The ``main`` function of this example creates an instance of ``serial_chimes_interface`` (i.e. a class inheriting ``chimesFF``,
  which computes energy, per-atom forces, and stress tensor for an overall system). For additional details, see :ref:`The ChIMES Calculator <page-chimesFF>`

   * Navigate to ``serial_interface/examples/cpp``
   * Compile with: ``make all``
   * Test with: ``./CPP-interface <parameter file> <xyz file>``

* **Fortran Example:** Similar to the C example, this ``main`` function establishes a pointer to a ``serial_chimes_interface`` object via ``f_set_chimes()``.
  The ``f_set_chimes()`` function call is defined in ``chimescalc_serial_F.F90,`` a wrapper for the C API ``chimescalc_serial_C.cpp`` (i.e which facilitates C-style access to
  ``serial_chimes_interface`` member functions, etc). Actual linking is achieved at compilation. See the ``Makefile`` for details.

   * Navigate to ``serial_interface/examples/fortran``
   * Compile with: ``make all``
   * Test with: ``./fortran_wrapper-serial_interface <parameter file> <xyz file>``
   * Additional notes:

* **Python Example:** This example accesses ``serial_chimes_interface`` functions through ``chimescalc_serial_py.py``, a ctypes-based python API for access to the C API functions
  (i.e. through ``chimescalc_serial_C.cpp``). Once ``chimescalc_serial_py.py`` is imported, it is associated with a compiled C API library file, i.e. ``lib-C_wrapper-serial_interface.so`` and  can be used to access ``serial_chimes_interface`` member functions.

   * Navigate to ``serial_interface/examples/python``
   * Compile lib-C_wrapper-serial_interface.so with: ``make all``
   * Test with: python main.py <parameter file> <coordinate file>
   * Additional notes:
