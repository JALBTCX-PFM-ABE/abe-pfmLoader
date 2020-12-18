
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/




#ifndef VERSION

#define     VERSION     "PFM Software - pfmLoader V3.14 - 08/20/19"

#endif

/*!< <pre>

    Version 1.00
    Jan C. Depner
    09/21/10

    First version.


    Version 1.01
    Jan C. Depner
    10/20/10

    Time as a depth attribute is now set across all input files that have time.


    Version 1.02
    Jan C. Depner
    12/13/10

    Fixed bug when setting HOF standard user flags.  I forgot that abdc of -70 is land just as abdc of 70 is.


    Version 1.03
    Jan C. Depner
    01/06/11

    Correct problem with the way I was instantiating the main widget.  This caused an intermittent error on Windows7/XP.


    Version 1.10
    Jan C. Depner
    01/17/11

    Added support for CHRTR2 data as we phase out the old CHRTR format.  Still using PFM_CHRTR_DATA flag.  Check the 
    file name on unload to determine the actual type (for now).


    Version 1.20
    Jan C. Depner
    02/08/11

    Added support for AHAB Hawkeye attributes.


    Version 1.21
    Jan C. Depner
    02/16/11

    Now converts path separators to native separators (\ on Windows) before writing the file name to the list file.


    Version 1.22
    Jan C. Depner
    02/23/11

    Shortened HAWKEYE attribute names because they were blowing out the PFM attribute name fields.


    Version 1.23
    Jan C. Depner
    02/25/11

    Now loads both PFM_HAWKEYE_HYDRO_DATA and PFM_HAWKEYE_TOPO_DATA.


    Version 1.24
    Jan C. Depner
    04/25/11

    Removed the "shoreline depth swapped" HOF lidar user flag and combined it with the "shallow water processed" HOF
    lidar user flag.  Replaced the "shoreline depth swapped" user flag with an "APD" user flag.  Since "shoreline depth swapped"
    (the old shallow water processing method) is never used with "shallow water processed" data processing this should
    cause no problems.


    Version 1.25
    Jan C. Depner
    06/16/11

    Removed HMPS_SPARE_1 flag check since we don't do swath filtering anymore and it's been removed from hmpsflag.h.


    Version 1.30
    Jan C. Depner
    07/21/11

    Added support for DRAFT CZMIL data.


    Version 1.31
    Jan C. Depner
    08/15/11

    Added in all of the currently available CZMIL attributes.  Many more to come.


    Version 1.32
    Jan C. Depner
    09/21/11

    Added ability to read an "update" parameter file (.upr) from pfmLoad.
    Replaced bin_inside calls with bin_inside_ptr calls.
    Added U.S. Government diclaimer comments to all files.


    Version 1.33
    Jan C. Depner
    10/06/11

    Fixed bug in TOF loader.  I was under the impression that if the last return was bad (-998.0)
    then the first return must be bad as well.  This is not the case.


    Version 1.34
    Jan C. Depner
    10/21/11

    Added a bunch of GSF attributes from the gsfSwathBathyPing record arrays.


    Version 1.35
    Jan C. Depner
    10/31/11

    Now sets max cache memory size (from parameter file) for PFM library.  Happy Halloween!


    Version 1.40
    Jan C. Depner
    01/10/12

    Finally got to check the north of 64N loading with geographic bins.  Turns out that we didn't
    need it (and it didn't work properly anyway).  So, I've commented out that code.


    Version 1.41
    Jan C. Depner
    02/21/12

    Now translates between PFM_DESIGNATED_SOUNDING flag and GSF HMPS_SELECTED_DESIGNATED flag.


    Version 1.42
    Jan C. Depner
    03/09/12

    Reads UTM BAG files and converts to lat/lon... sort of.  I'm not sure if this is absolutely correct
    but it looks right.


    Version 1.43
    Jan C. Depner
    05/17/12

    Fixed bug in GSF_PFM_Processing where I assumed (you know what they say about that ;-) that there
    would NEVER be more than 1024 beams.  DOH!


    Version 1.44
    Jan C. Depner
    06/13/12

    Removed temporary CZMIL stuff while finalizing the CZMIL API.


    Version 1.45
    Jan C. Depner
    07/14/12

    - Added support for the preliminary CZMIL CPF format data.
    - Changed all arguments to attribute setting routines to pointers instead of structures.  DOH!


    Version 1.46
    Jan C. Depner
    08/08/12

    Doesn't try to reset PFM_CHECKED and/or PFM_VERIFIED flags during recompute if it' creating a
    new PFM.  This may or may not be the source of segfaults when creating very large, new PFMs.


    Version 1.47
    Jan C. Depner
    08/15/12

    Force flush of the error file after each fprintf to it.


    Version 1.48
    Jan C. Depner
    09/27/12

    Added interest point, interest point rank, channel, and return number as CZMIL attributes.


    Version 1.49
    Jan C. Depner
    10/23/12

    - Added T0 interest point, Kd, and laser energy as CZMIL attributes.
    - Only 8 shopping days left to retirement!


    Version 1.50
    Jan C. Depner
    11/01/12

    - CZMIL points with an ip_rank of 0 (water surface for water shots) will not be loaded.
    - Only 1 shopping day left to retirement!


    Version 1.51
    Jan C. Depner (PFM Software)
    06/04/13

    - Option to load CZMIL null values added.


    Version 1.52
    Jan C. Depner (PFM Software)
    06/05/13

    - Changed "Load CZMIL Water Surface" option to "Invalidate CZMIL Water Surface".
    - Added "Load CZMIL Channel Data" options.


    Version 1.53
    Jan C. Depner (PFM Software)
    06/18/13

    - Added "Invalidate CZMIL Channel Data" options.


    Version 1.54
    Jan C. Depner (PFM Software)
    06/21/13

    - Now creates an initial parameter file (.ipf) in the .pfm.data folder for newly created PFMs.  This can be
      used on subsequent appends if needed.  Mostly it's there for the reload of reprocessed CZMIL data using
      pfm_czmil_reload.


    Version 1.55
    Jan C. Depner (PFM Software)
    06/24/13

    - Added "CZMIL Detection probability" and "CZMIL Validity reason" to the possible CZMIL attributes.


    Version 1.56
    Jan C. Depner (PFM Software)
    07/17/13

    - Changed the "Invalidate CZMIL Water Surface" option to "Load CZMIL Water Surface" option.  We really
      have no good reason for loading water surface data.  We never loaded it for CHARTS and I doubt if
      we'll need it for CZMIL.  Another reason for not loading it is that the unload program will take
      forever to finish if you invalidate the water surface.


    Version 1.57
    Jan C. Depner (PFM Software)
    12/16/13

    Added CZMIL standard user flag options.


    Version 1.58
    Jan C. Depner (PFM Software)
    12/19/13

    Changes to deal with CZMIL pre 2.0 reflectance values being set to .99 for NULL.


    Version 1.59
    Jan C. Depner (PFM Software)
    01/08/14

    Moved the CZMIL standard user flag setting code so that invalid points will also be flagged.


    Version 1.60
    Jan C. Depner (PFM Software)
    01/19/14

    Switched to CZMIL_READONLY_SEQUENTIAL for CPF files in order to, hopefully, speed up reads.


    Version 1.61
    Jan C. Depner (PFM Software)
    01/20/14

    Now using processing_mode instead of classification after changes to czmil API to handle
    processing mode being stored in classification prior to libCZMIL 2.0.  Added processing_mode
    to CZMIL attributes.


    Version 1.62
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 1.63
    Jan C. Depner (PFM Software)
    03/03/14

    Replaced HMPS flags with NV_GSF flags.


    Version 1.64
    Stacy Johnson (NAVO)
    03/13/14

    Since chrtr2 was moved to grid registration we no longer need the half node shifts
    Since bag is grid registration and not a pixel, remove half nodes
    Added support for Bag 1.5.2


    Version 1.65
    Jan C. Depner (PFM Software)
    03/17/14

    Removed WLF support.  Top o' the mornin' to ye!


    Version 1.66
    Jan C. Depner (PFM Software)
    05/07/14

    - Fixed string literal problems in format strings.
    - Check error return from fgets call.


    Version 1.67
    Jan C. Depner (PFM Software)
    05/27/14

    - Removed UNISIPS support.


    Version 1.68
    Jan C. Depner (PFM Software)
    05/27/14

    Now supports PFMv6 instead of PFMv5.


    Version 1.69
    Jan C. Depner (PFM Software)
    06/19/14

    - Removed PFMWDB support.  No one was using it.  It seemed like a good idea but I guess not.


    Version 1.70
    Jan C. Depner (PFM Software)
    07/05/14

    - Had to change the argument order in pj_init_plus for the UTM projection.  Newer versions of 
      proj4 appear to be very sensitive to this.


    Version 1.80
    Jan C. Depner (PFM Software)
    07/13/14

    - Added support for LAS files.  Only geographic or projected UTM using NAD83 zones 3-23 or WGS86
      zones 1-60 north or south.


    Version 1.81
    Jan C. Depner (PFM Software)
    07/17/14

    - No longer uses liblas.  Now uses libslas (my own API for LAS).


    Version 1.82
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 1.83
    Jan C. Depner (PFM Software)
    07/29/14

    - Fixed errors discovered by cppcheck.


    Version 1.84
    Jan C. Depner (PFM Software)
    09/04/14

    Support for new CZMIL v2 fields.


    Version 1.85
    Jan C. Depner (PFM Software)
    10/20/14

    I hate it when I make stupid mistakes.  When I added LAS support I hosed the CZMIL load.  DOH!


    Version 1.86
    Jan C. Depner (PFM Software)
    10/21/14

    More CZMIL v2 changes (specifically, removal of validity_reason).


    Version 1.87
    Jan C. Depner (PFM Software)
    10/24/14

    Added "CZMIL Average Depth" and "CZMIL Null" to CZMIL standard user flags.


    Version 1.90
    Jan C. Depner (PFM Software)
    11/12/14

    Handles user selected PFM_USER flags in any order (boy was that complicated).


    Version 1.91
    Jan C. Depner (PFM Software)
    11/24/14

    Made the loader ignore CZMIL_REPROCESSING_BUFFER points.


    Version 1.92
    Jan C. Depner (PFM Software)
    01/06/15

    Added code to deal with new leap second to be added on June 30th, 2015 at 23:59:60.


    Version 1.93
    Jan C. Depner (PFM Software)
    01/27/15

    Outputs message if error encountered reading CZMIL files.


    Version 2.00
    Jan C. Depner (PFM Software)
    03/12/15

    - Replaced my libslas with rapidlasso GmbH LASlib and LASzip.
    - Made preliminary file check always happen.


    Version 2.01
    Jan C. Depner (PFM Software)
    03/14/15

    - Since AHAB Hawkeye has switched to LAS format I have removed support for the old Hawkeye I binary format.


    Version 2.02
    Jan C. Depner (PFM Software)
    03/19/15

    - For LAS files, some people neglect to put in the GeographicTypeGeoKey just assuming that it's WGS_84.
      If we don't find the key we'll do the same.


    Version 2.03
    Jan C. Depner (PFM Software)
    03/30/15

    - Now handles uppercase LAS and LAZ file extensions for Windows afflicted files that have been transferred
      to operating systems that UnDeRsTaNd ThE DiFfErEnCe between upper and lowercase characters.


    Version 2.04
    Jan C. Depner (PFM Software)
    03/31/15

    - Fixed name filter for GSF files so that it wouldn't find files that don't end in .dNN where NN is numeric.
      This prevents us from seeing, for example, ESRI shape .dbf files in the GSF name list.


    Version 2.05
    Jan C. Depner (PFM Software)
    04/07/15

    - Fixed CZMIL flag settings.  I was breaking out of the loop for things other than NULL.  There can be multiple
      flags per point.  They are not mutually exclusive.


    Version 2.06
    Jan C. Depner (PFM Software)
    04/08/15

    - Correctly handles LAS v1.4 data.


    Version 2.07
    Jan C. Depner (PFM Software)
    04/14/15

    - Changed point counters to 64 bit integers.


    Version 2.08
    Jan C. Depner (PFM Software)
    05/07/15

    - Changed the point counters in the filter summery to 64 bit integers.


    Version 2.09
    Jan C. Depner (PFM Software)
    05/12/15

    - Removed options to invalidate CZMIL channels.  Invalidating an entire channel caused unload times to 
      be absolutely ridiculous.  If you need to actually kill entire channels do it in the CPF file with cpfFilter.


    Version 2.20
    Jan C. Depner (PFM Software)
    05/19/15

    - Added CSF attributes to the possible CZMIL attributes.  Using any of them will cause the load time to increase by
      approximately 3%.


    Version 2.21
    Jan C. Depner (PFM Software)
    05/21/15

    - Fixed problem with trying to detect similar attribute names in readParameterFile.cpp without the brackets on each end.


    Version 2.22
    Jan C. Depner (PFM Software)
    06/03/15

    - Fixed czmil user flag bug caused by conflict with CUBE attribute placeholders (which wasn't correct).
    - Set CUBE attribute placeholder default to "off".


    Version 2.23
    Jan C. Depner (PFM Software)
    06/27/15

    - Fixed PROJ4 init problem.


    Version 2.24
    Jan C. Depner (PFM Software)
    07/03/15

    - Finally straightened out the GPS time/leap second problem (I hope).


    Version 2.25
    Jan C. Depner (PFM Software)
    07/09/15

    - Now handles Riegl LAS files that don't have GeographicTypeGeoKey and ProjectedCSTypeGeoKey defined in
      the GeoKeyDirectoryTag (34735) required VLR.


    Version 2.26
    Jan C. Depner (PFM Software)
    07/22/15

    - Moved attributes.cpp and attributes.hpp to the utility library since they're used in multiple programs.
      Now I only have to modify them in one place.
    - Fixed bug that would cause input files to be loaded multiple times.  This was caused by the removal of
      the file check in pfmLoad.


    Version 2.27
    Jan C. Depner (PFM Software)
    07/31/15

    - load_file.cpp will always write the input file name to the PFM .ctl file even if no data that were read from
      the input file were placed in the PFM.  This keeps us from re-reading the same files again if we are appending
      a large batch of files to the same PFM, since pfmLoader will remove files from the input file list if they
      are already present in one or more of the target PFM files.


    Version 2.28
    Jan C. Depner (PFM Software)
    08/08/15

    - Fixed bug in readParameterFile.cpp.  It wasn't reading the default horizontal and vertical errors correctly.


    Version 2.29
    Jan C. Depner (PFM Software)
    08/10/15

    - Added waitForFinished after starting CUBE process (N/A for public domain release).


    Version 2.30
    Jan C. Depner (PFM Software)
    11/06/15

    - Modified scale factors for CZMIL interest points to match change to CZMIL API (from 10.0 to 100.0).


    Version 2.31
    Jan C. Depner (PFM Software)
    02/14/16

    - Added ability to load CZMIL CWF interest point amplitude as a attribute.  This may or may not be a good idea ;-)
    - Happy Valentine's day!


    Version 2.40
    Jan C. Depner (PFM Software)
    04/04/16

    - Fixed self-inflicted bug that caused attributes to not be loaded when appending to existing PFM.  Mea culpa.


    Version 2.41
    Jan C. Depner (PFM Software)
    04/24/16

    - Replaced CZMIL_RETURN_SELECTED_SOUNDING with CZMIL_RETURN_CLASSIFICATION_MODIFIED.  We also no longer turn off the PFM_SELECTED_SOUNDING
      flag on load since we're now using it for a LiDAR classification change flag.


    Version 2.50
    Jan C. Depner (PFM Software)
    05/02/16

    - Now allows the user to enter Well-known Text (WKT) for LAS files that have broken/missing/undecipherable coordinate system information.
    - Saves valid geographic (GEOGCS) WKT from CZMIL CPF file, LAS 1.4 file with valid geographic WKT, or valid geographic WKT for LAS file
      entered by the user.  This gets written to the bin header in the bin_header.proj_data.wkt field.
    - Saves user entered WKT for LAS files to FILE.pfm.data/FILE.pfm.wkt so that appending new LAS files to the existing PFM does not 
      require theuser to enter the WKT again.


    Version 2.51
    Jan C. Depner (PFM Software)
    05/05/16

    - Changed warning about LAZ files since pfm_unload will now actually unload them.
    - Happy Cinco de Mayo!


    Version 2.52
    Jan C. Depner (PFM Software)
    05/09/16

    - Now reads the LAS 1.4 WKT properly (I hope).


    Version 2.53
    Jan C. Depner (PFM Software)
    05/10/16

    - Fixed incorrect error checking code when opening CPF file.


    Version 2.54
    Jan C. Depner (PFM Software)
    05/12/16

    - Yet another LAS hack - some people don't put in the GeographicTypeGeoKey and just depend on the
      ProjectedCSTypeGeoKey to tell them what datum they're using.


    Version 2.55
    Jan C. Depner (PFM Software)
    07/12/16

    - Added 3 new CZMIL attributes that were added in version 3.0 of the CZMIL API.  These are CZMIL D_index,
      CZMIL D_index_cube, and CZMIL User data.
    - Removed experimental CWF interest point amplitude attribute.  D_index will work better and it comes
      right out of the CPF file so there isn't a huge amount of overhead.


    Version 2.56
    Jan C. Depner (PFM Software)
    07/21/16

    - Added ability to load CZMIL water surface (ip_rank = 0) data as reference data.


    Version 2.57
    Jan C. Depner (PFM Software)
    07/24/16

    - Fixed BAG load section to handle new BAGs with WKT.


    Version 2.58
    Jan C. Depner (PFM Software)
    10/14/16

    - Tried to get the proper WKT from LAS, CZMIL, and BAG input files.  The only one that I didn't handle is
      legacy BAG that is geodetic.
    - Removed the "Run CUBE" option.  This just added confusion and code kruft for something that can be
      easily done after the fact by running pfmCube.


    Version 2.59
    Jan C. Depner (PFM Software)
    12/21/16

    - Changed name of cube executable to use the new "cube_pfm" program.


    Version 2.60
    Jan C. Depner (PFM Software)
    04/18/17

    - Added changes to allow for using ip_rank to denote water or land processed returns in CZMIL_OPTECH_CLASS_HYBRID processed waveforms.
    - Limited CZMIL flags to land, water, hybrid, null, and water surface.
    - Happy tax day!!!


    Version 2.61
    Jan C. Depner (PFM Software)
    04/20/17

    -  Originally, water surface in CZMIL data was going to be indicated by having an ip_rank value of 0.  Unfortunately, someone,
       who shall remain nameless (but who looks exactly like Chris Macon) decided that they just had to be able to process a single
       waveform using both land and water processing modes (for areas along the edge of the water).  This caused us to have to use
       ip_rank to differentiate between land (1) and water (0) for hybrid processed returns.  So, the long and short of it is,
       ip_rank = 0 will still indicate water surface for older data but newer data (hybrid processed or not) will, from now on,
       have a classification of 41 (water surface) or 42 (derived water surface) for water surface points.  HydroFusion will assign
       the classification value.  This version of pfmLoader now checks for 41 and 42 in addition to ip_rank = 0 (for non-hybrid
       processed data) when loading water surface and when setting water surface to reference.


    Version 2.62
    Jan C. Depner (PFM Software)
    08/29/17

    - Now that the CZMIL API hard-codes ip_rank=0 points to classification 41 (where classification isn't already set), we
      no longer need to check ip_rank to determine water surface.


    Version 2.63
    Jan C. Depner (PFM Software)
    09/20/17

    - Finally, CZMIL has horizontal and vertical uncertainty.  These are now loaded instead of the dummy global values.  Of course,
      older files will load zeros but, since nobody was using it anyway, we can live with that.


    Version 2.64
    Jan C. Depner (PFM Software)
    09/21/17

    - A bunch of changes to support doing translations in the future.  There is a generic
      pfmLoader_xx.ts file that can be run through Qt's "linguist" to translate to another language.


    Version 2.65
    Jan C. Depner (PFM Software)
    04/13/18

    - Fixed percentage spinner. Broken due to translation changes.


    Version 2.70
    Jan C. Depner (PFM Software)
    06/06/18

    - Changed to support new options in pfmLoad, specifically:
      - Load land/water/shallow water based on classification ONLY (not processing mode)
      - Remove load of null CZMIL data
      - Remove flag of null CZMIL data
      - Remove flag of water surface data
      - Add "Flag shallow water data"
      - Add "Flag reprocessed data"


    Version 2.71
    Jan C. Depner (PFM Software)
    07/17/18

    - Added LAS 1.4 attributes to the LAS attributes


    Version 3.00
    Jan C. Depner (PFM Software)
    05/17/19

    - Now supports PFMv7.


    Version 3.01
    Jan C. Depner (PFM Software)
    06/29/19

    - Added support for [CZMIL HydroFusion Filter Invalid Load Flag].
    - One more shot at fixing the WKT issue when using CZMIL and GSF files.


    Version 3.10
    Jan C. Depner (PFM Software)
    07/03/19

    - Removed support for HYPACK raw and IVS XYZ.  Nobody is using them (AFAIK) and they probably don't work
      anymore.


    Version 3.11
    Jan C. Depner (PFM Software)
    07/10/19

    - Added support for using PFM_USER_06 for Hockey Puck filtered CZMIL data.
    - Hard-wired HOF or CZMIL standard user flags to always be set for HOF or CZMIL data.  CZMIL data will be
      flagged for land, water, hybrid processed, shallow water processed, reprocessed, and HP filtered.  There
      is essentially no overhead for loading the flags since the user flag space is always in the
      status/validity field.  Now that pfmLoad has been limited to a single data type, this just makes a lot
      more sense.


    Version 3.12
    Jan C. Depner (PFM Software)
    07/21/19

    - Added CZMIL Urban noise flags attribute.


    Version 3.13
    Jan C. Depner (PFM Software)
    08/18/19

    - Added support for the new CZMIL Urban noise "soft hit" flag.


    Version 3.14
    Jan C. Depner (PFM Software)
    08/20/19

    - Updated the CZMIL Urban Noise Filter flag setting to match the latest algorithm tweaks from Nick Johnson.

</pre>*/
