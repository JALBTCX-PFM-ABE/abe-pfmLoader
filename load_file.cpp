
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

#include "load_file.hpp"
#include "LAS_WKT.hpp"
#include "newgp.h"
#include "invgp.h"

// ******************************************************************
// The variables following this are global within this module only. 
// ******************************************************************


static int16_t          file_number[MAX_PFM_FILES];
static int32_t          recnum;
static int32_t          pfm_file_count;
static int16_t          line_count[MAX_PFM_FILES];
static char             line_time[2000][18];
static int16_t          next_line[MAX_PFM_FILES];
static double           prev_heading[MAX_PFM_FILES];
static int32_t          total_out_of_limits;
static int64_t          out_count[MAX_PFM_FILES];
static float            attr[NUM_ATTR];
static char             path[1024];
static char             native_path[1024];
static char             out_str[2048];
static QString          nativePath;
static uint8_t          lidar_null;
static int64_t          gps_start_time;  //  For LAS data using GPS time instead of GPS seconds of the week.
static double           las_time_offset;  //  For LAS conversion of GPS time to UTC time.


/********************************************************************
 *
 * Function Name :Do_PFM_Processing 
 *
 * Description : Once the data to be put into the PFM file has been 
 *               processed, then this is called to do the job.
 *
 * Inputs :int32_t    BeamNo  - Sub-record number (beam number for swath sonars)
 *         double     depth   - Depth to fill grid
 *         NV_F64_COORD2 nxy  - NAVO structure with position of sounding.
 *         uint32_t   flags   - flags associated with the sounding.
 *         double     heading - heading used to test for line change (GSF only)
 *         time_t     tv_sec  - POSIX time from epoch in seconds (for line name)
 *         long       tv_nsec - POSIX nanoseconds of second (for line name)
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
void Do_PFM_Processing (int32_t beam, double depth, NV_F64_COORD2 nxy, float herr, float verr, uint32_t flags, 
                        double heading, float *l_attr, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[],
                        int32_t percent, time_t tv_sec, long tv_nsec)
{
  int32_t             j, k, status;
  DEPTH_RECORD        depth_record;
  double              diff;
  static int32_t      old_percent = -1;


  //  Loop over PFM files we have to fill. 

  for (j = 0 ; j < pfm_file_count ; j++) 
    {
      //  Dateline check.

      if (pfm_def[j].dateline && nxy.x < 0.0) nxy.x += 360.0;


      if (bin_inside_ptr (&pfm_def[j].open_args.head, nxy))
        {
          compute_index_ptr (nxy, &depth_record.coord, &pfm_def[j].open_args.head);


          depth_record.xyz.x = nxy.x;
          depth_record.xyz.y = nxy.y;
          depth_record.xyz.z = depth;

          depth_record.vertical_error = verr;
          depth_record.horizontal_error = herr;

          for (k = 0 ; k < NUM_ATTR ; k++) depth_record.attr[k] = l_attr[k];


          //  Note that we may be setting the reference flag for this data.

          depth_record.validity = flags | (load_parms[j].flags.ref * PFM_REFERENCE);

          depth_record.beam_number = beam;
          depth_record.ping_number = recnum;


          //  A change of over 120 degrees in heading from the last saved heading is treated as a line change.  This only works
          //  for GSF data.

          diff = fabs (heading - prev_heading[j]);
          if (line_count[j] < 0 || ((diff > 120.0) && (diff < 345.0)))
            {
              line_count[j]++;

              int32_t year, jday, hour, minute;
              float second;
              cvtime (tv_sec, tv_nsec, &year, &jday, &hour, &minute, &second);


              //  DO NOT MODIFY THIS FORMAT!!!  It is being used in the PFM API to parse time.

              sprintf (line_time[line_count[j]], "%d-%03d-%02d:%02d:%02d", year + 1900, jday, hour, minute, NINT (second));

              prev_heading[j] = heading;
            }


          depth_record.line_number = line_count[j] + next_line[j];
          depth_record.file_number = file_number[j];


          //  If the depth is outside of the min and max depths, set it to the null depth (defined in the PFM API as
          //  max_depth + 1.0) and set the PFM_FILTER_INVAL and the PFM_MODIFIED bit that is used by the PFM API to signify that 
          //  the data must be saved to the input file after editing.  Note that these points will not be visible in the PFM editor.

          if (depth_record.xyz.z > pfm_def[j].max_depth || depth_record.xyz.z <= pfm_def[j].min_depth)
            {
              depth_record.xyz.z = pfm_def[j].max_depth + 1.0;


              //  Caveat - If we're loading HOF LIDAR null data and this is a null point we don't want to set this to invalid or modified 
              //  so that GCS can do reprocessing.

              if (!lidar_null) depth_record.validity |= (PFM_FILTER_INVAL | PFM_MODIFIED);

              total_out_of_limits++;
            }


          //  Load the point.

          if ((status = add_cached_depth_record (pfm_def[j].hnd, &depth_record)))
            //if ((status = add_depth_record_index (pfm_def[j].hnd, &depth_record)))
            {
              if (status == WRITE_BIN_RECORD_DATA_READ_ERROR)
                {
                  fprintf (stderr, "%s\n", pfm_error_str (status));
                  fprintf (stderr, "%d %d %f %f %f %f\n\n", depth_record.coord.y, depth_record.coord.x,
                           pfm_def[j].open_args.head.mbr.min_y, pfm_def[j].open_args.head.mbr.max_y,
                           pfm_def[j].open_args.head.mbr.min_x, pfm_def[j].open_args.head.mbr.max_x);
                  fflush (stderr);
                }
              else
                {
                  pfm_error_exit (status);
                }
            }


          //  Set the add_map value to show that we have new data in this bin.

          pfm_def[j].add_map[depth_record.coord.y * pfm_def[j].open_args.head.bin_width + depth_record.coord.x] = 1;


          out_count[j]++;
        }
    }

  if (old_percent != percent)
    {
      strcpy (out_str, pfmLoader::tr ("%1% processed        \r").arg (percent, 3, 10, QChar ('0')).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);

      old_percent = percent;
    }
}



/********************************************************************
 *
 * Function Name :  GSF_PFM_Processing
 *
 * Description :    This is called when GSF records are to be
 *               output to the PFM file. All GSF specific massaging is done
 *               and then passed to Do_PFM_Processing.
 *
 * Inputs : gsf_records - gsf data to be processed.
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
static void GSF_PFM_Processing (gsfRecords * gsf_records, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], int32_t percent)
{
  int32_t             i;
  uint32_t            flags;
  double              lateral, ang1, ang2, lat, lon;
  float               dep;
  NV_F64_COORD2       xy2;
  float               herr, verr;
  NV_F64_COORD2       nxy;


  void gsf_to_pfm_flags (uint32_t *pfm_flags, uint8_t gsf_flags);


  //  Set and then increment the record number.  

  lat = gsf_records->mb_ping.latitude;
  lon = gsf_records->mb_ping.longitude;
  ang1 = gsf_records->mb_ping.heading + 90.0;
  ang2 = gsf_records->mb_ping.heading;


  //  Make sure position and ping is valid.   

  if ((lat <= 90.0) && (lon <= 180.0) && !(gsf_records->mb_ping.ping_flags & GSF_IGNORE_PING))
    {
      //  Check for 1 beam multibeam.

      if (gsf_records->mb_ping.number_beams == 1)
        {
          herr = load_parms[0].pfm_global.horizontal_error;
          verr = load_parms[0].pfm_global.vertical_error;

          gsf_to_pfm_flags (&flags, gsf_records->mb_ping.beam_flags[0]);


          nxy.x = lon;
          nxy.y = lat;


          if (load_parms[0].flags.nom && gsf_records->mb_ping.nominal_depth != NULL)
            {
              dep = gsf_records->mb_ping.nominal_depth[0];
            }
          else
            {
              dep = gsf_records->mb_ping.depth[0];
            }


          if (load_parms[0].pfm_global.attribute_count) getGSFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                          load_parms[0].pfm_global.gsf_attribute_num, &gsf_records->mb_ping, 0);

          Do_PFM_Processing (1, dep, nxy, herr, verr, flags, ang2, attr, load_parms, pfm_def, percent, gsf_records->mb_ping.ping_time.tv_sec, 
                             gsf_records->mb_ping.ping_time.tv_nsec);
        }
      else
        {
           for (i = 0 ; i < gsf_records->mb_ping.number_beams ; i++)
            {
              herr = 0.0;
              verr = 0.0;

              if (load_parms[0].flags.nom && gsf_records->mb_ping.nominal_depth != NULL)
                {
                  dep = gsf_records->mb_ping.nominal_depth[i];
                }
              else
                {
                  dep = gsf_records->mb_ping.depth[i];
                }

              if (dep != 0.0 && gsf_records->mb_ping.beam_flags != NULL && !(check_flag (gsf_records->mb_ping.beam_flags[i], NV_GSF_IGNORE_NULL_BEAM))) 
                {
                  //  Adjust for cross track position.  

                  lateral = gsf_records->mb_ping.across_track[i];
                  newgp (lat, lon, ang1, lateral, &nxy.y, &nxy.x);


                  //  if the along track array is present then use it 
                
                  if (gsf_records->mb_ping.along_track != (double *) NULL) 
                    {
                      xy2.y = nxy.y;
                      xy2.x = nxy.x;
                      lateral = gsf_records->mb_ping.along_track[i];

                      newgp (xy2.y, xy2.x, ang2, lateral, &nxy.y, &nxy.x);
                    }


                  //  If the horizontal and vertical errors arrays are present then use them 

                  if (gsf_records->mb_ping.horizontal_error != (double *) NULL) 
                    {
                      herr = (float) gsf_records->mb_ping.horizontal_error[i];
                    }
                  else
                    {
                      herr = load_parms[0].pfm_global.horizontal_error;
                    }

                  if (gsf_records->mb_ping.vertical_error != (double *) NULL) 
                    {
                      verr = (float) gsf_records->mb_ping.vertical_error[i];
                    }
                  else
                    {
                      verr = load_parms[0].pfm_global.vertical_error;
                    }


                  gsf_to_pfm_flags (&flags, gsf_records->mb_ping.beam_flags[i]);


                  if (load_parms[0].pfm_global.attribute_count) getGSFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                                  load_parms[0].pfm_global.gsf_attribute_num, &gsf_records->mb_ping, i);

                  Do_PFM_Processing (i + 1, dep, nxy, herr, verr, flags, ang2, attr, load_parms, pfm_def, percent, gsf_records->mb_ping.ping_time.tv_sec, 
                                     gsf_records->mb_ping.ping_time.tv_nsec);
                }
            }
        }
    }
}


uint8_t check_gsf_file (char *path, WKT *wkt_data)
{
  int32_t           gsf_handle;
  extern int        gsfError;


  //  Open the GSF file.

  if (gsfOpen (path, GSF_READONLY_INDEX, &gsf_handle)) 
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      gsfPrintError (stderr);
      fflush (stderr);
      if (gsfError != GSF_FOPEN_ERROR) gsfClose (gsf_handle);
      return (NVFalse);
    }
  gsfClose (gsf_handle);


  //  AFAIK GSF files are always WGS84/WGS84e.  If not, somebody else can fix it.  I'm tired of messing with it.

  if (strlen (wkt_data->pfm_wkt) < 10)
    {
      strcpy (wkt_data->pfm_wkt, "GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]],VERT_CS[\"ellipsoid Z in meters\",VERT_DATUM[\"Ellipsoid\",2002],UNIT[\"metre\",1],AXIS[\"Z\",UP]]");
    }


  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_gsf_file
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_gsf_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  gsfDataID         gsf_data_id;
  gsfRecords        gsf_records;
  int32_t           i, j, gsf_handle, percent, records;
  time_t            sec;
  long              nsec;
  extern int        gsfError;
  char              temp[1024], line[1024], *ptr;
  uint8_t           nom_warning;


  nom_warning = NVFalse;


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;


  //  Open the GSF file indexed so we can get the number of records.

  if (gsfOpen (path, GSF_READONLY_INDEX, &gsf_handle)) 
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      gsfPrintError (stderr);
      fflush (stderr);
      if (gsfError != GSF_FOPEN_ERROR) gsfClose (gsf_handle);
      return (0);
    }


  //  Get the number of records in the file.

  records = gsfIndexTime (gsf_handle, GSF_RECORD_SWATH_BATHYMETRY_PING, -1, &sec, &nsec);


  //  Get the next available line number for this file so we can generate our own lines from GSF data.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
    }


  memset (&gsf_records, 0, sizeof (gsfRecords));
  memset (&gsf_data_id, 0, sizeof (gsfDataID));


  //  Read all of the data from this file.  

  for (i = 0 ; i < records ; i++)
    {
      gsf_data_id.recordID = GSF_RECORD_SWATH_BATHYMETRY_PING;
      gsf_data_id.record_number = i + 1;
      if (gsfRead (gsf_handle, GSF_RECORD_SWATH_BATHYMETRY_PING, &gsf_data_id, &gsf_records, NULL, 0) < 0) 
        {
          if (gsfError == GSF_READ_TO_END_OF_FILE) break;
          strcpy (out_str, pfmLoader::tr ("Error reading record - %1\nRecord %2 skipped\n").arg (gsfStringError ()).arg (recnum).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }
      else
        {
          //  NOTE: GSF records start at 1 not 0.

          recnum = i + 1;

          percent = gsfPercent (gsf_handle);


          //  Check for nominal depth present.

          if (!nom_warning && load_parms[0].flags.nom && gsf_records.mb_ping.nominal_depth == NULL)
            {
              strcpy (out_str, pfmLoader::tr ("\nFor file %1\nnominal depth requested, true depth used.\n").arg (path).toUtf8 ());
              fprintf (stderr, "%s", out_str);
              fflush (stderr);
              nom_warning = NVTrue;
            }


          GSF_PFM_Processing (&gsf_records, load_parms, pfm_def, percent);
        }
    }


  gsfClose (gsf_handle);


  //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
  //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file list for an append
  //  operation.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      //  May be MSYS on Windoze so we have to check for either separator.

      if (strrchr (path, '/'))
        {
          ptr = strrchr (path, '/');
        }
      else if (strrchr (path, '\\'))
        {
          ptr = strrchr (path, '\\');
        }
      else
        {
          ptr = NULL;
        }


      if (ptr == NULL) 
        {
          strcpy (temp, path);
        }
      else
        {
          strcpy (temp, (ptr + 1));
        }

      for (j = 0 ; j <= line_count[i] ; j++)
        {
          //  NEVER MODIFY THIS FORMAT!!!  It is used by the PFM API to parse time.

          sprintf (line, "%s-%s", temp, line_time[j]);
          write_line_file (pfm_def[i].hnd, line);
        }


      //  Convert to native separators so that we get nice file names on Windows.

      nativePath = QDir::toNativeSeparators (QString (path));
      strcpy (native_path, nativePath.toLatin1 ());

      write_list_file (pfm_def[i].hnd, native_path, PFM_GSF_DATA);
      count[i] += out_count[i];
    }

  return (total_out_of_limits);
}



uint8_t check_hof_file (char *path)
{
  FILE               *hof_fp;
  HOF_HEADER_T       head;


  //  Open the CHARTS .hof file.

  if ((hof_fp = open_hof_file (path)) != NULL)
    {
      hof_read_header (hof_fp, &head);

      if (!strstr (head.text.file_type, "Hydrographic Output File"))
        {
          strcpy (out_str, pfmLoader::tr ("\n\nFile %1 is not a .hof file.\n").arg (path).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          fclose (hof_fp);
          return (NVFalse);
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the CHARTS .hof file.\n%1 : %2\n").arg (path).arg (strerror (errno)).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }


  fclose (hof_fp);

  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_hof_file
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_hof_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam;
  FILE               *hof_fp;
  HYDRO_OUTPUT_T     hof;
  HOF_HEADER_T       head;
  int32_t            i, percent;
  NV_F64_COORD2      xy;
  float              herr, verr;
  double             depth;
  uint32_t           flags;
  float              eof;
  char               line[256];
  int32_t            data_type = PFM_CHARTS_HOF_DATA;


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Get the next available file number so that we can put it into the index file.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;


  //  HOF records start at 1.

  recnum = 1;


  //  Open the HOF files and read the data.

  if ((hof_fp = open_hof_file (path)) != NULL)
    {
      fseek (hof_fp, 0, SEEK_END);
      eof = ftell (hof_fp);
      fseek (hof_fp, 0, SEEK_SET);


      hof_read_header (hof_fp, &head);


      //  Read all of the data from this file.  

      while (hof_read_record (hof_fp, HOF_NEXT_RECORD, &hof))
        {
          //  Get the start time for the line name.

          if (recnum == 1)
            {
              int32_t year, jday, hour, minute;
              float second;
              time_t tv_sec = hof.timestamp / 1000000;
              long tv_nsec = (hof.timestamp % 1000000) * 1000;
              cvtime (tv_sec, tv_nsec, &year, &jday, &hour, &minute, &second);


              //  NEVER MODIFY THIS FORMAT!!!  It is used by the PFM API to parse time.

              sprintf (line, "Line %s %d-%03d-%02d:%02d:%02d", head.text.flightline_number, year + 1900, jday, hour, minute, NINT (second));
            }


          percent = (int32_t) roundf (((float) (ftell (hof_fp)) / eof) * 100.0);


          //  Check to see if we want to load HOF data in the old PFM_SHOALS_1K_DATA form.

          if (load_parms[0].flags.old)
            {
              data_type = PFM_SHOALS_1K_DATA;

              beam = 0;

              xy.y = hof.latitude;
              xy.x = hof.longitude;


              lidar_null = NVFalse;


              //  We'll allow 0 ABDC if we're loading NULL points.

              if (hof.abdc || load_parms[0].flags.hof)
                {
                  //  HOF uses the lower three bits of the status field for status thusly :

                  //  bit 0 = deleted    (1) 
                  //  bit 1 = kept       (2) 
                  //  bit 2 = swapped    (4)


                  flags = 0;
                  if (hof.status & AU_STATUS_DELETED_BIT) flags = PFM_MANUALLY_INVAL;


                  //  If the absolute value of the abdc is below 70 we mark it as filter invalid.  If the value is
                  //  below 70 (like a -93) but the absolute value is above 70 we will mark it as manually invalid.

                  if (hof.abdc < 70)
                    {
                      if (abs (hof.abdc) < 70)
                        {
                          flags = PFM_FILTER_INVAL;
                        }
                      else
                        {
                          flags = PFM_MANUALLY_INVAL;
                        }
                    }


                  //  Invalidate HOF land flags.

                  if (load_parms[0].flags.lnd && hof.abdc == 70) flags = (PFM_FILTER_INVAL | PFM_MODIFIED);


                  if (hof.suspect_status & SUSPECT_STATUS_SUSPECT_BIT) flags |= PFM_SUSPECT;
                  if (hof.suspect_status & SUSPECT_STATUS_FEATURE_BIT) flags |= PFM_SELECTED_FEATURE;


                  //  Maybe in the future ;-)

                  //  HOF classification status :
                  //  bit 0 = classified        (1=classified; 0=not classified)
                  //  bit 1 = non-bare_earth1   (0x02);   
                  //  bit 2 = non-bare_earth2   (0x04);
                  //  bit 3 = water             (0x08);   

                  //  if ((hof.classification_status & 0x01) && (!(hof.classification_status & 0x0e))) flags |= PFM_USER_01;


                  if (load_parms[0].flags.lid)
                    {
                      //  Flag "shallow water processed" or "shoreline depth swapped" HOF data (these two are mututally exclusive in practice).

                      if (abs (hof.abdc) == 74 || abs (hof.abdc) == 72) flags |= PFM_USER_01;


                      //  Flag "APD" data

                      if (hof.bot_channel) flags |= PFM_USER_02;


                      //  Flag "land" data

                      if (abs (hof.abdc) == 70) flags |= PFM_USER_03;
                    }


                  if (hof.correct_depth <= -998.0)
                    {
                      if (load_parms[0].flags.hof)
                        {
                          //  We're setting the null (-998.0) depths to -999999.0 to put them outside any sane bounds.
                          //  This will cause them to be set to the null depth for whatever PFM they fall in.  We
                          //  want to keep them so that the Optech GCS software can reprocess them if possible.

                          lidar_null = NVTrue;

                          if (load_parms[0].pfm_global.attribute_count) getHOFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                                          load_parms[0].pfm_global.hof_attribute_num, &hof);

                          depth = -999999.0;

                          Do_PFM_Processing (beam, depth, xy, 0.0, 0.0, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                        }
                    }
                  else
                    {
                      hof_get_uncertainty (&hof, &herr, &verr, hof.correct_depth, hof.abdc);

                      if (load_parms[0].pfm_global.attribute_count) getHOFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                                      load_parms[0].pfm_global.hof_attribute_num, &hof);


                      if (load_parms[0].flags.lid && hof.correct_sec_depth > -998.0) flags |= PFM_USER_04;


                      depth = -hof.correct_depth;

                      Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                    }
                }
            }
          else
            {
              data_type = PFM_CHARTS_HOF_DATA;

              lidar_null = NVFalse;


              if (hof.abdc)
                {
                  //  HOF uses the lower three bits of the status field for status thusly :
                  //
                  //  bit 0 = deleted    (1) 
                  //  bit 1 = kept       (2) 
                  //  bit 2 = swapped    (4)


                  flags = 0;
                  if (hof.status & AU_STATUS_DELETED_BIT) flags = PFM_MANUALLY_INVAL;


                  //  If the absolute value of the abdc is below 70 we mark it as filter invalid.  If the value is
                  //  below 70 (like a -93) but the absolute value is above 70 we will mark it as manually invalid.

                  if (hof.abdc < 70)
                    {
                      if (abs (hof.abdc) < 70)
                        {
                          flags = PFM_FILTER_INVAL;
                        }
                      else
                        {
                          flags = PFM_MANUALLY_INVAL;
                        }
                    }


                  //  Invalidate HOF land flags.

                  if (load_parms[0].flags.lnd && hof.abdc == 70) flags = (PFM_FILTER_INVAL | PFM_MODIFIED);


                  if (hof.suspect_status & SUSPECT_STATUS_SUSPECT_BIT) flags |= PFM_SUSPECT;
                  if (hof.suspect_status & SUSPECT_STATUS_FEATURE_BIT) flags |= PFM_SELECTED_FEATURE;


                  if (load_parms[0].flags.lid)
                    {
                      //  Flag "shallow water processed" or "shoreline depth swapped" HOF data (these two are mututally exclusive in practice).

                      if (abs (hof.abdc) == 74 || abs (hof.abdc) == 72) flags |= PFM_USER_01;


                      //  Flag "APD" data

                      if (hof.bot_channel) flags |= PFM_USER_02;


                      //  Flag "land" data

                      if (abs (hof.abdc) == 70) flags |= PFM_USER_03;
                    }


                  //  The same attributes will be loaded for primary and secondary returns.

                  if (load_parms[0].pfm_global.attribute_count) getHOFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                                  load_parms[0].pfm_global.hof_attribute_num, &hof);


                  beam = 0;
                  xy.y = hof.latitude;
                  xy.x = hof.longitude;


                  if (hof.correct_depth <= -998.0)
                    {
                      if (load_parms[0].flags.hof)
                        {
                          //  We're setting the null (-998.0) depths to -999999.0 to put them outside any sane bounds.  This will cause
                          //  them to be set to the null depth for whatever PFM they fall in.  We want to keep them just to see coverage.

                          lidar_null = NVTrue;

                          depth = -999999.0;

                          Do_PFM_Processing (beam, depth, xy, 0.0, 0.0, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                        }
                    }
                  else
                    {
                      hof_get_uncertainty (&hof, &herr, &verr, hof.correct_depth, hof.abdc);

                      if (load_parms[0].flags.lid && hof.correct_sec_depth > -998.0) flags |= PFM_USER_04;

                      depth = -hof.correct_depth;

                      Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                    }


                  if (hof.correct_sec_depth > -998.0)
                    {
                      flags = 0;


                      //  If the absolute value of the abdc is below 70 we mark it as filter invalid.  If the value is
                      //  below 70 (like a -93) but the absolute value is above 70 we will mark it as manually invalid.

                      if (hof.sec_abdc < 70)
                        {
                          if (abs (hof.sec_abdc) < 70)
                            {
                              flags = PFM_FILTER_INVAL;
                            }
                          else
                            {
                              flags = PFM_MANUALLY_INVAL;
                            }
                        }


                      //  Invalidate HOF land flags.

                      if (load_parms[0].flags.lnd && hof.sec_abdc == 70) flags = (PFM_FILTER_INVAL | PFM_MODIFIED);


                      //  Invalidate HOF secondary returns.

                      if (load_parms[0].flags.sec) flags = (PFM_FILTER_INVAL | PFM_MODIFIED);


                      if (load_parms[0].flags.lid)
                        {
                          //  Flag "shallow water processed" or "shoreline depth swapped" HOF data (these two are mututally exclusive in practice).

                          if (abs (hof.sec_abdc) == 74 || abs (hof.sec_abdc) == 72) flags |= PFM_USER_01;


                          //  Flag "APD" data

                          if (hof.sec_bot_chan) flags |= PFM_USER_02;


                          //  Flag "land" data

                          if (abs (hof.sec_abdc) == 70) flags |= PFM_USER_03;
                        }


                      hof_get_uncertainty (&hof, &herr, &verr, hof.correct_sec_depth, hof.sec_abdc);

                      if (load_parms[0].flags.lid) flags |= PFM_USER_04;


                      beam = 1;
                      xy.y = hof.sec_latitude;
                      xy.x = hof.sec_longitude;
                      depth = -hof.correct_sec_depth;

                      Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                    }
                }
            }
          recnum++;
        }


      fclose (hof_fp);


      //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
      //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
      //  operation.

      for (i = 0 ; i < pfm_file_count ; i++) 
        {
          write_line_file (pfm_def[i].hnd, line);


          //  Convert to native separators so that we get nice file names on Windows.

          nativePath = QDir::toNativeSeparators (QString (path));
          strcpy (native_path, nativePath.toLatin1 ());

          write_list_file (pfm_def[i].hnd, native_path, data_type);
          count[i] += out_count[i];
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }


  //  We don't return out of range values for HOF data since we load the -998.0 (NBR) values as null depths.

  total_out_of_limits = 0;
  return (total_out_of_limits);
}



uint8_t check_tof_file (char *path)
{
  FILE               *tof_fp;
  TOF_HEADER_T       head;


  //  Open the CHARTS .tof file.

  if ((tof_fp = open_tof_file (path)) != NULL)
    {
      tof_read_header (tof_fp, &head);

      if (!strstr (head.text.file_type, "Topographic Output File"))
        {
          strcpy (out_str, pfmLoader::tr ("\n\nFile %1 is not a .tof file.\n").arg (path).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          fclose (tof_fp);
          return (NVFalse);
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the CHARTS .tof file.\n%1 : %2").arg (path).arg (strerror (errno)).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }


  fclose (tof_fp);

  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_tof_file
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_tof_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam;
  FILE               *tof_fp;
  TOPO_OUTPUT_T      tof;
  TOF_HEADER_T       head;
  int32_t            i, percent;
  NV_F64_COORD2      xy;
  float              herr, verr;
  double             depth;
  uint32_t           flags;
  float              eof;
  char               line[256];


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;


  //  TOF records now start at 1 as per IVS so we'll be compatible with Optech GCS and Fledermaus.

  recnum = 1;


  //  Open the TOF files and read the data.

  if ((tof_fp = open_tof_file (path)) != NULL)
    {
      fseek (tof_fp, 0, SEEK_END);
      eof = ftell (tof_fp);
      fseek (tof_fp, 0, SEEK_SET);


      tof_read_header (tof_fp, &head);


      //  Check for files older than 10/07/2011.  At that point we changed the way we load TOF data into PFM
      //  so that we load first returns even if the second return is bad.

      uint8_t ver_dep_flag = NVFalse;
      int64_t hd_start_time;
      sscanf (head.text.start_time, "%"PRId64, &hd_start_time);
      if (hd_start_time < 1317945600000000LL) ver_dep_flag = NVTrue;


      //  Read all of the data from this file.  

      while (tof_read_record (tof_fp, TOF_NEXT_RECORD, &tof))
        {
          //  Get the start time for the line name.

          if (recnum == 1)
            {
              int32_t year, jday, hour, minute;
              float second;
              time_t tv_sec = tof.timestamp / 1000000;
              long tv_nsec = (tof.timestamp % 1000000) * 1000;
              cvtime (tv_sec, tv_nsec, &year, &jday, &hour, &minute, &second);


              //  DO NOT MODIFY THIS FORMAT!!!  It is being used in the PFM API to parse time.

              sprintf (line, "Line %s %d-%03d-%02d:%02d:%02d", head.text.flightline_number, year + 1900, jday, hour, minute, NINT (second));
            }


          herr = load_parms[0].pfm_global.horizontal_error;
          verr = load_parms[0].pfm_global.vertical_error;


          percent = (int32_t) roundf (((float) (ftell (tof_fp)) / eof) * 100.0);


          if (load_parms[0].pfm_global.attribute_count) getTOFAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                          load_parms[0].pfm_global.tof_attribute_num, &tof);


          //  If the difference between the first and last returns is less than 5 cm we will not load the first 
          //  return and will give it the last return validity on unload.

          if (tof.elevation_first > -998.0 && fabs ((double) (tof.elevation_last - tof.elevation_first)) > 0.05)
            {
              //  We don't load first returns if the last return was bad in files prior to 10/07/2011.

              if (ver_dep_flag && tof.elevation_last > -998.0)
                {
                  beam = 0;

                  flags = 0;
                  if (load_parms[0].flags.tof) flags = PFM_REFERENCE;

                  if (tof.conf_first < 50) flags = PFM_FILTER_INVAL;

                  xy.y = tof.latitude_first;
                  xy.x = tof.longitude_first;

                  depth = -tof.elevation_first;

                  Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                }
            }


          if (tof.elevation_last > -998.0) 
            {
              //  TOF uses the lower two bits of the status field for status thusly :
              //
              //  bit 0 = first deleted    (1) 
              //  bit 1 = second deleted   (2)

              beam = 1;

              flags = 0;

              if (tof.conf_last < 50) flags = PFM_FILTER_INVAL;


              xy.y = tof.latitude_last;
              xy.x = tof.longitude_last;

              depth = -tof.elevation_last;

              Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
            }
          recnum++;
        }


      fclose (tof_fp);


      //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
      //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
      //  operation.

      for (i = 0 ; i < pfm_file_count ; i++) 
        {
          write_line_file (pfm_def[i].hnd, line);


          //  Convert to native separators so that we get nice file names on Windows.

          nativePath = QDir::toNativeSeparators (QString (path));
          strcpy (native_path, nativePath.toLatin1 ());

          write_list_file (pfm_def[i].hnd, native_path, PFM_SHOALS_TOF_DATA);
          count[i] += out_count[i];
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }

  return (total_out_of_limits);
}



uint8_t check_las_file (char *path)
{
  time_t                  gps_tv_sec;
  long                    gps_tv_nsec;


  //  Set the GPS start time (00:00:00 on 6 January 1980) in POSIX form.

  inv_cvtime (80, 6, 0, 0, 0.0, &gps_tv_sec, &gps_tv_nsec);
  gps_start_time = (int64_t) gps_tv_sec;


  //  Open the LAS file.

  LASreadOpener lasreadopener;
  LASreader *lasreader;

  lasreadopener.set_file_name (path);
  lasreader = lasreadopener.open ();
  if (!lasreader)
    {
      strcpy (out_str, pfmLoader::tr ("\n\n*** ERROR ***\nUnable to open LAS file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }


  lasreader->close ();
  delete lasreader;


  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_las_file
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_las_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], WKT *wkt_data)
{
  int32_t                     beam, i, percent;
  uint64_t                    numrecs;
  NV_F64_COORD2               xy;
  float                       herr, verr;
  double                      depth;
  uint32_t                    flags;
  float                       eof;
  char                        line[256], *ptr;
  uint8_t                     las_v14, extended_flag;


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;


  //  We're starting with record 1 because I'm assuming (you know what they say about that) that's what QPS (nee IVS) Fledermaus used.

  recnum = 1;


  //  Open the LAS file and read the header.

  LASreadOpener lasreadopener;
  LASreader *lasreader;
  LASheader *lasheader;

  lasreadopener.set_file_name (path);
  lasreader = lasreadopener.open ();
  if (!lasreader)
    {
      strcpy (out_str, pfmLoader::tr ("\n\n*** ERROR ***\nUnable to open LAS file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }


  lasheader = &lasreader->header;


  //  Check to see if we're using the extended formats in v1.4 (point data format above 5).

  las_v14 = NVFalse;
  extended_flag = NVFalse;
  if (lasheader->version_minor == 4) las_v14 = NVTrue;
  if (lasheader->point_data_format > 5) extended_flag = NVTrue;


  if (las_v14)
    {
      numrecs = lasheader->extended_number_of_point_records;
    }
  else
    {
      numrecs = lasheader->number_of_point_records;
    }

  eof = (float) numrecs;


  //  Check to see if we can figure out what the hell kind of coordinate system the LAS file is using.
  //  If the user input WKT for a previous file we're going to use that for all subsequent files.

  if (!wkt_data->input_wkt) LAS_WKT (0, wkt_data, lasheader, path);


  //  Now that we know that the file is kosher, read all of the data from this file.  

  for (uint64_t i = 0 ; i < numrecs ; i++)
    {
      if (!lasreader->read_point ())
        {
          strcpy (out_str, pfmLoader::tr ("\n\n*** ERROR ***\nReading point record %1 in %2\n").arg (i).arg (path).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
	  lasreader->close ();
	  delete lasreader;
          return (0);
        }

      percent = NINT (((float) i / eof) * 100.0);


      //  If Global Encoding (reserved) is set, time is GPS time (as opposed to GPS seconds of the week).

      float posix_minutes = 0.0;

      if (lasheader->global_encoding & 0x01)
        {
	  //  Note, GPS time is ahead of UTC time by some number of leap seconds depending on the date of the survey.
	  //  The leap seconds that are relevant for CHARTS and/or CZMIL data are as follows
	  //
	  //  December 31 2005 23:59:59 - 1136073599 - 14 seconds ahead
	  //  December 31 2008 23:59:59 - 1230767999 - 15 seconds ahead
	  //  June 30 2012 23:59:59     - 1341100799 - 16 seconds ahead
	  //  June 30 2015 23:59:59     - 1435708799 - 17 seconds ahead

	  las_time_offset = 1000000000.0 - 13.0;

	  int64_t tv_sec = lasreader->point.get_gps_time () + gps_start_time + las_time_offset;
	  if (tv_sec > 1136073599) las_time_offset -= 1.0;
	  if (tv_sec > 1230767999) las_time_offset -= 1.0;
	  if (tv_sec > 1341100799) las_time_offset -= 1.0;
	  if (tv_sec > 1435708799) las_time_offset -= 1.0;

          posix_minutes = (float) ((lasreader->point.get_gps_time () + (double) gps_start_time + las_time_offset) / 60.0);
        }
      else
        {
          posix_minutes = ABE_NULL_TIME;
        }

      if (wkt_data->projected)
        {
          double ux, uy;

          ux = lasreader->point.get_x ();
          uy = lasreader->point.get_y ();
          pj_transform (wkt_data->pj_utm, wkt_data->pj_latlon, 1, 1, &ux, &uy, NULL);
          ux *= NV_RAD_TO_DEG;
          uy *= NV_RAD_TO_DEG;

          xy.x = ux;
          xy.y = uy;
        }
      else
        {
          xy.x = lasreader->point.get_x ();
          xy.y = lasreader->point.get_y ();
        }

      beam = 0;
      herr = load_parms[0].pfm_global.horizontal_error;
      verr = load_parms[0].pfm_global.vertical_error;

      if (load_parms[0].pfm_global.attribute_count) getLASAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                      load_parms[0].pfm_global.las_attribute_num, &posix_minutes,
                                                                      &lasreader->point, extended_flag);

      lidar_null = NVFalse;
      flags = 0;


      //  Check for the "withheld" bit and set to invalid if set.

      if (lasreader->point.get_withheld_flag ()) flags = PFM_MANUALLY_INVAL;


      //  Check for the "key point" bit and set to selected feature.

      if (lasreader->point.get_keypoint_flag ()) flags = PFM_SELECTED_FEATURE;


      //  Check for the "synthetic" bit and set to reference.

      if (lasreader->point.get_synthetic_flag ()) flags = PFM_REFERENCE;


      depth = -lasreader->point.get_z ();


      Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);

      recnum++;
    }


  lasreader->close ();
  delete lasreader;


  //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
  //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
  //  operation.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      //  May be MSYS on Windoze so we have to check for either separator.

      if (strrchr (path, '/'))
        {
          ptr = strrchr (path, '/');
        }
      else if (strrchr (path, '\\'))
        {
          ptr = strrchr (path, '\\');
        }
      else
        {
          ptr = NULL;
        }


      if (ptr == NULL) 
        {
          strcpy (line, path);
        }
      else
        {
          strcpy (line, (ptr + 1));
        }

      write_line_file (pfm_def[i].hnd, line);


      //  Convert to native separators so that we get nice file names on Windows.

      nativePath = QDir::toNativeSeparators (QString (path));
      strcpy (native_path, nativePath.toLatin1 ());

      write_list_file (pfm_def[i].hnd, native_path, PFM_LAS_DATA);
      count[i] += out_count[i];
    }


  return (total_out_of_limits);
}



uint8_t check_llz_file (char *path)
{
  int32_t llz_hnd;
  LLZ_HEADER llz_header;


  //  Open the llz file.

  if ((llz_hnd = open_llz (path, &llz_header)) < 0)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the LLZ file.\n%1 : %2\n").arg (path).arg (strerror (errno)).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }

  if (!strstr (llz_header.version, "llz library"))
    {
      strcpy (out_str, pfmLoader::tr ("\n\nLLZ file version corrupt or %1 is not an LLZ file.\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }      

  close_llz (llz_hnd);

  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_llz_file
 *
 * Description : Reads a NAVO LLZ file.  See llz.h for format.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_llz_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam, i, percent, llz_hnd;
  uint32_t           flags;
  LLZ_HEADER         llz_header;
  LLZ_REC            llz;
  NV_F64_COORD2      xy;
  float              herr, verr;
  char               line[256], *ptr;
  double             depth;


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;
  recnum = 0;


  //  Open the LLZ file and read the data.

  if ((llz_hnd = open_llz (path, &llz_header)) >= 0)
    {
      //  Read all of the data from this file.  

      for (i = 0 ; i < llz_header.number_of_records ; i++)
        {
          herr = load_parms[0].pfm_global.horizontal_error;
          verr = load_parms[0].pfm_global.vertical_error;


          percent = (int32_t) (((float) i / (float) llz_header.number_of_records) * 100.0);

          read_llz (llz_hnd, i, &llz);
          
          xy.y = llz.xy.lat;
          xy.x = llz.xy.lon;
          depth = llz.depth;
          flags = llz.status;

          beam = 0;

          recnum = i;

          if (load_parms[0].pfm_global.attribute_count)
            {
              int32_t index = load_parms[0].pfm_global.time_attribute_num - 1;
              if (load_parms[0].pfm_global.time_attribute_num)
                {
                  //  Special case!  LLZ files may not have time so we load the null time value.

                  if (llz.tv_sec == 0 && llz.tv_nsec == 0)
                    {
                      attr[index] = ABE_NULL_TIME;
                    }
                  else
                    {
                      attr[index] = (float) (llz.tv_sec / 60);
                    }
                }
            }

          Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
        }


      close_llz (llz_hnd);


      //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
      //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
      //  operation.

      for (i = 0 ; i < pfm_file_count ; i++) 
        {
          //  May be MSYS on Windoze so we have to check for either separator.

          if (strrchr (path, '/'))
            {
              ptr = strrchr (path, '/');
            }
          else if (strrchr (path, '\\'))
            {
              ptr = strrchr (path, '\\');
            }
          else
            {
              ptr = NULL;
            }


          if (ptr == NULL) 
            {
              strcpy (line, path);
            }
          else
            {
              strcpy (line, (ptr + 1));
            }

          write_line_file (pfm_def[i].hnd, line);


          //  Convert to native separators so that we get nice file names on Windows.

          nativePath = QDir::toNativeSeparators (QString (path));
          strcpy (native_path, nativePath.toLatin1 ());

          write_list_file (pfm_def[i].hnd, native_path, PFM_NAVO_LLZ_DATA);
          count[i] += out_count[i];
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }

  return (total_out_of_limits);
}



uint8_t check_czmil_file (char *path, uint8_t csf_attr, WKT *wkt_data)
{
  int32_t            cpf_hnd, csf_hnd;
  CZMIL_CPF_Header   czmil_cpf_header;
  CZMIL_CSF_Header   czmil_csf_header;
  QString            cpfName, csfName;
  char               csf_name[1024];


  //  Open the CZMIL CPF file.

  if ((cpf_hnd = czmil_open_cpf_file (path, &czmil_cpf_header, CZMIL_READONLY)) < 0)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file\n%1 : %2\n").arg (path).arg (czmil_strerror ()).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }

  czmil_close_cpf_file (cpf_hnd);

  if (csf_attr)
    {
      cpfName = QString (path);
      csfName = cpfName.replace (".cpf", ".csf");

      strcpy (csf_name, csfName.toLatin1 ());


      //  Open the CZMIL CSF file.

      if ((csf_hnd = czmil_open_csf_file (csf_name, &czmil_csf_header, CZMIL_READONLY)) < 0)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file\n%1 : %2\n").arg (csf_name).arg (czmil_strerror ()).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (NVFalse);
        }


      czmil_close_csf_file (csf_hnd);
    }


  //  Since we cleared the file checks we want to take the WKT from the CZMIL CPF header and save it so we can put it in the PFM header.
  //  CZMIL data is ALWAYS geographic so we can just transfer the WKT.  Only do this once - we want the first one.

  if (strlen (wkt_data->pfm_wkt) < 10)
    {
      QString wkt = QString (czmil_cpf_header.wkt).remove ("{WELL-KNOWN TEXT =").remove ("}").simplified ();
      strcpy (wkt_data->pfm_wkt, wkt.toUtf8 ());
    }


  return (NVTrue);
}




/********************************************************************
 *
 * Function Name : load_czmil_file
 *
 * Description : Reads a CZMIL file.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_czmil_file (int32_t pfm_fc, int64_t *count, QString file, uint8_t csf_attr, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            j, k, beam, percent, cpf_hnd, csf_hnd = -1;
  uint32_t           flags = 0;
  uint16_t           major_version, minor_version;
  static int32_t     user_flag[10] = {PFM_USER_01, PFM_USER_02, PFM_USER_03, PFM_USER_04, PFM_USER_05, PFM_USER_06, PFM_USER_07, PFM_USER_08,
                                      PFM_USER_09, PFM_USER_10};
  CZMIL_CPF_Header   czmil_cpf_header;
  CZMIL_CPF_Data     cpf;
  NV_F64_COORD2      xy;
  float              herr, verr;
  char               line[256], *ptr;
  double             depth;
  CZMIL_CSF_Header   czmil_csf_header;
  CZMIL_CSF_Data     csf;
  QString            cpfName, csfName;
  char               csf_name[1024];


  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Get the next available file number so that we can put it into the index file.

  for (int32_t i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;
  recnum = 0;


  //  Open the CZMIL CPF file.

  if ((cpf_hnd = czmil_open_cpf_file (path, &czmil_cpf_header, CZMIL_READONLY_SEQUENTIAL)) < 0)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file\n%1 : %2\n").arg (path).arg (czmil_strerror ()).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }


  //  If we're loading CSF attributes we also need to open the CSF file.

  if (csf_attr)
    {
      cpfName = QString (path);
      csfName = cpfName.replace (".cpf", ".csf");

      strcpy (csf_name, csfName.toLatin1 ());


      //  Open the CZMIL CSF file.

      if ((csf_hnd = czmil_open_csf_file (csf_name, &czmil_csf_header, CZMIL_READONLY)) < 0)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file\n%1 : %2\n").arg (csf_name).arg (czmil_strerror ()).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (NVFalse);
        }
    }


  //  We need to get the file's major version number so we can handle a NULL (.99) reflectance value in pre 2.0 files.

  czmil_get_version_numbers (czmil_cpf_header.version, &major_version, &minor_version);


  //  Read all of the data from this file.  

  for (int32_t i = 0 ; i < czmil_cpf_header.number_of_records ; i++)
    {
      percent = (int32_t) (((float) i / (float) czmil_cpf_header.number_of_records) * 100.0);


      lidar_null = NVFalse;


      if (czmil_read_cpf_record (cpf_hnd, i, &cpf) == CZMIL_SUCCESS)
        {
          //  If we are loading CSF attributes we need to read the CSF record as well.

          if (csf_attr)
            {
              if (czmil_read_csf_record (csf_hnd, i, &csf) != CZMIL_SUCCESS)
                {
                  //  This shouldn't happen.

                  czmil_perror ();
                  break;
                }
            }


          recnum = i;

          for (j = 0 ; j < 9 ; j++)
            {
              if (cpf.returns[j] && load_parms[0].flags.czmil_chan[j])
                {
                  //  Process all returns for the channel.

                  for (k = 0 ; k < cpf.returns[j] ; k++)
                    {
                      //  We need to ignore any points that have a filter_reason of CZMIL_REPROCESSING_BUFFER.

                      if (cpf.channel[j][k].filter_reason != CZMIL_REPROCESSING_BUFFER)
                        {
                          //  Check for loading CZMIL HydroFusion filter invalidated data (we still load application filtered data).

                          if (load_parms[0].flags.HF_filt || cpf.channel[j][k].filter_reason < 1 ||
                              cpf.channel[j][k].filter_reason >= CZMIL_APP_HP_FILTERED)
                            {
                              //  Check for loading based on classification.  The three types are "land", "water", and "water surface".  These
                              //  classifications follow (for the most part) the ASPRS LAS v1.4 r13 spec for extended classification and the
                              //  2013 ASPRS paper - "LAS Domain Profile Description: Topo-Bathy Lidar", July 17, 2013."  That is, land
                              //  will be defined as classifications 0 through 39, water is classifications 40 and 43 through 45, and water
                              //  surface is classifications 41 and 42.

                              if ((load_parms[0].flags.czmil_land && cpf.channel[j][k].classification < 40) ||
                                  (load_parms[0].flags.czmil_water && (cpf.channel[j][k].classification == 40 ||
                                                                       cpf.channel[j][k].classification > 43)) ||
                                  (load_parms[0].flags.czmil_water_surface && (cpf.channel[j][k].classification == 41 ||
                                                                               cpf.channel[j][k].classification == 42)))
                                {
                                  beam = j * 100 + k;

                                  xy.y = cpf.channel[j][k].latitude;
                                  xy.x = cpf.channel[j][k].longitude;

                                  flags = 0;

                                  if (load_parms[0].pfm_global.attribute_count)
                                    {
                                      getCZMILAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                          load_parms[0].pfm_global.czmil_attribute_num, &cpf, &csf, j, k, major_version);
                                    }


                                  //  These are non-null depths based on the null_z value.

                                  if (cpf.channel[j][k].elevation != czmil_cpf_header.null_z_value)
                                    {
                                      //  Check for NULL returns as defined by CZMIL v2 (filter_reason between 1 and 15).  These will have a
                                      //  valid Z value so they won't show up as NULLS in pfmEdit3D.  We'll have to figure out how to handle
                                      //  this at a later date.

                                      if (cpf.channel[j][k].filter_reason == 0 || cpf.channel[j][k].filter_reason > 15)
                                        {
                                          //  Flipping elevation to depth after subtracting local vertical datum offset stored in the record.

                                          depth = -(cpf.channel[j][k].elevation - cpf.local_vertical_datum_offset);

                                          flags = 0;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_MANUALLY_INVAL) flags = PFM_MANUALLY_INVAL;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_FILTER_INVAL) flags = PFM_FILTER_INVAL;


                                          if (cpf.channel[j][k].status & CZMIL_RETURN_REFERENCE) flags |= PFM_REFERENCE;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_SUSPECT) flags |= PFM_SUSPECT;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_CLASSIFICATION_MODIFIED) flags |= PFM_SELECTED_SOUNDING;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_SELECTED_FEATURE) flags |= PFM_SELECTED_FEATURE;
                                          if (cpf.channel[j][k].status & CZMIL_RETURN_DESIGNATED_SOUNDING) flags |= PFM_DESIGNATED_SOUNDING;


                                          //  If we are loading water surface returns and we're referencing the water surface, set the
                                          //  reference flag (unless, of course, the waveform has been CZMIL_OPTECH_CLASS_HYBRID processed).

                                          if (load_parms[0].flags.czmil_water_surface && load_parms[0].flags.ref_water_surface &&
                                              (cpf.channel[j][k].classification == 41 || cpf.channel[j][k].classification == 42))
                                            flags |= PFM_REFERENCE;


                                          //  Set the CZMIL flags if requested.  The flags are defined in attributes.cpp.

                                          if (load_parms[0].pfm_global.czmil_flag_count)
                                            {
                                              int32_t set_count = 0;


                                              //  Note: backwards loop.

                                              for (int32_t flg = CZMIL_FLAGS - 1 ; flg >= 0 ; flg--)
                                                {
                                                  if (load_parms[0].pfm_global.czmil_flag_num[flg])
                                                    {
                                                      //  Hockey puck filtered data...

                                                      if (flg == 5)
                                                        {
                                                          if ((cpf.channel[j][k].status & CZMIL_RETURN_FILTER_INVAL) &&
                                                              (cpf.channel[j][k].filter_reason == CZMIL_APP_HP_FILTERED))
                                                            flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                        }


                                                      //  Reprocessed data can also be flagged as "Land", "Water", "Hybrid", or
                                                      //  "Shallow water" so we don't break out of the loop on this one.  This is one of
                                                      //  the reasons that the loop is run backwards.

                                                      if (flg == 4)
                                                        {
                                                          if (cpf.channel[j][k].status & CZMIL_RETURN_REPROCESSED)
                                                            flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                        }


                                                      //  Shallow Water.

                                                      else if (flg == 3)
                                                        {
                                                          if (cpf.optech_classification[j] == CZMIL_OPTECH_CLASS_SHALLOW_WATER)
                                                            {
                                                              flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                              break;
                                                            }
                                                        }


                                                      //  Hybrid processed

                                                      else if (flg == 2)
                                                        {
                                                          if (cpf.optech_classification[j] == CZMIL_OPTECH_CLASS_HYBRID)
                                                            {
                                                              flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                              break;
                                                            }
                                                        }


                                                      //  Water (based on classification)

                                                      else if (flg == 1)
                                                        {
                                                          if (cpf.channel[j][k].classification == 40 ||
                                                              cpf.channel[j][k].classification == 43 ||
                                                              cpf.channel[j][k].classification == 44)
                                                            {
                                                              flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                              break;
                                                            }
                                                        }


                                                      //  Land (based on classification)

                                                      else if (flg == 0)
                                                        {
                                                          if (cpf.channel[j][k].classification < 40)
                                                            {
                                                              flags |= user_flag[load_parms[0].pfm_global.czmil_flag_num[flg] - 1];
                                                              break;
                                                            }
                                                        }


                                                      //  Keep track of how many flags we've checked and break out of the loop when we're done.
                                                      //  Hopefully this will make things a bit faster.

                                                      set_count++;
                                                      if (set_count == load_parms[0].pfm_global.czmil_flag_count) break;
                                                    }
                                                }
                                            }

                                          herr = cpf.channel[j][k].horizontal_uncertainty;
                                          verr = cpf.channel[j][k].vertical_uncertainty;


                                          Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
      else
        {
          //  This shouldn't happen.

          czmil_perror ();
          break;
        }
    }


  czmil_close_cpf_file (cpf_hnd);
  if (csf_attr) czmil_close_csf_file (csf_hnd);


  //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
  //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
  //  operation.

  for (int32_t i = 0 ; i < pfm_file_count ; i++) 
    {
      //  May be MSYS on Windoze so we have to check for either separator.

      if (strrchr (path, '/'))
        {
          ptr = strrchr (path, '/');
        }
      else if (strrchr (path, '\\'))
        {
          ptr = strrchr (path, '\\');
        }
      else
        {
          ptr = NULL;
        }


      if (ptr == NULL) 
        {
          strcpy (line, path);
        }
      else
        {
          strcpy (line, (ptr + 1));
        }

      write_line_file (pfm_def[i].hnd, line);


      //  Convert to native separators so that we get nice file names on Windows.

      nativePath = QDir::toNativeSeparators (QString (path));
      strcpy (native_path, nativePath.toLatin1 ());

      write_list_file (pfm_def[i].hnd, native_path, PFM_CZMIL_DATA);
      count[i] += out_count[i];
    }

  return (total_out_of_limits);
}



/********************************************************************
 *
 * Function Name : load_srtm_file
 *
 * Description : Reads the compressed Shuttle Radar Topography Mission
 *               (SRTM) data and loads it as PFM_SRTM_DATA and,
 *               optionally, flagged as reference data.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_srtm_file (int32_t pfm_fc, int64_t *count, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam, i, percent;
  uint32_t           flags, type = 0;
  NV_F64_COORD2      xy;
  float              herr, verr;
  double             depth, min_x, min_y, max_x, max_y;
  char               line[256], *ptr;
  QString            string;


  
  type = 0;
  string = "/Best_available_resolution_SRTM_data";

  if (load_parms[0].flags.srtm1)
    {
      type = 1;
      string = "/1_second_SRTM_data";
    }
  else if (load_parms[0].flags.srtm3)
    {
      type = 2;
      string = "/3_second_SRTM_data";
    }
  else if (load_parms[0].flags.srtm30)
    {
      type = 3;
      string ="/30_second_SRTM_data";
    }


  if (load_parms[0].flags.srtmr) string += "__Reference";
  if (type <= 1 && !load_parms[0].flags.srtme) string += "__SRTM2";


  strcpy (path, string.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;



  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.  We also need to get the min and max boundaries for all
  //  of the PFM areas.

  min_x = 999.0;
  min_y = 999.0;
  max_x = -999.0;
  max_y = -999.0;
  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);

      if (pfm_def[i].open_args.head.mbr.min_x < min_x) min_x = pfm_def[i].open_args.head.mbr.min_x;
      if (pfm_def[i].open_args.head.mbr.max_x > max_x) max_x = pfm_def[i].open_args.head.mbr.max_x;
      if (pfm_def[i].open_args.head.mbr.min_y < min_y) min_y = pfm_def[i].open_args.head.mbr.min_y;
      if (pfm_def[i].open_args.head.mbr.max_y > max_y) max_y = pfm_def[i].open_args.head.mbr.max_y;
    }

  percent = 0;
  recnum = 0;


  int32_t start_lat = (int32_t) min_y;
  int32_t start_lon = (int32_t) min_x;
  int32_t end_lat = (int32_t) max_y + 1;
  int32_t end_lon = (int32_t) max_x + 1;

  if (min_y < 0.0)
    {
      start_lat--;
      if (max_y < 0.0) end_lat--;
    }

  if (min_x < 0.0)
    {
      start_lon--;
      if (max_x < 0.0) end_lon--;
    }

  int32_t range_y = end_lat - start_lat;
  int32_t range_x = end_lon - start_lon;
  int32_t total_cells = range_y * range_x;

  int16_t *array;
  double hinc, winc;
  int32_t wsize = -1, hsize = -1, incount = 0;


  for (int32_t lat = start_lat ; lat < end_lat ; lat++)
    {
      for (int32_t lon = start_lon ; lon < end_lon ; lon++)
        {
          switch (type)
            {
            case 0:
              wsize = read_srtm1_topo_one_degree (lat, lon, &array);
              hsize = wsize;


              //  Try the SRTM2 1 second data if the SRTM1 data area was undefined.

              if (wsize == 2 && !load_parms[0].flags.srtme)
                {
                  wsize = read_srtm2_topo_one_degree (lat, lon, &array);
                  hsize = wsize;
                }
              break;

            case 1:
              set_exclude_srtm2_data (load_parms[0].flags.srtme);
              wsize = read_srtm_topo_one_degree (lat, lon, &array);
              hsize = wsize;
              break;

            case 2:
              wsize = read_srtm3_topo_one_degree (lat, lon, &array);
              hsize = wsize;
              break;

            case 3:
              wsize = read_srtm30_topo_one_degree (lat, lon, &array);
              hsize = wsize;
              break;
            }


          //  Check for the SRTM2 1 by 2 second data (returns a wsize of 1800).

          if (wsize == 1800) hsize = 3600;


          if (wsize > 2)
            {
              winc = 1.0L / (double) wsize;
              hinc = 1.0L / (double) hsize;

              for (int32_t i = 0 ; i < hsize ; i++)
                {
                  xy.y = (lat + 1.0L) - (i + 1) * hinc;
                  for (int32_t j = 0 ; j < wsize ; j++)
                    {
                      //  We don't load zeros from SRTM since that is the only way they marked water surface.
                      //  This leaves a ring around Death Valley ;-)

                      if (array[i * wsize + j] && array[i * wsize + j] > -32768)
                        {
                          xy.x = lon + j * winc;
                          depth = (double) (-array[i * wsize + j]);
                          flags = 0;
                          if (load_parms[0].flags.srtmr) flags = PFM_REFERENCE;
                          beam = 0;
                          incount++;


                          //  Record number is meaningless for SRTM data and can exceed the PFM limits.

                          recnum = 0;
                          herr = 0.0;
                          verr = 0.0;
                          Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                        }
                    }
                  percent = (int32_t) (((float) ((lat - start_lat) * (float) range_x +
                                                       (float) (lon - start_lon)) / (float) total_cells) * 100.0) +
                    (int32_t) (((float) i / (float) hsize) * 100.0 * (1.0 / total_cells));
                }
            }
        }
    }


  switch (type)
    {
    case 0:
      cleanup_srtm_topo ();
      break;

    case 1:
      cleanup_srtm1_topo ();
      if (!load_parms[0].flags.srtme) cleanup_srtm2_topo ();
      break;

    case 2:
      cleanup_srtm3_topo ();
      break;

    case 3:
      cleanup_srtm30_topo ();
      break;
    }


  //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
  //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
  //  operation.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      //  May be MSYS on Windoze so we have to check for either separator.

      if (strrchr (path, '/'))
        {
          ptr = strrchr (path, '/');
        }
      else if (strrchr (path, '\\'))
        {
          ptr = strrchr (path, '\\');
        }
      else
        {
          ptr = NULL;
        }


      if (ptr == NULL) 
        {
          strcpy (line, path);
        }
      else
        {
          strcpy (line, (ptr + 1));
        }

      write_line_file (pfm_def[i].hnd, line);


      //  Convert to native separators so that we get nice file names on Windows.

      nativePath = QDir::toNativeSeparators (QString (path));
      strcpy (native_path, nativePath.toLatin1 ());

      write_list_file (pfm_def[i].hnd, native_path, PFM_NAVO_ASCII_DATA);
      count[i] += out_count[i];
    }

  return (0);
}



uint8_t check_dted_file (char *path)
{
  FILE           *fp;
  UHL            uhl;


  //  Open the DTED file.

  if ((fp = fopen (path, "rb")) == NULL)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the DTED file.\n%1 : %2").arg (path).arg (strerror (errno)).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }


  if (read_uhl (fp, &uhl) < 0)
    {
      strcpy (out_str, pfmLoader::tr ("\n\n%1 is not a DTED file.\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (NVFalse);
    }


  fclose (fp);

  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_dted_file
 *
 * Description : Reads NGA DTED format files.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_dted_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam, i, j, percent, total_points, status;
  FILE               *fp;
  UHL                uhl;
  uint32_t           flags;
  NV_F64_COORD2      xy;
  float              herr, verr;
  double             depth, lat_inc, lon_inc;
  char               line[256], *ptr;
  DTED_DATA          dted_data;


  
  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.  We also need to get the min and max boundaries for all
  //  of the PFM areas.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;
  recnum = 0;


  //  Open the DTED file and read the data.

  if ((fp = fopen (path, "rb")) != NULL)
    {
      //  Read the UHL record.

      read_uhl (fp, &uhl);

      lon_inc = uhl.lon_int / 3600.0;
      lat_inc = uhl.lat_int / 3600.0;


      //  Read all of the data from this file.  

      total_points = uhl.num_lat_points * uhl.num_lon_lines;

      for (i = 0 ; i < uhl.num_lon_lines ; i++)
        {
          xy.x = uhl.ll_lon + (double) i * lon_inc;


          if ((status = read_dted_data (fp, uhl.num_lat_points, i, &dted_data))) 
            {
              strcpy (out_str, pfmLoader::tr ("READ ERROR %1\n").arg (status).toUtf8 ());
              fprintf (stderr, "%s", out_str);
              fflush (stderr);
              return (0);
            }


          for (j = 0 ; j < uhl.num_lat_points ; j++)
            {
              xy.y = uhl.ll_lat + (double) j * lat_inc;


              percent = (int32_t) ((((float) i * uhl.num_lon_lines) / (float) total_points) * 100.0);


              depth = -dted_data.elev[j];


              //  Loading undefined values as an imposible depth so that we can get rid of bad -32767 values.
 
              if (dted_data.elev[j] <= -32767) depth = 11999.0;


              flags = 0;

              recnum = i;
              beam = j;

              herr = 0.0;
              verr = 0.0;
              Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
            }
        }


      fclose (fp);


      //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
      //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
      //  operation.

      for (i = 0 ; i < pfm_file_count ; i++) 
        {
          //  May be MSYS on Windoze so we have to check for either separator.

          if (strrchr (path, '/'))
            {
              ptr = strrchr (path, '/');
            }
          else if (strrchr (path, '\\'))
            {
              ptr = strrchr (path, '\\');
            }
          else
            {
              ptr = NULL;
            }


          if (ptr == NULL) 
            {
              strcpy (line, path);
            }
          else
            {
              strcpy (line, (ptr + 1));
            }

          write_line_file (pfm_def[i].hnd, line);


          //  Convert to native separators so that we get nice file names on Windows.

          nativePath = QDir::toNativeSeparators (QString (path));
          strcpy (native_path, nativePath.toLatin1 ());

          write_list_file (pfm_def[i].hnd, native_path, PFM_DTED_DATA);
          count[i] += out_count[i];
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }

  return (total_out_of_limits);
}



uint8_t check_chrtr_file (char *path)
{
  int32_t        chrtr_handle;
  CHRTR_HEADER   chrtr_header;
  CHRTR2_HEADER  chrtr2_header;


  //  Since we're phasing out the use of the old CHRTR format we're reusing the PFM_CHRTR_DATA flag for the new CHRTR2 
  //  format.  There will be a transitional period where both must be supported.  Nobody uses CHRTR or CHRTR2 except 
  //  NAVO so I doubt if this will cause a problem.  JCD 01/17/11

  if (!strcmp (path, ".ch2"))
    {
      if ((chrtr_handle = chrtr2_open_file (path, &chrtr2_header, CHRTR2_READONLY)) < 0)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the CHRTR2 file.\n%1 : %2\n").arg (path).arg (chrtr2_strerror ()).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (NVFalse);
        }

      chrtr2_close_file (chrtr_handle);
    }
  else
    {
      if ((chrtr_handle = open_chrtr (path, &chrtr_header)) < 0)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the CHRTR file.\n%1 : %2\n").arg (path).arg (strerror (errno)).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (NVFalse);
        }

      close_chrtr (chrtr_handle);
    }

  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_chrtr_file
 *
 * Description : Reads NAVO CHRTR format files.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_chrtr_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[])
{
  int32_t            beam, i, j, percent, total_points;
  int32_t            chrtr_handle;
  CHRTR_HEADER       chrtr_header;
  float              *chrtr_data;
  CHRTR2_HEADER      chrtr2_header;
  CHRTR2_RECORD      *chrtr2_data;
  uint32_t           flags;
  NV_F64_COORD2      xy;
  float              herr, verr;
  double             depth;
  char               line[256], *ptr;


  
  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.  We also need to get the min and max boundaries for all
  //  of the PFM areas.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;
  recnum = 0;
  chrtr_data = NULL;


  //  Since we're phasing out the use of the old CHRTR format we're reusing the PFM_CHRTR_DATA flag for the new CHRTR2 
  //  format.  There will be a transitional period where both must be supported.  Nobody uses CHRTR or CHRTR2 except 
  //  NAVO so I doubt if this will cause a problem.  JCD 01/17/11

  if (!strcmp (path, ".ch2"))
    {
      if ((chrtr_handle = chrtr2_open_file (path, &chrtr2_header, CHRTR2_READONLY)) >= 0)
        {
          chrtr2_data = (CHRTR2_RECORD *) malloc (chrtr2_header.width * sizeof (CHRTR2_RECORD));
          if (chrtr2_data == NULL)
            {
              perror ("Allocating memory for CHRTR2 input array in load_file");
              exit (-1);
            }


          uint8_t huncert = NVFalse;
          uint8_t vuncert = NVFalse;
          uint8_t vstd = NVFalse;
          if (chrtr2_header.horizontal_uncertainty_scale != 0.0) huncert = NVTrue;
          if (chrtr2_header.vertical_uncertainty_scale != 0.0) vuncert = NVTrue;
          if (chrtr2_header.uncertainty_scale != 0.0) vstd = NVTrue;

          double half_x = chrtr2_header.lon_grid_size_degrees * EPS;
          double half_y = chrtr2_header.lat_grid_size_degrees * EPS;


          //  Read all of the data from this file.  

          total_points = chrtr2_header.width * chrtr2_header.height;

          for (i = 0 ; i < chrtr2_header.height ; i++)
            {
              //  Compute the latitude of the center of the bin.

              NV_F64_COORD2 xy;
              xy.y = chrtr2_header.mbr.slat + i * chrtr2_header.lat_grid_size_degrees + half_y;


              if (!chrtr2_read_row (chrtr_handle, i, 0, chrtr2_header.width, chrtr2_data))
                {
                  for (j = 0 ; j < chrtr2_header.width ; j++)
                    {
                      //  Compute the longitude of the center of the bin.

                      xy.x = chrtr2_header.mbr.wlon + j * chrtr2_header.lon_grid_size_degrees + half_x;


                      //  Only load "real" data.

                      if (chrtr2_data[j].status & (CHRTR2_REAL | CHRTR2_DIGITIZED_CONTOUR))
                        {
                          depth = chrtr2_data[j].z;
                          flags = 0;

                          recnum = i;
                          beam = j;

                          herr = 0.0;
                          verr = 0.0;

                          if (huncert) herr = chrtr2_data[j].horizontal_uncertainty;
                          if (vuncert)
                            {
                              verr = chrtr2_data[j].vertical_uncertainty;
                            }
                          else if (vstd)
                            {
                              verr = chrtr2_data[j].uncertainty;
                            }

                          percent = (int32_t) ((((float) (i * chrtr2_header.width + j)) / (float) total_points) * 100.0);

                          Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                        }
                    }
                }
            }

          if (chrtr2_data) free (chrtr2_data);
          chrtr2_close_file (chrtr_handle);
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the file %1\n").arg (path).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (0);
        }
    }
  else
    {
      //  Open the CHRTR file and read the data.

      if ((chrtr_handle = open_chrtr (path, &chrtr_header)) >= 0)
        {
          chrtr_data = (float *) malloc (chrtr_header.width * sizeof (float));
          if (chrtr_data == NULL)
            {
              perror ("Allocating memory for CHRTR input array in load_file");
              exit (-1);
            }


          //  Read all of the data from this file.  

          double grid_degrees = chrtr_header.grid_minutes / 60.0;
          total_points = chrtr_header.width * chrtr_header.height;

          for (i = 0 ; i < chrtr_header.height ; i++)
            {
              xy.y = chrtr_header.slat + (double) i * grid_degrees;

              if (read_chrtr (chrtr_handle, i, 0, chrtr_header.width, chrtr_data))
                {
                  for (j = 0 ; j < chrtr_header.width ; j++)
                    {
                      xy.x = chrtr_header.wlon + (double) j * grid_degrees;


                      //  Only load "real" data.

                      if (bit_test (chrtr_data[j], 0))
                        {
                          depth = chrtr_data[j];
                          flags = 0;

                          recnum = i;
                          beam = j;

                          herr = 0.0;
                          verr = 0.0;

                          percent = (int32_t) ((((float) (i * chrtr_header.width + j)) / (float) total_points) * 100.0);

                          Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                        }
                    }
                }
            }

          if (chrtr_data) free (chrtr_data);
          close_chrtr (chrtr_handle);
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the file %1\n").arg (path).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          return (0);
        }
    }


  //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
  //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
  //  operation.

  for (i = 0 ; i < pfm_file_count ; i++) 
    {
      //  May be MSYS on Windoze so we have to check for either separator.

      if (strrchr (path, '/'))
        {
          ptr = strrchr (path, '/');
        }
      else if (strrchr (path, '\\'))
        {
          ptr = strrchr (path, '\\');
        }
      else
        {
          ptr = NULL;
        }


      if (ptr == NULL) 
        {
          strcpy (line, path);
        }
      else
        {
          strcpy (line, (ptr + 1));
        }

      write_line_file (pfm_def[i].hnd, line);


      //  Convert to native separators so that we get nice file names on Windows.

      nativePath = QDir::toNativeSeparators (QString (path));
      strcpy (native_path, nativePath.toLatin1 ());

      write_list_file (pfm_def[i].hnd, native_path, PFM_CHRTR_DATA);
      count[i] += out_count[i];
    }

  return (total_out_of_limits);
}



uint8_t check_bag_file (char *path)
{
  bagHandle                    bagHnd;
  bagError                     bagErr;


  //  Open the BAG file.

  if ((bagErr = bagFileOpen (&bagHnd, BAG_OPEN_READONLY, (u8 *) path)) != BAG_SUCCESS)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the BAG file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);

      u8 *errstr;

      if (bagGetErrorString (bagErr, &errstr) == BAG_SUCCESS)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nError opening the BAG file\n%1 : %2\n").arg (path).arg ((char *) errstr).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }

      return (NVFalse);
    }


  bagFileClose (bagHnd);


  return (NVTrue);
}



/********************************************************************
 *
 * Function Name : load_bag_file
 *
 * Description : Reads NGA BAG format files.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/

int32_t load_bag_file (int32_t pfm_fc, int64_t *count, QString file, PFM_LOAD_PARAMETERS load_parms[], PFM_DEFINITION pfm_def[], WKT *wkt_data)
{
  bagHandle                    bagHnd;
  bagError                     bagErr;
  char                         *xml_str = NULL;
  int32_t                      length;  
  BAG_METADATA                 bag_metadata;
  bagLegacyReferenceSystem     system;
  BAG_RECORD                   bag;
  int32_t                      beam, percent;
  uint32_t                     flags;
  NV_F64_COORD2                xy;
  float                        herr, verr;
  double                       depth;
  char                         line[256], *ptr;


  
  strcpy (path, file.toLatin1 ());


  //  Zero count on number of soundings per file. 

  memset (out_count, 0, MAX_PFM_FILES * sizeof (int64_t));
  pfm_file_count = pfm_fc;
  total_out_of_limits = 0;


  //  Clear the attributes.

  if (load_parms[0].pfm_global.attribute_count) for (int32_t i = 0 ; i < NUM_ATTR ; i++) attr[i] = 0.0;


  //  Just so that we don't mistakenly think this might be a HOF LIDAR NULL.

  lidar_null = NVFalse;


  //  Get the next available file number so that we can put it into the index file.  We also need to get the min and max boundaries for all
  //  of the PFM areas.

  for (int32_t i = 0 ; i < pfm_file_count ; i++) 
    {
      line_count[i] = -1; 
      next_line[i] = get_next_line_number (pfm_def[i].hnd);
      file_number[i] = get_next_list_file_number (pfm_def[i].hnd);
    }

  percent = 0;
  recnum = 0;


  //  Open the BAG file and read the data.

  if ((bagErr = bagFileOpen (&bagHnd, BAG_OPEN_READONLY, (u8 *) path)) == BAG_SUCCESS)
    {
      bagReadXMLStream (bagHnd);


      xml_str = (char*)malloc(sizeof(char) * XML_METADATA_MAX_LENGTH);

      strcpy(xml_str, (const char *) bagGetDataPointer (bagHnd)->metadata);
      length = strlen (xml_str);


      /* initialize the bag_metadata structure */

      bagInitMetadata (&bag_metadata);


      /* read the xml metadata into the bag_metadata structure */

      bagImportMetadataFromXmlBuffer ((u8 *) xml_str, length, &bag_metadata, False);
      

      free (xml_str);

      
      /* retrieve coordsys info and vertical datum from wkt */

      bagWktToLegacy ((const char *) bag_metadata.horizontalReferenceSystem->definition, (const char *) bag_metadata.verticalReferenceSystem->definition, &system);  


      //  Check the projection.

      uint8_t projected = NVFalse;
      int32_t data_cols, data_rows;
      double x_bin_size, y_bin_size, sw_corner_x, sw_corner_y, falseNorthing = 0.0, falseEasting = 0.0;
      projPJ pj_utm = NULL, pj_latlon = NULL;


      //  Check to see if this is a new BAG with WKT.

      if (strstr ((char *) bag_metadata.horizontalReferenceSystem->type, "WKT"))
        {
          QString wktString = QString ((const char *) bag_metadata.horizontalReferenceSystem->definition);

          if (wktString.contains ("PROJCS"))
            {
              char wkt[8192];
              strcpy (wkt, wktString.toLatin1 ());

              projected = NVTrue;

              OGRSpatialReference SRS;
              char string[128], *ppszProj4, *ptr_wkt = wkt;

              if (SRS.importFromWkt (&ptr_wkt) != OGRERR_NONE)
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1, can't import from WKT:\n%2\n").arg (path).arg (ptr_wkt).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }

              if (SRS.exportToProj4 (&ppszProj4) != OGRERR_NONE)
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1, can't export to Proj4:\n%2\n").arg (path).arg (ptr_wkt).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }


              strcpy (string, ppszProj4);
              OGRFree (ppszProj4);

              if (!(pj_utm = pj_init_plus (string)))
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1, error initializing UTM projection\n").arg (path).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }

              strcpy (string, "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

              OGRSpatialReference pfmSRS;
              ppszProj4 = string;
              pfmSRS.importFromProj4 (ppszProj4);
              pfmSRS.exportToWkt (&ptr_wkt);

              strcpy (wkt_data->pfm_wkt, ptr_wkt);
              OGRFree (ptr_wkt);


              if (!(pj_latlon = pj_init_plus (string)))
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1, error initializing latlon projection\n").arg (path).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }
            }
          else
            {
              projected = NVFalse;

              if (!wktString.contains ("GEOGCS"))
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1 not Geodetic or UTM projection.\n").arg (path).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }

              strcpy (wkt_data->pfm_wkt, wktString.toLatin1 ());
            }
        }
      else
        {
          if (system.coordSys == UTM)
            {
              projected = NVTrue;

              falseNorthing = system.geoParameters.false_northing;
              falseEasting = system.geoParameters.false_easting;


              //  Set up the UTM projection.

              char string[128];
              if (system.geoParameters.latitude_of_centre < 0.0)
                {
                  sprintf (string, "+proj=utm +lon_0=%.9f +ellps=WGS84 +datum=WGS84 +south", system.geoParameters.longitude_of_centre);
                }
              else
                {
                  sprintf (string, "+proj=utm +lon_0=%.9f +ellps=WGS84 +datum=WGS84", system.geoParameters.longitude_of_centre);
                }

              if (!(pj_utm = pj_init_plus (string)))
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("Error initializing UTM projection\n").toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }


              strcpy (string, "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

              OGRSpatialReference pfmSRS;
              char *ppszProj4, *ptr_wkt;
              ppszProj4 = string;
              pfmSRS.importFromProj4 (ppszProj4);
              pfmSRS.exportToWkt (&ptr_wkt);

              strcpy (wkt_data->pfm_wkt, ptr_wkt);
              OGRFree (ptr_wkt);


              if (!(pj_latlon = pj_init_plus (string)))
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("Error initializing latlon projection\n").toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }
            }
          else
            {
              projected = NVFalse;

              if (system.coordSys != Geodetic)
                {
                  bagFileClose (bagHnd);
                  strcpy (out_str, pfmLoader::tr ("\n\nBAG file %1 not Geodetic or UTM projection.\n").arg (path).toUtf8 ());
                  fprintf (stderr, "%s", out_str);
                  fflush (stderr);
                  return (0);
                }
            }
        }


      if (projected)
        {
          data_rows = bag_metadata.spatialRepresentationInfo->numberOfRows;
          data_cols = bag_metadata.spatialRepresentationInfo->numberOfColumns;
          y_bin_size = bag_metadata.spatialRepresentationInfo->rowResolution;
          x_bin_size = bag_metadata.spatialRepresentationInfo->columnResolution;

          sw_corner_x = bag_metadata.spatialRepresentationInfo->llCornerX;
          sw_corner_y = bag_metadata.spatialRepresentationInfo->llCornerY;
        }
      else
        {
          data_cols = bagGetDataPointer (bagHnd)->def.ncols;
          data_rows = bagGetDataPointer (bagHnd)->def.nrows;
          x_bin_size = bagGetDataPointer (bagHnd)->def.nodeSpacingX;
          y_bin_size = bagGetDataPointer (bagHnd)->def.nodeSpacingY;
          sw_corner_y = bagGetDataPointer (bagHnd)->def.swCornerY;
          sw_corner_x = bagGetDataPointer (bagHnd)->def.swCornerX;
        }


      double half_y = y_bin_size * EPS;
      double half_x = x_bin_size * EPS;


      float *data = (float *) malloc (data_cols * sizeof (float));
      if (data == NULL) 
        {
          perror ("Allocating data memory in load_bag_file");
          exit (-1);
        }

      float *uncert = NULL;
      uncert = (float *) malloc (data_cols * sizeof (float));
      if (uncert == NULL) 
        {
          perror ("Allocating uncertainty memory in load_bag_file");
          exit (-1);
        }


      int32_t total_points = data_rows * data_cols;

      for (int32_t i = 0 ; i < data_rows ; i++)
        {
          bagErr = bagReadRow (bagHnd, i, 0, data_cols - 1, Elevation, (void *) data);

          bagErr = bagReadRow (bagHnd, i, 0, data_cols - 1, Uncertainty, (void *) uncert);


          double y_pos = sw_corner_y + (double) i * y_bin_size + half_y;


          for (int32_t j = 0 ; j < data_cols ; j++)
            {
              if (data[j] < NULL_ELEVATION)
                {
                  double x_pos = sw_corner_x + (double) j * x_bin_size + half_x;


                  //  If the data was projected we need to convert to lat/lon.

                  if (projected)
                    {
                      double easting = x_pos + falseEasting;
                      double northing = y_pos + falseNorthing;
                      pj_transform (pj_utm, pj_latlon, 1, 1, &easting, &northing, NULL);

                      xy.x = easting * NV_RAD_TO_DEG;
                      xy.y = northing * NV_RAD_TO_DEG;
                    }
                  else
                    {
                      xy.y = y_pos;
                      xy.x = x_pos;
                    }


                  if (uncert[j] < NULL_UNCERTAINTY)
                    {
                      bag.uncert = uncert[j];
                    }
                  else
                    {
                      bag.uncert = 1001.0;
                    }

                  bag.elev = -data[j];

                  if (load_parms[0].pfm_global.attribute_count) getBAGAttributes (attr, load_parms[0].pfm_global.time_attribute_num,
                                                                                  load_parms[0].pfm_global.bag_attribute_num, &bag);

                  flags = 0;

                  recnum = i;
                  beam = j;

                  herr = 0.0;
                  verr = 0.0;

                  depth = bag.elev;

                  percent = (int32_t) ((((float) (i * data_cols + j)) / (float) total_points) * 100.0);

                  Do_PFM_Processing (beam, depth, xy, herr, verr, flags, 0.0, attr, load_parms, pfm_def, percent, 0, 0);
                }
            }
        }


      bagFileClose (bagHnd);


      //  Whether we wrote any data to the PFM or not, write the input file name to the list file.  This helps us to avoid
      //  scanning the file again for this PFM if we inadvertently (or on purpose) add it to the file ist for an append
      //  operation.

      for (int32_t i = 0 ; i < pfm_file_count ; i++) 
        {
          //  May be MSYS on Windoze so we have to check for either separator.

          if (strrchr (path, '/'))
            {
              ptr = strrchr (path, '/');
            }
          else if (strrchr (path, '\\'))
            {
              ptr = strrchr (path, '\\');
            }
          else
            {
              ptr = NULL;
            }


          if (ptr == NULL) 
            {
              strcpy (line, path);
            }
          else
            {
              strcpy (line, (ptr + 1));
            }

          write_line_file (pfm_def[i].hnd, line);


          //  Convert to native separators so that we get nice file names on Windows.

          nativePath = QDir::toNativeSeparators (QString (path));
          strcpy (native_path, nativePath.toLatin1 ());

          write_list_file (pfm_def[i].hnd, native_path, PFM_BAG_DATA);
          count[i] += out_count[i];
        }
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("\n\nUnable to open the file %1\n").arg (path).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      return (0);
    }

  return (total_out_of_limits);
}
