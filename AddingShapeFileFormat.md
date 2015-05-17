This page describes how to add support of new format of file with discretized volumetric description of the shape, i.e. the one which directly describes a set of dipoles to be used in DDA simulations. If you want to implement a more high-level shape option, which require discretization by `ADDA` (even if arguments of this shape are provided by file), look at [adding a new predefined shape](AddingShape.md) instead.

Currently all formats supported by `ADDA` are two-way (both for reading and writing). It is very desirable to keep it this way. So the instructions below describe adding full support for a new format. However, it should be trivial to locate parts specific to reading or writing. Moreover, currently `ADDA` automatically detects the file format. If that is problematic for the new format, you may either hard-link the format to file extension or [add additional command line option](AddingCommandLineOption.md) to give more control to the user.

All places in the code, where something should be added, are commented. These comments start with "`TO ADD NEW FORMAT OF SHAPE FILE`" and contain detailed description; the text below is less detailed. Added code should be analogous to shape-file formats already present. The procedure is the following:
  * Add a descriptor for new shape file format in the file [const.h](http://code.google.com/p/a-dda/source/browse/trunk/src/const.h). Starting descriptor name with `SF_` is recommended, as well as adding a descriptive comment.
  * Add information about the new format in the file [param.c](http://code.google.com/p/a-dda/source/browse/trunk/src/param.c):
    * Modify the description of command line `-sg_format` in static structure `options` to include new format and its short description.
    * Add correspondence between argument of `-sg_format` and assignment of global variable `sg_format` in function `PARSE_FUNC(sg_format)`.
  * Implement write support of new format in function `SaveGeometry` in [make\_particle.c](http://code.google.com/p/a-dda/source/browse/trunk/src/make_particle.c). It should include:
    * Definition of default filename (by supplying an appropriate extension).
    * Writing header to the beginning of the file.
    * Writing a single line for each dipole (this is done in parallel).
  * Implement automatic detection of new shape format and first scan through the file in function `InitDipFile` in [make\_particle.c](http://code.google.com/p/a-dda/source/browse/trunk/src/make_particle.c), including:
    * Scanning the beginning of file detecting the correct format and assigning the following common variables: `read_format`, `rf_text`, `mustbe`, and `skiplines`. The code for new format should be incorporated into existent auto-detection logic.
    * Scanning each data line and assigning the following common variables: `x`,`y`,`z` and possibly `mat`. Variable `scanned` should be set to return value of `sscanf`.
    * Consistency checks, both for every scanned data line and for the whole file.
  * Add code to scan and process a single data line during second (main) scan through the file in function `ReadDipFile` in [make\_particle.c](http://code.google.com/p/a-dda/source/browse/trunk/src/make_particle.c). The code is similar to the one in previous section, but can be simpler because consistency checks are no longer required.

Please refer to section _Construction of a dipole set_ of the [manual](http://a-dda.googlecode.com/svn/trunk/doc/manual.pdf) for general overview of how `ADDA` interacts with shape files.

If you add a new format of shape file to `ADDA` according to the described procedure, please consider [contributing your code](InstructionCommitters.md) to be incorporated in future releases.