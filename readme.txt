/******************************************************************************
 *                                Overview
 ******************************************************************************/
 This snip demonstrates the dedicated keyscan hardware which processes key press
 events from LHL GPIOs without interrupting the CPU core. This snip also
 demonstrates low power entry when the system is idle (no activity/keypress)
 and wakes up when there is key press.

 /******************************************************************************
 *                                Features demonstrated
 ******************************************************************************/
 1. Functionality of Keyscan with max possible buttons on CYW9208XXEVB-02EVB
    (7 Rows * 9 Columns).
    ------------------------
    ROW#        Mapped Px#
    ------------------------
    0           P0
    1           P1
    2           P2
    3           P3
    4           P4
    5           P5
    6           P6
    ------------------------
    COL#        Mapped Px#
    ------------------------
    0           P8
    1           P9
    2           P10
    3           P11
    4           P12
    5           P13
    6           P14
    7           P15
    8           P17

 2. Low power Configuration.

 /******************************************************************************
 *                                Instructions
 ******************************************************************************/
 To demonstrate the app, work through the following steps.
 1. Plug the CYW9208XXEVB-02 evaluation board to your computer.
 2. Build and download the application. (See CYW9208XXEVB-02 User Guide)
 3. Connect a matrix button keys to the corresponding rows(P0-P7) and
    the supermuxed columns(any of P0-P39) on the evaluation board. (or)
    In case, there is no availability of matrix button keys, bring the
    corresponding row and column pins in contact by flywiring.
 4. Use Terminal emulation tools like Teraterm or Putty to view the
    trace messages(See Kit User Guide).