AMCS/CS247 Scientific Visualization
Programming Assignment #4 Data Info

The vector data is loaded into vector_array, components of one vector is stored in sequence x y z. the grid contains 2D slices of vector fields at different time steps.

When a dataset is loaded, its resolution, number of time steps, and number
of contained scalar fields are displayed.

You can download vector fields that the framework can load here:

1. block:
   http://www.cg.tuwien.ac.at/courses/Visualisierung/data/blockData.zip

   Grid size is 314 x 538. 1 time step (i.e., not time-dependent)

   In addition to the vector field, this dataset also contains 2 scalar fields. They are loaded automatically by the framework.

   The two scalar fields are pressure, and vorticity, respectively.

   Description:
   http://www.cg.tuwien.ac.at/courses/Visualisierung/Angaben/README_BSP2

2. hurricane:
   http://www.cg.tuwien.ac.at/courses/Visualisierung/data/hurricane_timeseries.zip

   The original dataset is 3D, but the link above already
   gives you extracted 2D slices for 2D flow visualization.

   Grid size is 500 x 500. 48 time steps.

   In addition to the vector field, this dataset also contains 2 scalar fields. They are loaded automatically by the framework.

   The two scalar fields are temperature, and cloud moisture mixing ratio (kg water/kg dry air), respectively.

   Originally computed by http://www.ucar.edu/
   This is a simulation of Hurricane Isabel from 2003.

   More information on the particular version of the dataset that we took this from is available here:
   http://vis.computer.org/vis2004contest/data.html

   Two indidivdual, extracted time steps for testing are also available:

   http://www.cg.tuwien.ac.at/courses/Visualisierung/data/hurricane10.zip
   http://www.cg.tuwien.ac.at/courses/Visualisierung/data/hurricane48.zip

3. tube:
   http://www.cg.tuwien.ac.at/courses/Visualisierung/data/tube.zip

   Grid size is 599 x 58. 19 time steps.

   In addition to the vector field, this dataset also contains 2 scalar fields. They are loaded automatically by the framework.

   The two scalar fields are pressure, and vorticity, respectively.


In order for the framework to find (and load) these datasets, they have to be in
subdirectories "block", "hurricane", and "tube", respectively.

