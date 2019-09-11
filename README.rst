===========================
Fledge "RMS" Filter plugin
===========================

Simple readings data transformation plugin that calculates the RMS value
of data points over a set sample range. It may optionally also include
peak to peak measurements (i.e. the maximum swing) within the same data
period as the RMS value is calculated.

Note, peak values may be less than individual values of the input if the
asset value does not fall to or below zero. Where a data value swings
between negative and positive values then the peak value will be greater
than the maximum value in the data stream. For example if the minimum value
of a data point in the sample set is 0.3 and the maximum is 3.4 then the peak
value will be 3.1. If the maximum value is 2.4 and the minimum is zero then
the peak will be 2.4. If the maximum value is 1.7 and the minimum is -0.5
then the peak value will be 2.2.

The user may also choose to include or not the raw data that is used to
calculate the RMS values via a switch in the configuration.

Where a datastream has multiple assets within it the RMS filter may
be limited to work only on those assets whose name matches a regular
expression given in the configuration of the filter. The default for
this expression is .*, i.e. all assets are processed.

Runtime configuration
=====================

A number of configuration options exist:

samples
  The number of data samples to perform a calcuation over

assetName
  The asset name to use to output the RMS values. "%a" will be replaced
  with the original asset name.

rawData 
  A switch to include the raw input data in the output

peak
  A switch to include peak to peak measurements for the same data set
  as the RMS measurement

match
  A  regular expression to limit the asset names on which this filter
  operations

Build
-----
To build Fledge "RMS" C++ filter plugin:

.. code-block:: console

  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the Fledge develop package header files and libraries
  are expected to be located in /usr/include/fledge and /usr/lib/fledge
- If **FLEDGE_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FLEDGE_ROOT directory.
  Please note that you must first run 'make' in the FLEDGE_ROOT directory.

You may also pass one or more of the following options to cmake to override
this default behaviour:

- **FLEDGE_SRC** sets the path of a Fledge source tree
- **FLEDGE_INCLUDE** sets the path to Fledge header files
- **FLEDGE_LIB sets** the path to Fledge libraries
- **FLEDGE_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FLEDGE_INCLUDE** option should point to a location where all the Fledge
   header files have been installed in a single directory.
 - The **FLEDGE_LIB** option should point to a location where all the Fledge
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FLEDGE_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FLEDGE_ROOT set

  $ export FLEDGE_ROOT=/some_fledge_setup

  $ cmake ..

- set FLEDGE_SRC

  $ cmake -DFLEDGE_SRC=/home/source/develop/Fledge  ..

- set FLEDGE_INCLUDE

  $ cmake -DFLEDGE_INCLUDE=/dev-package/include ..
- set FLEDGE_LIB

  $ cmake -DFLEDGE_LIB=/home/dev/package/lib ..
- set FLEDGE_INSTALL

  $ cmake -DFLEDGE_INSTALL=/home/source/develop/Fledge ..

  $ cmake -DFLEDGE_INSTALL=/usr/local/fledge ..
