
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



#ifndef __LOAD_FILE_H_
#define __LOAD_FILE_H_


#include "nvutility.h"
#include "globals.hpp"

#include "attributes.hpp"


/*  LLZ header files.  */

#include "llz.h"


#define MAX_LOAD_FILES    30


#define GCS_NAD83         4269
#define GCS_WGS_84        4326


//   WGS84 / UTM northern hemisphere: 326zz where zz is UTM zone number
//   WGS84 / UTM southern hemisphere: 327zz where zz is UTM zone number
//   US State Plane (NAD83): 269xx

#define PCS_NAD83_UTM_zone_3N 26903
#define PCS_NAD83_UTM_zone_23N 26923
#define PCS_WGS84_UTM_zone_1N 32601
#define PCS_WGS84_UTM_zone_60N 32660
#define PCS_WGS84_UTM_zone_1S 32701
#define PCS_WGS84_UTM_zone_60S 32760


typedef struct PFM_Load_Parameters 
{
  FLAGS                  flags;
  PFM_GLOBAL             pfm_global;
} PFM_LOAD_PARAMETERS;


uint8_t check_gsf_file (char *path, WKT *wkt_data);
uint8_t check_las_file (char *path);
uint8_t check_hof_file (char *path);
uint8_t check_tof_file (char *path);
uint8_t check_llz_file (char *path);
uint8_t check_czmil_file (char *path, uint8_t csf_attr, WKT *wkt_data);
uint8_t check_bag_file (char *path);
uint8_t check_dted_file (char *path);
uint8_t check_chrtr_file (char *path);

int32_t load_gsf_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_las_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], WKT *wkt_data);
int32_t load_hof_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_tof_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_hypack_xyz_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_llz_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_czmil_file (int32_t pfm_fc, int64_t *count, QString file, uint8_t csf_attr, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_bag_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], WKT *wkt_data);
int32_t load_ivs_xyz_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_srtm_file (int32_t pfm_fc, int64_t *count, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_dted_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);
int32_t load_chrtr_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[]);

void Do_PFM_Processing (int32_t beam, double depth, NV_F64_COORD2 nxy, uint32_t flags, double heading, uint8_t force,
                        float *l_attr, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], int32_t percent);

#endif
