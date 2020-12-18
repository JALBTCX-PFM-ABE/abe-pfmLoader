
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



#ifndef __PFMLOADDEF_H__
#define __PFMLOADDEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#include <proj_api.h>
#include <ogr_spatialref.h>

#include "nvutility.h"

#include "pfm.h"
#include "pfm_extras.h"

#include "read_srtm_topo.h"
#include "dted.h"
#include "chrtr.h"
#include "chrtr2.h"
#include "binaryFeatureData.h"

#include "attributes.hpp"


typedef struct
{
  QString             name;
  float               max_depth;
  float               min_depth;
  float               precision;
  float               mbin_size;
  float               gbin_size;
  uint8_t             apply_area_filter;
  uint8_t             deep_filter_only;
  PFM_OPEN_ARGS       open_args;
  int32_t             polygon_count;
  NV_F64_COORD2       polygon[2000];
  QString             mosaic;
  QString             feature;
  float               cellstd;
  float               radius;
  int32_t             index;
  int32_t             hnd;
  int8_t              *add_map;
  uint8_t             existing;
  uint8_t             dateline;
  uint8_t             rebuild_flag;
} PFM_DEFINITION;


typedef struct
{
  int32_t            max_files;
  int32_t            max_lines;
  int32_t            max_pings;
  int32_t            max_beams;
  double             horizontal_error;
  double             vertical_error;
  uint8_t            appending;              //  Set if we are appending to one or more pre-existing files
  uint8_t            upr_flag;               //  Set if we are loading a .upr parameter file (update parameter file)
  int16_t            attribute_count;
  ATTR_BOUNDS        time_attribute;
  int16_t            time_attribute_num;
  ATTR_BOUNDS        gsf_attribute[GSF_ATTRIBUTES];
  int16_t            gsf_attribute_num[GSF_ATTRIBUTES];
  ATTR_BOUNDS        hof_attribute[HOF_ATTRIBUTES];
  int16_t            hof_attribute_num[HOF_ATTRIBUTES];
  ATTR_BOUNDS        tof_attribute[TOF_ATTRIBUTES];
  int16_t            tof_attribute_num[TOF_ATTRIBUTES];
  ATTR_BOUNDS        czmil_attribute[CZMIL_ATTRIBUTES];
  int16_t            czmil_attribute_num[CZMIL_ATTRIBUTES];
  ATTR_BOUNDS        las_attribute[LAS_ATTRIBUTES];
  int16_t            las_attribute_num[LAS_ATTRIBUTES];
  ATTR_BOUNDS        bag_attribute[BAG_ATTRIBUTES];
  int16_t            bag_attribute_num[BAG_ATTRIBUTES];
  int16_t            czmil_flag_count;
  QString            czmil_flag_name[CZMIL_FLAGS];
  int16_t            czmil_flag_num[CZMIL_FLAGS];
  QStringList        input_dirs;             //  List of directories input from the .upr parameter file with the [DIR] = tag
  QStringList        upr_input_files;        //  List of already loaded files from the .upr parameter file
  int32_t/*size_t*/  cache_mem;
} PFM_GLOBAL;


typedef struct
{
  QString             name;
  uint8_t             status;
  int32_t             type;
} FILE_DEFINITION;


typedef struct
{
  uint8_t             hof;                   //  Load HOF NULL values
  uint8_t             sub;                   //  Invert paths from Linux to Windoze
  uint8_t             lnd;                   //  Invalidate HOF land data
  uint8_t             ref;                   //  Load data as reference
  uint8_t             tof;                   //  Load TOF first returns as reference
  uint8_t             lid;                   //  Load HOF LIDAR user flags
  uint8_t             nom;                   //  Load GSF nominal depth if available
  uint8_t             sec;                   //  Invalidate all HOF secondary returns
  uint8_t             old;                   //  Load HOF LIDAR data in the old PFM_SHOALS_1K_Data form
  uint8_t             srtmb;                 //  Load best resolution SRTM data
  uint8_t             srtm1;                 //  Load 1 second resolution SRTM data (US only)
  uint8_t             srtm2;                 //  Load 1 second resolution SRTM data (world - DOD restricted)
  uint8_t             srtm3;                 //  Load 3 second resolution SRTM data
  uint8_t             srtm30;                //  Load 30 second resolution SRTM data
  uint8_t             srtmr;                 //  Load SRTM as reference
  uint8_t             srtme;                 //  Exclude SRTM2 data (1 second world)
  uint8_t             attr;                  //  Load CUBE attributes
  uint8_t             czmil;                 //  Load CZMIL user flags
  uint8_t             czmil_land;            //  Load CZMIL data classified as land (any classification from 0 to 39)
  uint8_t             czmil_water;           //  Load CZMIL data classified as water (classification 40 and 43 through 45)
  uint8_t             czmil_water_surface;   //  Load CZMIL data classified as water surface (classification 41 and 42)
  uint8_t             ref_water_surface;     //  Load water surface data as reference data
  uint8_t             czmil_chan[9];         //  Load flags for CZMIL channels
  uint8_t             HF_filt;               //  Load HydroFusion filter invalidated data.
} FLAGS;



//  The following are used to try to get the proper coordinate system for LAS files.

typedef struct
{
  char                wkt[8192];             //  Well-known Text input from CZMIL, LAS, or BAG
  char                pfm_wkt[8192];         //  Well-known Text that we use when putting data into the PFM (from LAS, CZMIL, or BAG)
  uint8_t             input_wkt;             //  Flag set if user had to input WKT
  projPJ              pj_utm;                //  Proj.4 UTM projection
  projPJ              pj_latlon;             //  Proj.4 latlon projection
  uint8_t             projected;             //  Flag set if LAS file is projected
  char                progname[512];         //  Program name
  QString             wktString[10];         //  QStrings holding recently used WKT settings
} WKT;


#include "load_file.hpp"


#endif
