<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>LAS_WKT</name>
    <message>
        <location filename="LAS_WKT.cpp" line="68"/>
        <location filename="LAS_WKT.cpp" line="817"/>
        <source>Recent WKT %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="216"/>
        <location filename="LAS_WKT.cpp" line="492"/>
        <source>%1 %2 %3 %4 - Error initializing UTM projection!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="235"/>
        <location filename="LAS_WKT.cpp" line="524"/>
        <source>%1 %2 %3 %4 - Error initializing latlon projection!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="544"/>
        <location filename="LAS_WKT.cpp" line="769"/>
        <location filename="LAS_WKT.cpp" line="829"/>
        <source>%1 Well-known Text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="562"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; is version 1.4.  The &lt;b&gt;global_encoding&lt;/b&gt; field of the LAS header indicates that there is a Coordinate System Well-known Text (WKT) record in the file however, it appears to be missing.  This means that the LAS file does not meet the LAS 1.4 specification.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="571"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; is version 1.4.  The &lt;b&gt;global_encoding&lt;/b&gt; field of the LAS header indicates that there is not a Coordinate System Well-known Text (WKT) record in the file.  Since the Point Data Record Format for this file is greater than 5, the use of the WKT record is required.  This LAS file does not meet the LAS 1.4 specification.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="581"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; is version 1.%2.  There are no Variable Length Records (VLR) defined for this file.  According to the LAS 1.%2 specification, a 34735 (GeoKeyDirectoryTag) VLR is required.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="590"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; has a 34735 Variable Length Record (VLR) but the &lt;b&gt;GTModelTypeGeoKey&lt;/b&gt; isn&apos;t &lt;b&gt;ModelTypeProjected&lt;/b&gt; or &lt;b&gt;ModelTypeGeographic&lt;/b&gt;.  &lt;b&gt;%2&lt;/b&gt; only handles those two types.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="598"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; is version 1.%2.  The required 34735 (GeoKeyDirectoryTag) Variable Length Record (VLR) is missing.  According to the LAS 1.%2 specification, a 34735 (GeoKeyDirectoryTag) VLR is required</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="606"/>
        <source>The LAS file &lt;b&gt;%1&lt;/b&gt; has a 34735 Variable Length Record (VLR) with the &lt;b&gt;GTModelTypeGeoKey&lt;/b&gt; set to &lt;b&gt;ModelTypeProjected&lt;/b&gt;.  &lt;b&gt;%2&lt;/b&gt; has determined that the &lt;b&gt;Geographic CS Type&lt;/b&gt; is not &lt;b&gt;GCS_NAD83&lt;/b&gt; (4269) or &lt;b&gt;GCS_WGS_84&lt;/b&gt; (4326).  &lt;b&gt;%2&lt;/b&gt; only handles those two types.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="612"/>
        <source>&lt;br&gt;&lt;br&gt;Due to the above listed condition &lt;b&gt;%1&lt;/b&gt; is unable to convert positions for the input LAS data to latitude and longitude.  If you would like to know why the positions are being converted to latitude and longitude you can read &lt;a href=&quot;http://pfmabe.software/a-note-about-utm-vs-latlon/&quot;&gt;this article&lt;/a&gt; on pfmabe.software.&lt;br&gt;&lt;br&gt;This dialog will allow you to enter Well-known Text (WKT) to be used to convert the positions.  WKT for many different areas is available on &lt;a href=&quot;http://spatialreference.org/&quot;&gt;spatialreference.org&lt;/a&gt; (for example, &lt;a href=&quot;http://spatialreference.org/ref/epsg/2451/&quot;&gt;EPSG:2451&lt;/a&gt;).  Only GEOGCS or PROJCS WKT is supported.&lt;br&gt;&lt;br&gt;Simply copy the &lt;b&gt;OGC WKT&lt;/b&gt; that best matches the data in the above LAS file and paste it into the text box below.&lt;br&gt;&lt;br&gt;Do not be overly concerned about whether the WKT is exactly correct for your data.  The Area-Based Editor (ABE) is only used to modify status (e.g. &lt;b&gt;Withheld&lt;/b&gt;, &lt;b&gt;Synthetic&lt;/b&gt;, &lt;b&gt;Key-point&lt;/b&gt;) and/or classification (e.g. &lt;b&gt;Low Vegetation&lt;/b&gt;, &lt;b&gt;Building&lt;/b&gt;, &lt;b&gt;Road Surface&lt;/b&gt;) in the LAS files (when unloaded from ABE).  The positions in the LAS files are not modified.  Because of this, the absolute accuracy of the positions in ABE is not really important.  It is much more important that the positions are accurate relative to each other and fairly accurate in relation to the globe (in order to use Google Earth for external reference).&lt;br&gt;&lt;br&gt;If the data in the LAS file is projected, &lt;b&gt;%1&lt;/b&gt; will convert the input positions to WGS84 latitude and longitude.&lt;br&gt;&lt;br&gt;It is assumed that all LAS files being read for the current run are from the same general area and the WKT used for this file will be used for all subsequent files.&lt;br&gt;&lt;br&gt;If you do not enter WKT data and click the &lt;b&gt;Close&lt;/b&gt; button, &lt;b&gt;%1&lt;/b&gt; will be terminated.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="632"/>
        <source>Click &lt;b&gt;Help&lt;/b&gt; button then click here for an even more verbose explanation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="648"/>
        <location filename="LAS_WKT.cpp" line="649"/>
        <source>Select a previously used WKT string (hover to see WKT in tool tip)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="693"/>
        <source>Enter the Well-known Text (WKT) for the LAS input file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="694"/>
        <source>Enter the Well-known Text (WKT) coordinate system definition for the LAS input file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="695"/>
        <source>Well-known Text (WKT)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="705"/>
        <source>Enter What&apos;s This mode for help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="711"/>
        <source>Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="712"/>
        <source>Discard WKT text, close this dialog, and exit the program</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="716"/>
        <source>Accept</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="717"/>
        <source>Accept the WKT text that has been entered, close this dialog, and process the files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="769"/>
        <source>The WKT data entered does not appear to be correct and/or supported.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKT.cpp" line="829"/>
        <source>The WKT text input field is empty.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LAS_WKTHelp.hpp" line="41"/>
        <source>&lt;b&gt;&lt;/b&gt;So, the problem with many LAS files is that the coordinate system is not defined properly.  Even though the LAS specification requires a 34735 (GeoKeyDirectoryTag) Variable Length Record (VLR) many people don&apos;t put one in.  If you&apos;re using LAS 1.4 and you use point data formats above 5 you are required to insert a Well-known Text (WKT) VLR or Extended VLR (eVLR).  Apparently many people think this is a waste of time.  I find that rather interesting, especially with projected data.  For example, I recently saw some data from a LiDAR system in use in Japan that was in LAS 1.4 format using point data format 6.  This requires a WKT VLR or eVLR but there wasn&apos;t one in the file.  The global_encoding field was set to indicate that there was a 34735 VLR. That is an error.  There wasn&apos;t even a 34735 VLR anyway.  Since the file appeared to be UTM what we had was a whole bunch of measurements (northing and easting, presumably in meters) from nowhere (or possibly OZ).&lt;br&gt;&lt;br&gt;Even when people insert the 34735 record they often leave out important information.  For instance, I&apos;ve seen files that proudly proclaim that they are NAD83 UTM but then they don&apos;t put the zone information into the record.  Once again, meters (we hope) from nowhere.&lt;br&gt;&lt;br&gt;Another pet peeve of mine - why do people use any vertical measurement other than meters (or metres if you will)?  Measuring things based on the length of some king&apos;s body parts seems a bit old school.  Wait until you&apos;ve finished processing the data before you convert it to cubits or fathoms or furlongs or feet or yards for the great unwashed masses to gaze upon.  ABE assumes that the input elevations are in meters.  If the data in your LAS file isn&apos;t in meters it doesn&apos;t really matter.  Just look at the pretty numbers and say to yourself &lt;b&gt;&apos;self, that number is really feet even though it says meters&apos;&lt;/b&gt;.  If you&apos;re building PFMs from LAS files I wouldn&apos;t recommend trying to extract (pfmExtract or pfmGeotiff) the data (meaning X, Y, and Z, not validity and/or classification).&lt;br&gt;&lt;br&gt;Now, on to the fun stuff, if you&apos;re not familiar with WKT you can look it up on the interwebs.  To quote Wikipedia &lt;b&gt;&apos;Well-known text (WKT) is a text markup language for representing vector geometry objects on a map, spatial reference systems of spatial objects and transformations between spatial reference systems&apos;&lt;/b&gt;. The world has moved past the GeoKeyDirectoryTag and is moving to WKT. Since this program uses the GDAL/OGR library it only supports projected (PROJCS) or geographic (GEOGCS) coordinate systems.  That&apos;s probably fine since that&apos;s what most people use (the other possibility being (GEOCCS) geocentric).&lt;br&gt;&lt;br&gt;This program will try to decipher the crazy quilt LAS oordinate system information but it won&apos;t try very hard.  If you are using geographic data it will probably make it through fine (unless you&apos;ve completely hosed or forgotten your 34735).  Projected data is much more complicated. In any event, if the program can&apos;t figure it out it will ask you for a WKT describing the coordinate system for your data.  You can find a bunch of WKT over on &lt;a href=&quot;http://spatialreference.org/&quot;&gt;spatialreference.org&lt;/a&gt;.&lt;br&gt;&lt;br&gt;Once you&apos;ve entered one or more WKT definitions the program will try to keep track of the 10 most recently used ones. These will be available in any of the ABE programs that have LAS inputs.  Hopefully you&apos;ll be using the same few for quite a while before you have to change to another area and/or coordinate system.  To see what each recent WKT string looks like just open the combo box on the bottom left and let the cursor hover over each entry.  The tool tip will be the full text of the WKT in human readable form.&lt;br&gt;&lt;br&gt;&lt;b&gt;IMPORTANT NOTE: In the pfmLoad or pfmLoader program, if you input LAS files and have to input WKT to load them, the program will keep track of the WKT and will use it again if you append to the existing PFM.&lt;/b&gt;&lt;br&gt;&lt;br&gt;Jan Depner, 3 May, 2016</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>pfmLoader</name>
    <message>
        <location filename="load_file.cpp" line="206"/>
        <source>%1% processed        </source>
        <oldsource>%1% processed        
</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="369"/>
        <location filename="load_file.cpp" line="455"/>
        <location filename="load_file.cpp" line="1021"/>
        <location filename="load_file.cpp" line="1266"/>
        <location filename="load_file.cpp" line="1763"/>
        <location filename="load_file.cpp" line="2682"/>
        <source>

Unable to open file %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="491"/>
        <source>Error reading record - %1
Record %2 skipped
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="508"/>
        <source>
For file %1
nominal depth requested, true depth used.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="591"/>
        <source>

File %1 is not a .hof file.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="600"/>
        <source>

Unable to open the CHARTS .hof file.
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1050"/>
        <source>

File %1 is not a .tof file.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1059"/>
        <source>

Unable to open the CHARTS .tof file.
%1 : %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1298"/>
        <location filename="load_file.cpp" line="1388"/>
        <source>

*** ERROR ***
Unable to open LAS file %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1430"/>
        <source>

*** ERROR ***
Reading point record %1 in %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1588"/>
        <source>

Unable to open the LLZ file.
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1596"/>
        <source>

LLZ file version corrupt or %1 is not an LLZ file.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="1787"/>
        <location filename="load_file.cpp" line="1807"/>
        <location filename="load_file.cpp" line="1899"/>
        <location filename="load_file.cpp" line="1920"/>
        <source>

Unable to open file
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2491"/>
        <source>

Unable to open the DTED file.
%1 : %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2500"/>
        <source>

%1 is not a DTED file.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2599"/>
        <source>READ ERROR %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2708"/>
        <source>

Unable to open the CHRTR2 file.
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2720"/>
        <source>

Unable to open the CHRTR file.
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="2883"/>
        <location filename="load_file.cpp" line="2945"/>
        <location filename="load_file.cpp" line="3450"/>
        <source>

Unable to open the file %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3011"/>
        <source>

Unable to open the BAG file %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3018"/>
        <source>

Error opening the BAG file
%1 : %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3159"/>
        <source>

BAG file %1, can&apos;t import from WKT:
%2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3168"/>
        <source>

BAG file %1, can&apos;t export to Proj4:
%2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3181"/>
        <source>

BAG file %1, error initializing UTM projection
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3201"/>
        <source>

BAG file %1, error initializing latlon projection
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3214"/>
        <location filename="load_file.cpp" line="3283"/>
        <source>

BAG file %1 not Geodetic or UTM projection.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3248"/>
        <source>Error initializing UTM projection
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="load_file.cpp" line="3270"/>
        <source>Error initializing latlon projection
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="76"/>
        <source>
Usage: pfmLoader PFM_LOAD_PARAMETERS_FILE

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="473"/>
        <source>Unable to open file %1
Reason %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="480"/>
        <source>Unable to determine file type for %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="525"/>
        <source>Checking file %1 of %2 - %3                                   
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="532"/>
        <source>Preliminary file check complete                                                                                                

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="750"/>
        <source>

No input files need to be loaded.  Terminating.

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="763"/>
        <source>
Unable to update the parameter file!

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="856"/>
        <source>Initializing PFM structure %1 of %2 - %3                                                 
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="899"/>
        <source>Creating PFM checkpoint file %1 of %2 - %3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1177"/>
        <source>PFM structure initialization complete

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1202"/>
        <source>#
#
# Date : %1  (GMT)
#
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1293"/>
        <source># File : %1
# Total data points read = %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1300"/>
        <source>Total data points read = %1 for file %2                                         
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1323"/>
        <source>Reading SRTM data

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1337"/>
        <location filename="pfmLoader.cpp" line="1366"/>
        <source>

Best available resolution SRTM data
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1342"/>
        <location filename="pfmLoader.cpp" line="1371"/>
        <source>

1 second resolution SRTM data
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1347"/>
        <location filename="pfmLoader.cpp" line="1376"/>
        <source>

3 second resolution SRTM data
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1352"/>
        <location filename="pfmLoader.cpp" line="1381"/>
        <source>

30 second resolution SRTM data
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1356"/>
        <location filename="pfmLoader.cpp" line="1385"/>
        <source>Total data points read = %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1391"/>
        <source>No SRTM data loaded
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1405"/>
        <source># --------------------------------------
# Finished with data load
# --------------------------------------
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1428"/>
        <source>
Using the following WKT:

</source>
        <oldsource>
Using the following WKT for LAS data:

</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1484"/>
        <source>An error occurred while trying to recompute the bin surfaces for file %1
The error message was : %2
Please try running pfm_recompute on the file to correct the problem.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1532"/>
        <source>Filtering bins in PFM %1 of %2 - %3
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1538"/>
        <source>Recomputing bins in PFM %1 of %2 - %3
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1645"/>
        <source>%1% processed</source>
        <oldsource>%1% processed
</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1655"/>
        <source>Filtering bins - complete

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1660"/>
        <source>Recomputing bins - complete

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1688"/>
        <source>Error reading the handle file %1 in file %2, function %3, line %4.  This shouldn&apos;t happen!
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1765"/>
        <source>

Unable to create backup list file %1.

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1784"/>
        <source># --------------------------------------
# PFM Filter Statistics
# Total number of points processed %1
# Number of soundings Good     : %2
# Percent of soundings Good     : %3
# Number of soundings Filtered : %4
# Percent of soundings Filtered : %f
# --------------------------------------
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1803"/>
        <source># --------------------------------------
# Finished with recompute section
# --------------------------------------
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1815"/>
        <source>
#**** WARNING ****
#File %1 :
#%2 soundings out of min/max range!
#These will be marked invalid in the input files
#if you unload this file!
#*****************
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfmLoader.cpp" line="1836"/>
        <source>
************************************ WARNING ***************************************
You have loaded LAZ files.
Changes to LAZ data can be unloaded but it will be slower than unloading LAS files
because the LAZ file must be uncompressed before unloading then re-compressed after.
************************************** WARNING ***************************************

</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
