
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


//  The <b></b> is just forcing the program to assume it's rich text format.

QString wktLabelText = 
  LAS_WKT::tr ("<b></b>So, the problem with many LAS files is that the coordinate system is not defined properly.  Even though the LAS specification "
               "requires a 34735 (GeoKeyDirectoryTag) Variable Length Record (VLR) many people don't put one in.  If you're using LAS 1.4 and "
               "you use point data formats above 5 you are required to insert a Well-known Text (WKT) VLR or Extended VLR (eVLR).  Apparently many "
               "people think this is a waste of time.  I find that rather interesting, especially with projected data.  For example, I recently "
               "saw some data from a LiDAR system in use in Japan that was in LAS 1.4 format using point data format 6.  This requires a WKT "
               "VLR or eVLR but there wasn't one in the file.  The global_encoding field was set to indicate that there was a 34735 VLR. "
               "That is an error.  There wasn't even a 34735 VLR anyway.  Since the file appeared to be UTM what we had was a whole bunch of "
               "measurements (northing and easting, presumably in meters) from nowhere (or possibly OZ).<br><br>"
               "Even when people insert the 34735 record they often leave out important information.  For instance, I've seen files that proudly "
               "proclaim that they are NAD83 UTM but then they don't put the zone information into the record.  Once again, meters (we hope) from "
               "nowhere.<br><br>"
               "Another pet peeve of mine - why do people use any vertical measurement other than meters (or metres if you will)?  Measuring "
               "things based on the length of some king's body parts seems a bit old school.  Wait until you've finished processing the data "
               "before you convert it to cubits or fathoms or furlongs or feet or yards for the great unwashed masses to gaze upon.  ABE assumes "
               "that the input elevations are in meters.  If the data in your LAS file isn't in meters it doesn't really matter.  Just look at the "
               "pretty numbers and say to yourself <b>'self, that number is really feet even though it says meters'</b>.  If you're building PFMs "
               "from LAS files I wouldn't recommend trying to extract (pfmExtract or pfmGeotiff) the data (meaning X, Y, and Z, not validity and/or "
               "classification).<br><br>"
               "Now, on to the fun stuff, if you're not familiar with WKT you can look it up on the interwebs.  To quote Wikipedia <b>'Well-known text "
               "(WKT) is a text markup language for representing vector geometry objects on a map, spatial reference systems of spatial objects and "
               "transformations between spatial reference systems'</b>. The world has moved past the GeoKeyDirectoryTag and is moving to WKT. Since this "
               "program uses the GDAL/OGR library it only supports projected (PROJCS) or geographic (GEOGCS) coordinate systems.  That's probably fine "
               "since that's what most people use (the other possibility being (GEOCCS) geocentric).<br><br>"
               "This program will try to decipher the crazy quilt LAS oordinate system information but it won't try very hard.  If you are using "
               "geographic data it will probably make it through fine (unless you've completely hosed or forgotten your 34735).  Projected data is "
               "much more complicated. In any event, if the program can't figure it out it will ask you for a WKT describing the coordinate system for "
               "your data.  You can find a bunch of WKT over on <a href=\"http://spatialreference.org/\">spatialreference.org</a>.<br><br>"
               "Once you've entered one or more WKT definitions the program will try to keep track of the 10 most recently used ones. "
               "These will be available in any of the ABE programs that have LAS inputs.  Hopefully you'll be using the same few for quite a while "
               "before you have to change to another area and/or coordinate system.  To see what each recent WKT string looks like just open the combo box "
               "on the bottom left and let the cursor hover over each entry.  The tool tip will be the full text of the WKT in human readable "
               "form.<br><br>"
               "<b>IMPORTANT NOTE: In the pfmLoad or pfmLoader program, if you input LAS files and have to input WKT to load them, the program will "
               "keep track of the WKT and will use it again if you append to the existing PFM.</b><br><br>"
               "Jan Depner, 3 May, 2016");
