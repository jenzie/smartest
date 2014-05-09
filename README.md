Smartest - The Smartest Little Computer
=======================================

We simualte a stripped-down pipelined MIPS architecture with
Branch Prediction (BP).  A Two-Way Adaptive Predictor is used as the algorithm
for making predictions to branch, or not to branch.  The Branch Prediction Table
(BPT) has slots for 16 branches, and will swap out branches for new ones once
the table is full.

The pipeline itself provides two regular clock ticks per stage to do work. There
are five stages total: Fetch, Decode, Execute, Memory, and Writeback. The
Fetch and Decode stages are responsible for branch prediction and purging of
mispredictions.  The pipeline does NOT support forwarding, or other advanced
features of modern pipelines.

Output
======

Instruction Output
------------------
Instruction output has four distinct parts, separated by lines. The below is an
example output for a logical clock tick (two clock ticks per stage).  Each
stage is given a 16 character width to print the work it performed.

    Read PC(03)     NOP             R1 = MEM[R3+2]  R0 = MEM[R3+1]  NOP
    No BPT Hit      -               B:00; IMM:02    MEM[ALU_OUT]    -
    No BPT Update   -               ALU_OUT<-02     MDR<-MEM[02]    -

The first line documents the instruction processed for that stage. Since the
Fetch stage can not opearte on an instruction, it simply reports the current
value of the PC register.

The second line is the work of that stage completed during the first clock tick.
Likewise, the third line is the work completed for that stage during the second
clock tick.  Important note, the work reported per clock tick per stage is not
an exact translation of the actual hardware components altered, or completely
inclusive of all hardware components updated.  Each stage only reports the
critical registers or components affected so as to minimize total output.

The fourth part that may be printed is the contents of the general registers,
which will only print if there was a changed performed during the Writeback 
stage. Below is an example output.  The values correspond to registers 1, 2, 3,
and 4 respectively.

    Registers 15 20 35 00

BPT Statistics
--------------

After the successful execution of a HALT instruction a BP statictics report
will print out summarizing various aspects of the effectiveness of the BP
algorithm.  An example is provided below.

    BPT Statistics
    ==============
    Predictions:    00
    Failures:       00
    Accuracy:       00%
    Unique Branches:00
    Branch Swapped: 00
    Saturation:     0.00

Saturation values <= 1 or below 1.5 are good, as larger values mean that more
branch history is lost to new branch instructions.  This is caused by a BPT 
that is too small, forcing the BP algorithm to swap out old branches for new 
ones.

Special Cases and Oddities
==========================

HALT Instruction
----------------

The HALT instruction will only terminate operation of the smartest computer at
the completion of the second clock tick for the Writeback stage.  This is
simply done as a convenience to make writing programs easier.  There is no need
to count the stages that the last instruction needs to execute before allowing
a HALT instruction to terminate.  This also eliminates the need to add special 
detection during the Fetch stage for HALT instructions, or disabling the
pipeline as stages "execute" the HALT, or having the HALT terminate at the
decode stage, which would be awkward.

Input Files
===========

The formatting and location of input files for use.

Format
------

The input files follow the same format as the labs.

Instruction object files, which contain read-only data for instruction exection,
are named "inst.N", where N is some unique number.

Data object files, which full read/write data, are named "data.N", where N is
the corresponding unique number of the instruction file.

Location
--------

    obj/

Building
========

    make

Executing
=========

    ./smartest inst_obj data_obj
