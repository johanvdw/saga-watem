# 1 Introduction

The complete calculation of the erosion map in SAGA GIS consists of 4 steps:

![img1](/img/erosiekaart-img1.png)

1. Creation of the plot grid
2. Filtering of the DEM
3. Creation of the water erosion map (pixel map)
4. Adding water erosion map values to the plot map.

All steps can be performed in SAGA GIS. For the first 3 a separate module was 
developed, for the last step (adding erosion map to plot map) an existing 
module is used.
The creation of the water erosion map itself is conceived in 5 different steps, which can be performed together, but also separately:

1. The calculation of the upslope area
2. Calculation of the LS factor
3. The calculation of the C factor
4. The calculation of the water erosion
5. Calculation of the machining erosion (optional: If no value is given here this calculation is not performed).

For more detail on the parameters and input layers used, please refer to the 
final reports of the "Potential soil erosion maps by plot" (Oorts et al., 2019).

# 2 Preprocessing

## 2.1 ELEVATION MODEL CREATION

The elevation model used is the DHMv2 from Agency Information Flanders. The different 
files were pasted together using the mosaic tool: Grid mosaic
The default options were used (the extent of this mosaic is also the extent that was further 
used).

Optionally, this input grid can also be filtered. This can be done with the Grid Filter tool 
(smooth filter with radius 2 and square kernel). This method was abandoned in the final creation 
of the erosion map to proceed with a grid filter per plot.

## 2.2 CREATION OF K-GRID

The K-map consists of the digitised version of the polygon map based on the digital 
soil map_2_0. Following K-values were used for the texture class from the soil map:

| Texture | K-value       | information                                                  |
| ------- | ------------- | ------------------------------------------------------------ |
| A       | 0,04200       |                                                              |
| E       | 0,02750       |                                                              |
| L       | 0,04000       |                                                              |
| O       | Interpolation |                                                              |
| P       | 0,02500       |                                                              |
| S       | 0,02000       |                                                              |
| U       | 0,02250       | From erosion map2018 onwards, for all of the erosion map, the value 0.02250 was used. In the erosion map 2016, the value of 0.02750 used in the eastern K-map; In the western K map of 2016, the value was indeed 0,02250. In the 2017 erosion map, this meant the value was for all of Flanders, the value 0.02750 was used. |
| Z       | 0,01200       |                                                              |

Where there is missing data, it was filled in with an nearest neighbour algorithm.
In SAGA, it is not directly possible to use nearest neighbour within a grid. 
Therefore, we convert the grid to a point file. If we were to do this at 5m, this becomes 
quickly become much too slow, however, and therefore a 25m grid is used for interpolation. The 
original grid is subtracted from this afterwards.

- The creation of the K grid was done in the following way:
- Import K-map from the postgresql working database soil (production): take view 
  'k_metzeepolders' under schema 'soil map_derived
- Shapes to grid - using the extent of the Digtaal elevation model
  - Tools Grid\Gridding\Shapes to Grid
- Convert grid to 25m x 25m
  - Tools\Grid\Resampling
    - Upscaling method: Majority - this ensures that a single edge pixel does not affect the 
      does not affect the map too much
- Border outside Belgium artificially set to a known value: All values located more 
  than 3,000 metres outside Belgium were set to K-value -99. That way 
  these values are not interpolated. This in order to distinguish between 
  between unknown values (no data) that have to be interpolated and values for which 
  for which a K-value is deliberately not calculated. After interpolation, the no-data 
  value is set to -99, and values outside Belgium are therefore not used further.
- Convert the grid 25x25 metres to points. Grid values to points - without NA values
  Tools Shapes\Grid Tools\Add Grid Values to Points
- Interpolation on a 5m grid: the points obtained above are interpolated back 
  interpolated at 5m (although the points at which it is determined are thus at 25m).
  - Grid\Gridding\Interpolation from Points 
    - Select Nearest Neighbour
- Set No-data value of this grid to -99 (to omit areas outside Belgium).
- Lay original grid over interpolated grid (grid patching). Where values are effectively 
  are known, we obviously want to work with the known values and not with the 
  interpolated values.
  - Tools\Grid\Patching
    - Grid: original grid
    - Patch grid: the interpolated grid

## 2.3 CREATION OF LAND-USE MAP

The land use map is created based on the reclassified land use map with 
forest pointer of the sediment model (file 'Land use_forest pointer_reclassified'). The 
reclassification of the 'Land use_forest pointer' file is done using the 'reclassify grid 
values' and the file 'reclasslanduse' (shown below). You can find this tool under 'Tool > 
Grid > Tools > Reclassify Grid Values'.

![img2](/img/erosiekaart-img2.png)

Figure 3: creation of land-use map

![img3](/img/erosiekaart-img3.png)

| minimum   | maximum   | new         |
| --------- | --------- | ----------- |
| -6.100000 | -5.900000 | 1.000000    |
| -3.100000 | -2.900000 | 1000.000000 |
| -4.100000 | -3.900000 | 1.000000    |
| -5.100000 | -4.900000 | -1.000000   |
| 9.900000  | 10.100000 | 1.000000    |

Note: value -2 remains -2 in the new map and is therefore not converted.

## 2.4 CREATION OF THE PARCEL GRID (PRC MAP) (MODULE 1.)

![img4](/img/erosiekaart-img4.png)

The plot grid can be created in SAGA using the module '1. 
(PRC)' (see 1.1.2.1 Creating plot grid (PRC)).
The plot grid is created from the files listed below. The 
order used is this: (where later map layers are superimposed on previous map layers)

- Land use map 'Land use_forest_reclassified' 

  - 10000: forest

  - 1: other land use

  - -2: built-up 

- GRB layers(geopoint - dataset GRBgis)

  - GBG (building to land), GBA (building attachment), WGA (road attachment), 
    KNW (structure), TRN (terrain): built-up (-2)

- Parcel map

  - Get values between 2 and 9999

- Waterways and roads

  - SBN (railway line), WBN (road line) (-2)
  - WLas (VHA lines) [For the erosion map 2018, the shape 'VHA_09052017.shp' was used 
    was used and not WLas from GRB (because VHA was more recent)], WTZ (VHA polygons) 
    (-1)

![img5](/img/erosiekaart-img5.png)

Figure 4: module '1. Creation of parcel grid (PRC)'.

## 2.5 3X3 FILTER WITHIN PLOT BOUNDARIES (MODULE 2.)

![img6](/img/erosiekaart-img6.png)

A second optional step is to perform a 3x3 filter on the grid. This filters a digital elevation model but only considers cells that lie within the same plot. This filter can be done in SAGA using the tool '2. 3x3 filter within plot boundaries' (see 1.1.2.2 3x3 filter within plot boundaries). This option is on by default because in 2017 it was decided to use this option to be used starting from the calculation of the erosion map 2018.

![img7](/img/erosiekaart-img7.png)

Figure 5: Module 2. 3x3 filter within plot boundaries

# 3 Run in GUI

## 3.1 STARTING WITH THE WATEM MODULE

In SAGA GIS, all modules can be called from the "Geoprocessing" menu, in the 'Watem' submenu.

![img8](/img/erosiekaart-img8.png)

Figure 1: Calling the Watem module from the menu

It is also possible to launch the same modules from the tools tab in the "manager" window. The description tab then also displays the module's documentation.

![img9](/img/erosiekaart-img9.png)

Figure 2: Calling up the Watem module via the "manager" window

The import grids are displayed in the interface with ">>" This means that they are mandatory for import. Output grids (or shapefiles) start by default with "<<" for mandatory output and "<" for optional output.
