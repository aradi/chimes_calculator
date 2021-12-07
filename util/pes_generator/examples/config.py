"""

    Configuration file for the ChIMES potential energy surface generator (pes_generator.py)

	Don't forget to set PAIR CHEBYSHEV PENALTY SCALING to zero in the parameter file.

"""


CHMS_REPO  = "/Users/lindsey11/My_codes/Forks/chimes_calculator-fork/"

PARAM_FILE = "/Users/lindsey11/My_codes/Forks/chimes_calculator-fork/serial_interface/tests/force_fields/test_params.CHON.txt"

PAIRTYPES  = [0,    3,    5   ] # Pair type index for scans, i.e. number after "PAIRTYPE PARAMS:" in parameter file
PAIRSTART  = [1.0,  1.0,  1.0 ] # Smallest distance for scan
PAIRSTOP   = [4.0,  4.0,  4.0 ] # Largest distance for scan
PAIRSTEP   = [0.01, 0.01, 0.01] # Step size for scan

TRIPTYPES  = [1,    4   ] # Triplet type index for scans, i.e. number after "TRIPLETTYPE PARAMS:" in parameter file
TRIPSTART  = [1.0,  1.0 ] # Smallest distance for scan
TRIPSTOP   = [4.0,  4.0 ] # Largest distance for scan
TRIPSTEP   = [0.10, 0.10] # Step size for scan

# The example parameter file doesn't contain four body interactions, so the following is not needed.
# If four body scans are desired, keep in mind a small step size will take a long time to run
# Start with something very large to get a handle on run time, and modify from there
#
#QUADTYPES  = [7   ] # Triplet type index for scans, i.e. number after "TRIPLETTYPE PARAMS:" in parameter file
#QUADSTART  = [1.0 ] # Smallest distance for scan
#QUADSTOP   = [4.0 ] # Largest distance for scan
#QUADSTEP   = [1.00] # Step size for scan
