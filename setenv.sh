export LD_LIBRARY_PATH=/opt/rocprofiler-systems/lib
export ROCPROFSYS_CI=ON
export ROCPROFSYS_OUTPUT_PATH=rocprofsys-tests-output
export ROCPROFSYS_CI_TIMEOUT=120

export OMP_PLACES=threads
export OMP_NUM_THREADS=2
export ROCPROFSYS_CONFIG_FILE=$PWD/prog.cfg
export ROCPROFSYS_DEBUG_SETTINGS=1
