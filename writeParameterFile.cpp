
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



#include "pfmLoader.hpp"

uint8_t writeParameterFile (QString parameter_file, PFM_GLOBAL pfm_global, FLAGS flags)
{
  char fname [1024];
  strcpy (fname, parameter_file.toLatin1 ());


  FILE *fp;
  if ((fp = fopen (fname, "w")) == NULL) return (NVFalse);


  //  We only use this function here to write an initial parameter file (.ipf) in the .pfm.data folder so we don;t
  //  save the PFM Definition or Input Files sections.

  fprintf (fp, "#\n#Initial Parameter File\n#\n");

  fprintf (fp, "\n************  PFM Global Options  ************\n");

  /*
  fprintf (fp, "[Maximum Input Files] = %d\n", pfm_global.max_files);
  fprintf (fp, "[Maximum Input Lines] = %d\n", pfm_global.max_lines);
  fprintf (fp, "[Maximum Input Pings] = %d\n", pfm_global.max_pings);
  fprintf (fp, "[Maximum Input Beams] = %d\n", pfm_global.max_beams);
  */

  fprintf (fp, "[Cached Memory Size] = %d\n", pfm_global.cache_mem);

  fprintf (fp, "[Load GSF Nominal Depth Flag] = %d\n", flags.nom);
  fprintf (fp, "[Insert CUBE Attributes Flag] = %d\n", flags.attr);
  fprintf (fp, "[Reference Data Flag] = %d\n", flags.ref);
  fprintf (fp, "[Invert Substitute Paths Flag] = %d\n", flags.sub);

  fprintf (fp, "[Load HOF GCS Compatible Flag] = %d\n", flags.old);
  fprintf (fp, "[HOF Load Null Flag] = %d\n", flags.hof);
  fprintf (fp, "[Invalidate HOF Land Flag] = %d\n", flags.lnd);
  fprintf (fp, "[Invalidate HOF Secondary Flag] = %d\n", flags.sec);
  fprintf (fp, "[Reference TOF First Return Flag] = %d\n", flags.tof);
  fprintf (fp, "[CZMIL Reference Water Surface Flag] = %d\n", flags.ref_water_surface);
  fprintf (fp, "[CZMIL Water Surface Load Flag] = %d\n", flags.czmil_water_surface);
  fprintf (fp, "[CZMIL Land Load Flag] = %d\n", flags.czmil_land);
  fprintf (fp, "[CZMIL Water Load Flag] = %d\n", flags.czmil_water);
  fprintf (fp, "[CZMIL HydroFusion Filter Invalid Load Flag] = %d\n", flags.HF_filt);


  fprintf (fp, "[CZMIL Shallow Channel 1 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_1]);
  fprintf (fp, "[CZMIL Shallow Channel 2 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_2]);
  fprintf (fp, "[CZMIL Shallow Channel 3 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_3]);
  fprintf (fp, "[CZMIL Shallow Channel 4 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_4]);
  fprintf (fp, "[CZMIL Shallow Channel 5 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_5]);
  fprintf (fp, "[CZMIL Shallow Channel 6 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_6]);
  fprintf (fp, "[CZMIL Shallow Channel 7 Data Flag] = %d\n", flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_7]);
  fprintf (fp, "[CZMIL IR Channel Data Flag] = %d\n", flags.czmil_chan[CZMIL_IR_CHANNEL]);
  fprintf (fp, "[CZMIL Deep Channel Data Flag] = %d\n", flags.czmil_chan[CZMIL_DEEP_CHANNEL]);

  fprintf (fp, "[Load Best SRTM Flag] = %d\n", flags.srtmb);
  fprintf (fp, "[Load 1 Second SRTM Flag] = %d\n", flags.srtm1);
  fprintf (fp, "[Load 3 Second SRTM Flag] = %d\n", flags.srtm3);
  fprintf (fp, "[Load 30 Second SRTM Flag] = %d\n", flags.srtm30);
  fprintf (fp, "[Load SRTM As Reference Flag] = %d\n", flags.srtmr);
  fprintf (fp, "[Exclude SRTM2 Data Flag] = %d\n", flags.srtme);

  fprintf (fp, "[Default Horizontal Error] = %f\n", pfm_global.horizontal_error);
  fprintf (fp, "[Default Vertical Error] = %f\n", pfm_global.vertical_error);

  fprintf (fp,   "**********  End PFM Global Options  **********\n");


  if (pfm_global.attribute_count)
    {
      fprintf (fp, "\n************  Index Attribute Types  ************\n");

      char attr[128];

      if (pfm_global.time_attribute_num)
        {
          strcpy (attr, pfm_global.time_attribute.name);
          fprintf (fp, "[%s] = %d\n", attr, pfm_global.time_attribute_num);
        }

      for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++)
        {
          if (pfm_global.gsf_attribute_num[i])
            {
              strcpy (attr, pfm_global.gsf_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.gsf_attribute_num[i]);
            }
        }

      for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++)
        {
          if (pfm_global.hof_attribute_num[i])
            {
              strcpy (attr, pfm_global.hof_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.hof_attribute_num[i]);
            }
        }

      for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++)
        {
          if (pfm_global.tof_attribute_num[i])
            {
              strcpy (attr, pfm_global.tof_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.tof_attribute_num[i]);
            }
        }

      for (int32_t i = 0 ; i < CZMIL_ATTRIBUTES ; i++)
        {
          if (pfm_global.czmil_attribute_num[i])
            {
              strcpy (attr, pfm_global.czmil_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.czmil_attribute_num[i]);
            }
        }

      for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++)
        {
          if (pfm_global.las_attribute_num[i])
            {
              strcpy (attr, pfm_global.las_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.las_attribute_num[i]);
            }
        }

      for (int32_t i = 0 ; i < BAG_ATTRIBUTES ; i++)
        {
          if (pfm_global.bag_attribute_num[i])
            {
              strcpy (attr, pfm_global.bag_attribute[i].name);
              fprintf (fp, "[%s] = %d\n", attr, pfm_global.bag_attribute_num[i]);
            }
        }

      fprintf (fp, "************  End Index Attribute Types  ************\n");
    }


  fclose (fp);


  return (NVTrue);
}
